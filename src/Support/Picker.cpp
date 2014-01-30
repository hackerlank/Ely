/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/src/Support/Picker.cpp
 *
 * \date 26/dic/2012 (10:58:28)
 * \author consultit
 */

#include "Support/Picker.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "SceneComponents/NodePathWrapper.h"
#include "Game/GamePhysicsManager.h"
#include <mouseWatcher.h>

#if defined (ELY_DEBUG) && !defined (TESTING)
#	define PRINT_DEBUG_HIT \
		std::cout << result.get_node()->get_type().get_name() << \
		"| panda node: " << result.get_node()->get_name() << \
		"| hit pos: " << "\"" \
			<< result.get_hit_pos().get_x() << "," \
			<< result.get_hit_pos().get_y() << "," \
			<< result.get_hit_pos().get_z() \
			<< "\"" << \
		"| hit normal: " << "\"" \
			<< result.get_hit_normal().get_x() << "," \
			<< result.get_hit_normal().get_y() << "," \
			<< result.get_hit_normal().get_z() \
			<< "\"" << \
		"| hit fraction: " << "\"" \
			<< result.get_hit_fraction() \
			<< "\"" << \
		"| from pos: " << "\"" \
			<< result.get_from_pos().get_x() << "," \
			<< result.get_from_pos().get_y() << "," \
			<< result.get_from_pos().get_z() \
			<< "\"" << \
		"| to pos: " << "\"" \
			<< result.get_to_pos().get_x() << "," \
			<< result.get_to_pos().get_y() << "," \
			<< result.get_to_pos().get_z() \
			<< "\"" << std::endl
#endif

namespace ely
{

Picker::Picker(PandaFramework* app, WindowFramework* window,
		const std::string& pickKeyOn, const std::string& pickKeyOff) :
		mApp(app), mWindow(window)
{
	//some preliminary checks
	CHECK_EXISTENCE_DEBUG(mApp, "Picker::Picker: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(mWindow, "Picker::Picker: invalid WindowFramework")
	SMARTPTR(Object)render =
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(ObjectId("render"));
	CHECK_EXISTENCE_DEBUG(render, "Picker::Picker: invalid render object")
	SMARTPTR(Object)camera =
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(ObjectId("camera"));
	CHECK_EXISTENCE_DEBUG(camera, "Picker::Picker: invalid camera object")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(), "Picker::Picker: "
			"invalid GamePhysicsManager")
	//get bullet world reference
	mWorld = GamePhysicsManager::GetSingletonPtr()->bulletWorld();
	//get render, camera node paths
	if (render->getComponent(ComponentFamilyType("Scene"))->componentType()
			== ComponentType("NodePathWrapper"))
	{
		mRender = DCAST(NodePathWrapper,
				render->getComponent(ComponentFamilyType("Scene")))->getNodePath();
	}
	if (camera->getComponent(ComponentFamilyType("Scene"))->componentType()
			== ComponentType("NodePathWrapper"))
	{
		mCamera = DCAST(NodePathWrapper,
				camera->getComponent(ComponentFamilyType("Scene")))->getNodePath();
		mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
	}
	//reset picking logic data
	mCsPick.clear();
	mPivotPos = LPoint3f::zero();
	// setup event callback for picking body
	mPickKeyOn = pickKeyOn;
	mPickKeyOff = pickKeyOff;
	mPickBodyData = new EventCallbackInterface<Picker>::EventCallbackData(this,
			&Picker::pickBody);
	mApp->define_key(mPickKeyOn, "pickBody",
			&EventCallbackInterface<Picker>::eventCallbackFunction,
			reinterpret_cast<void*>(mPickBodyData.p()));
	mApp->define_key(mPickKeyOff, "pickBodyUp",
			&EventCallbackInterface<Picker>::eventCallbackFunction,
			reinterpret_cast<void*>(mPickBodyData.p()));
	// setup task for moving picked (body)
	mMovePickedData = new TaskInterface<Picker>::TaskData(this,
			&Picker::movePicked);
	mMovePickedTask = new GenericAsyncTask("move picked task",
			&TaskInterface<Picker>::taskFunction,
			reinterpret_cast<void*>(mMovePickedData.p()));
	mApp->get_task_mgr().add(mMovePickedTask);
}

Picker::~Picker()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mApp)
	{
		// remove task for moving picked (body)
		if (mMovePickedTask)
		{
			mApp->get_task_mgr().remove(mMovePickedTask);
		}
		// remove event callback for picking body
		mApp->get_event_handler().remove_hooks_with(
				reinterpret_cast<void*>(mPickBodyData.p()));
	}
}

void Picker::pickBody(const Event* event)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	// handle body picking
	if (event->get_name() == mPickKeyOn)
	{
		//get the mouse watcher
		SMARTPTR(MouseWatcher)mwatcher = DCAST(MouseWatcher,
				mWindow->get_mouse().node());
		if (mwatcher->has_mouse())
		{
			// Get to and from pos in camera coordinates
			LPoint2f pMouse = mwatcher->get_mouse();
			//
			LPoint3f pFrom, pTo;
			if (mCamLens->extrude(pMouse, pFrom, pTo))
			{
				//Transform to global coordinates
				pFrom = mRender.get_relative_point(mCamera, pFrom);
				pTo = mRender.get_relative_point(mCamera, pTo);
				//cast a ray to detect a body
				BulletClosestHitRayResult result = mWorld->ray_test_closest(pFrom, pTo,
						BitMask32::all_on());
				//
				if (result.has_hit())
				{
					//possible hit objects:
					//- BulletRigidBodyNode
					//- BulletCharacterControllerNode
					//- BulletVehicle
					//- BulletConstraint
					//- BulletSoftBodyNode
					//- BulletGhostNode
					if (result.get_node()->is_of_type(BulletRigidBodyNode::get_class_type()))
					{
						mPickedBody = DCAST(BulletRigidBodyNode,
								const_cast<PandaNode*>(result.get_node()));
						if (not(mPickedBody->is_static() or mPickedBody->is_kinematic()))
						{
							//
							mPickedBody->set_active(true);
							mPickedBody->set_deactivation_enabled(false);
							mPivotPos = result.get_hit_pos();
							//
							NodePath bodyNP(mPickedBody);
							LPoint3f pivotLocalPos = bodyNP.get_relative_point(mRender, mPivotPos);
							//create constraint
							mCsPick = new BulletSphericalConstraint(
									mPickedBody,
									pivotLocalPos);
							//and attach it to the world
							mWorld->attach(mCsPick);
							//
							PRINT_DEBUG_HIT;
						}
						else
						{
							PRINT_DEBUG_HIT;
						}
					}
					else
					{
						PRINT_DEBUG_HIT;
					}
				}
			}
		}
	}
	else
	{
		if(not mCsPick.is_null())
		{
			//remove constraint from world
			mWorld->remove(mCsPick);
			//delete constraint
			mCsPick.clear();
			mPickedBody->set_deactivation_enabled(true);
			mPickedBody->set_active(false);
		}
	}
}

AsyncTask::DoneStatus Picker::movePicked(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	// handle picked body if any
	if (not mCsPick.is_null())
	{
		//get the mouse watcher
		SMARTPTR(MouseWatcher)mwatcher =
		DCAST(MouseWatcher, mWindow->get_mouse().node());
		if (mwatcher->has_mouse())
		{
			// Get to and from pos in camera coordinates
			LPoint2f pMouse = mwatcher->get_mouse();
			//
			LPoint3f pNear, pFar;
			if (mCamLens->extrude(pMouse, pNear, pFar))
			{
				// Transform to global coordinates
				pNear = mRender.get_relative_point(mCamera, pNear);
				pFar = mRender.get_relative_point(mCamera, pFar);
				// new pivot (b) pos
				LVector3f vecFarNear = pFar - pNear;
				LVector3f vecPivotNear = mPivotPos - pNear;
				mPivotPos = pNear + (vecFarNear / vecFarNear.length_squared()) *
				vecPivotNear.dot(vecFarNear);
				mCsPick->set_pivot_b(mPivotPos);
			}
		}
	}
	//
	return AsyncTask::DS_cont;
}

} // namespace ely
