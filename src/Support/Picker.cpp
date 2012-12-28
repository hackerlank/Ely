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
 * \author marco
 */

#include "Support/Picker.h"

Picker::Picker(PandaFramework* app, WindowFramework* window,
		const std::string& pickKeyOn, const std::string& pickKeyOff) :
		mApp(app), mWindow(window), mPickingBodyType("PickingBody"), mPickingBodyId(
				"pickingBody1")
{
	//some preliminary checks
	CHECKEXISTENCE(mApp, "Picker::Picker: invalid PandaFramework")
	CHECKEXISTENCE(mWindow, "Picker::Picker: invalid WindowFramework")
	SMARTPTR(Object)render =
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObject("render");
	CHECKEXISTENCE(render, "Picker::Picker: invalid render object")
	SMARTPTR(Object)camera =
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObject("camera");
	CHECKEXISTENCE(camera, "Picker::Picker: invalid camera object")
	CHECKEXISTENCE(GamePhysicsManager::GetSingletonPtr(), "Picker::Picker: "
	"invalid GamePhysicsManager")
	//get bullet world reference
	mWorld = GamePhysicsManager::GetSingletonPtr()->bulletWorld();
	//get render, camera node paths
	if (render->getComponent("Scene")->is_of_type(
			NodePathWrapper::get_class_type()))
	{
		mRender =
				DCAST(NodePathWrapper, render->getComponent("Scene"))->getNodePath();
	}
	if (camera->getComponent("Scene")->is_of_type(
			NodePathWrapper::get_class_type()))
	{
		mCamera =
				DCAST(NodePathWrapper, camera->getComponent("Scene"))->getNodePath();
		mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
	}
	//reset picking logic data
	mCsPick.clear();
	mPivotPos = LPoint3f::zero();
	///XXX: HOWTO create a picking body object:
	//1) create a "PickerType" ObjectTemplate
	SMARTPTR(ObjectTemplate)objTmplPtr = new ObjectTemplate(mPickingBodyType,
			ObjectTemplateManager::GetSingletonPtr(), app, mWindow);
	//2) add the component template for a RigidBody
	objTmplPtr->addComponentTemplate(
			ComponentTemplateManager::GetSingleton().getComponentTemplate(
					ComponentType("RigidBody")));
	//3) add "PickerType" object template to manager
	ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmplPtr);
	//4) fill component parameter table up
	ParameterTableMap compTmplParams;
	compTmplParams["RigidBody"].insert(
			ParameterTable::value_type("body_type", "kinematic"));
	compTmplParams["RigidBody"].insert(
			ParameterTable::value_type("body_mass", "0.0"));
	compTmplParams["RigidBody"].insert(
			ParameterTable::value_type("shape_type", "sphere"));
	compTmplParams["RigidBody"].insert(
			ParameterTable::value_type("shape_size", "minimum"));
	compTmplParams["RigidBody"].insert(
			ParameterTable::value_type("shape_radius", "0.1"));
	compTmplParams["RigidBody"].insert(
			ParameterTable::value_type("collide_mask", "all_off"));
	//5) fill object parameter table up
	ParameterTable objTmplParams;
	objTmplParams.insert(ParameterTable::value_type("parent", "render"));
	//6) create the picking body object actually
	SMARTPTR(Object)pickingBody = ObjectTemplateManager::GetSingletonPtr()->createObject(
			mPickingBodyType, mPickingBodyId, true, objTmplParams,
			compTmplParams, false);
	//get references to pickingBody RigidBody and BulletRigidBodyNode
	mPickingBody = DCAST(RigidBody,
			pickingBody->getComponent("Physics"));
	mPickingBodyNode =
			DCAST(BulletRigidBodyNode, mPickingBody->getNodePath().node());
	//and remove it from bullet world
	mWorld->remove(mPickingBodyNode);
	mPickingBodyAttached = false;
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
	HOLDMUTEX(mMutex)

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
		if (ObjectTemplateManager::GetSingletonPtr())
		{
			///XXX: check if PickingBody is attached to bullet world.
			if (not mPickingBodyAttached)
			{
				//re-attach to avoid bullet warning
				mWorld->attach(mPickingBodyNode);
			}
			// remove picking body
			ObjectTemplateManager::GetSingletonPtr()->removeCreatedObject(
					mPickingBodyId);
			// remove picking body object template
			ObjectTemplateManager::GetSingletonPtr()->removeObjectTemplate(
					mPickingBodyType);
		}
	}
}

void Picker::pickBody(const Event* event)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	// handle body picking
	if (event->get_name() == mPickKeyOn)
	{
		//get the mouse watcher
		SMARTPTR(MouseWatcher)mwatcher =
		DCAST(MouseWatcher, mWindow->get_mouse().node());
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
						SMARTPTR(BulletRigidBodyNode) pickedBody =
						DCAST(BulletRigidBodyNode,result.get_node());
						if (not(pickedBody->is_static() or pickedBody->is_kinematic()))
						{
							//attach bullet picking body node to world
							mWorld->attach(mPickingBodyNode);
							mPickingBodyAttached = true;
							//
							pickedBody->set_active(true);
							mPivotPos = result.get_hit_pos() +
							result.get_hit_normal() * 0.1;
							//
							NodePath bodyNP(pickedBody);
							LPoint3f pivotLocalPos = bodyNP.get_relative_point(mRender, mPivotPos);
							mPickingBody->getNodePath().set_pos(mPivotPos);
							//create constraint
							mCsPick = new BulletSphericalConstraint(
									mPickingBodyNode,
									pickedBody,
									LPoint3f::zero(),
									pivotLocalPos);
							//and attach it to the world
							mWorld->attach(mCsPick);
							PRINT(result.get_node()->get_type().get_name() <<
									"| panda node: " << result.get_node()->get_name() <<
									"| hit pos: " << result.get_hit_pos() <<
									"| hit normal: " << result.get_hit_normal() <<
									"| hit fraction: " << result.get_hit_fraction() <<
									"| from pos: " << result.get_from_pos() <<
									"| to pos: " << result.get_to_pos());
						}
						else
						{
							PRINT(result.get_node()->get_type().get_name() <<
									"| panda node: " << result.get_node()->get_name() <<
									"| hit pos: " << result.get_hit_pos() <<
									"| hit normal: " << result.get_hit_normal() <<
									"| hit fraction: " << result.get_hit_fraction() <<
									"| from pos: " << result.get_from_pos() <<
									"| to pos: " << result.get_to_pos());
						}
					}
					else
					{
						PRINT(result.get_node()->get_type().get_name() <<
								"| panda node: " << result.get_node()->get_name() <<
								"| hit pos: " << result.get_hit_pos() <<
								"| hit normal: " << result.get_hit_normal() <<
								"| hit fraction: " << result.get_hit_fraction() <<
								"| from pos: " << result.get_from_pos() <<
								"| to pos: " << result.get_to_pos());
					}
				}
			}
		}
	}
	else
	{
		if(not mCsPick.is_null())
		{
			//remove constraint and bullet picking body node from world
			mWorld->remove(mCsPick);
			mWorld->remove(mPickingBodyNode);
			mPickingBodyAttached = false;
			//delete constraint
			mCsPick.clear();
		}
	}
}

AsyncTask::DoneStatus Picker::movePicked(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
				// new pivot pos
				LVector3f vecFarNear = pFar - pNear;
				LVector3f vecPivotNear = mPivotPos - pNear;
				mPivotPos = pNear + (vecFarNear / vecFarNear.length_squared()) *
				vecPivotNear.dot(vecFarNear);
				mPickingBody->getNodePath().set_pos(mPivotPos);
			}
		}
	}
	//
	return AsyncTask::DS_cont;
}

ReMutex& Picker::getMutex()
{
	return mMutex;
}
