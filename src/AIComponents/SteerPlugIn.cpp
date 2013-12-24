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
 * \file /Ely/src/AIComponents/SteerPlugIn.cpp
 *
 * \date 04/dic/2013 (09:11:38)
 * \author consultit
 */

#include "AIComponents/SteerPlugIn.h"
#include "AIComponents/SteerPlugInTemplate.h"
#include "AIComponents/OpenSteerLocal/PlugIn_OneTurning.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"

namespace ely
{

SteerPlugIn::SteerPlugIn()
{
}

SteerPlugIn::SteerPlugIn(SMARTPTR(SteerPlugInTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
	"OpenSteerPlugIn::OpenSteerPlugIn: invalid GameAIManager")

	mTmpl = tmpl;
	reset();
}

SteerPlugIn::~SteerPlugIn()
{
}

ComponentFamilyType SteerPlugIn::familyType() const
{
	return mTmpl->familyType();
}

ComponentType SteerPlugIn::componentType() const
{
	return mTmpl->componentType();
}

bool SteerPlugIn::initialize()
{
	bool result = true;
	//set SteerPlugIn parameters (store internally for future use)
	//type
	std::string type = mTmpl->parameter(std::string("plugin_type"));
	if (type == std::string(""))
	{
	}
	else if (type == std::string(""))
	{
	}
	else
	{
		//default: OneTurningPlugIn
		mPlugIn = new OneTurningPlugIn<SteerVehicle>;
	}
	//
	return result;
}

void SteerPlugIn::onAddToObjectSetup()
{
	mPlugIn->open();
}

void SteerPlugIn::onRemoveFromObjectCleanup()
{

	//remove all handled SteerVehicles (if any) from update
	std::set<SMARTPTR(SteerVehicle)>::const_iterator iter;
	for (iter = mSteerVehicles.begin(); iter != mSteerVehicles.end();
			++iter)
	{
		//set steerVehicle reference to null
		(*iter)->mSteerPlugIn.clear();
		//do remove from real update list
		dynamic_cast<PlugIn*>(mPlugIn)->removeVehicle(&(*iter)->getAbstractVehicle());
	}

#ifdef ELY_DEBUG
	if (not mDebugCamera.is_empty())
	{
		//set the recast debug camera to empty node path
		mDebugCamera = NodePath();
		//remove the recast debug node paths
		mDrawer3dNP.remove_node();
		mDrawer2dNP.remove_node();
	}
	//delete the DebugDrawers
	delete mDrawer3d;
	delete mDrawer2d;
#endif
	//
	delete mPlugIn;
	reset();
}

void SteerPlugIn::onAddToSceneSetup()
{
	//Add to the AI manager update
	GameAIManager::GetSingletonPtr()->addToAIUpdate(this);

#ifdef ELY_DEBUG
	mDrawer3dNP = ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(
			"render")->getNodePath().attach_new_node(
			"Drawer3dNP_" + COMPONENT_STANDARD_NAME);
	mDrawer2dNP = ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(
			"aspect2d")->getNodePath().attach_new_node(
			"Drawer2dNP_" + COMPONENT_STANDARD_NAME);

	//get the camera object
	SMARTPTR(Object)cameraDebug = ObjectTemplateManager::GetSingletonPtr()->
	getCreatedObject("camera");
	if (cameraDebug)
	{
		//set debug node paths
		mDrawer3dNP.set_bin("fixed", 10);
		mDrawer2dNP.set_bin("fixed", 10);
		//by default Debug NodePaths are hidden
		mDrawer3dNP.hide();
		mDrawer2dNP.hide();
		//set the recast debug camera to the first child of "camera" node path
		mDebugCamera = cameraDebug->getNodePath().get_child(0);
	}
	//create new Debug Drawers
	mDrawer3d = new DrawMeshDrawer(mDrawer3dNP, mDebugCamera, 100, 0.04);
	mDrawer2d = new DrawMeshDrawer(mDrawer2dNP, mDebugCamera, 50, 0.04);
#endif //ELY_DEBUG
}

void SteerPlugIn::onRemoveFromSceneCleanup()
{
	//remove from AI manager update
	GameAIManager::GetSingletonPtr()->removeFromAIUpdate(this);
}

SteerPlugIn::Result SteerPlugIn::addSteerVehicle(SMARTPTR(SteerVehicle)steerVehicle)
{
	RETURN_ON_COND(not steerVehicle,false)

	bool result;
	//lock (guard) the SteerVehicle SteerPlugIn mutex
	HOLD_REMUTEX(steerVehicle->mSteerPlugInMutex)
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

		{
			//lock (guard) the steerVehicle mutex
			HOLD_REMUTEX(steerVehicle->mMutex)

			//return if steerVehicle is destroying or already belongs to any plug in
			RETURN_ON_ASYNC_COND(steerVehicle->mDestroying, Result::Result::ERROR)
			RETURN_ON_COND(steerVehicle->mSteerPlugIn, Result::ERROR)

			//add to the set of SteerVehicles
			mSteerVehicles.insert(steerVehicle);
			//do add to real update list
			dynamic_cast<PlugIn*>(mPlugIn)->addVehicle(&steerVehicle->getAbstractVehicle());
			//set steerVehicle reference to this plugin
			steerVehicle->mSteerPlugIn = this;
		}
	}
	//
	return (result = true ? Result::OK:Result::ERROR);
}

SteerPlugIn::Result SteerPlugIn::removeSteerVehicle(SMARTPTR(SteerVehicle)steerVehicle)
{
	RETURN_ON_COND(not steerVehicle, Result::ERROR)

	//lock (guard) the SteerVehicle SteerPlugIn mutex
	HOLD_REMUTEX(steerVehicle->mSteerPlugInMutex)
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

		{
			//lock (guard) the steerVehicle mutex
			HOLD_REMUTEX(steerVehicle->mMutex)

			//return if steerVehicle is destroying or doesn't belong to this plug in
			RETURN_ON_ASYNC_COND(steerVehicle->mDestroying, Result::Result::ERROR)
			RETURN_ON_COND(steerVehicle->mSteerPlugIn != this, Result::ERROR)

			//set steerVehicle reference to null
			steerVehicle->mSteerPlugIn.clear();
			//do remove from real update list
			dynamic_cast<PlugIn*>(mPlugIn)->removeVehicle(&steerVehicle->getAbstractVehicle());
			//remove from the set of SteerVehicles
			mSteerVehicles.erase(steerVehicle);
		}
	}
	//
	return Result::OK;
}

void SteerPlugIn::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//elapsedTime
	float dt = *(reinterpret_cast<float*>(data));
	//currentTime
	mCurrentTime += dt;

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

#ifdef ELY_DEBUG
	{
		//lock (guard) the Drawers' mutex
		HOLD_REMUTEX(gOpenSteerDebugMutex)

		if (mEnableDebugDrawUpdate)
		{
			//unset enableAnnotation
			enableAnnotation = true;

			//set drawers
			mDrawer3d->reset();
			mDrawer2d->reset();
			gDrawer3d = mDrawer3d;
			gDrawer2d = mDrawer2d;

//			// service queued reset request, if any
//			if (OpenSteer::gDelayedResetPlugInXXX)
//			{
//				mPlugIn->reset();
//				OpenSteer::gDelayedResetPlugInXXX = false;
//			}

			// invoke PlugIn's Update method
			mPlugIn->update(mCurrentTime, dt);

			// invoke selected PlugIn's Redraw method
			mPlugIn->redraw(mCurrentTime, dt);
			// draw any annotation queued up during selected PlugIn's Update method
			OpenSteer::drawAllDeferredLines();
			OpenSteer::drawAllDeferredCirclesOrDisks();
		}
		else
		{
			//unset enableAnnotation
			enableAnnotation = false;
#endif

//			// service queued reset request, if any
//			if (OpenSteer::gDelayedResetPlugInXXX)
//			{
//				mPlugIn->reset();
//				OpenSteer::gDelayedResetPlugInXXX = false;
//			}

			// invoke PlugIn's Update method
			mPlugIn->update(mCurrentTime, dt);

#ifdef ELY_DEBUG
		}
	}
#endif
}

#ifdef ELY_DEBUG
SteerPlugIn::Result SteerPlugIn::debug(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if mDrawer3dNP or mDrawer2dNP is empty
	RETURN_ON_COND(mDrawer3dNP.is_empty() or mDrawer2dNP.is_empty(), Result::ERROR)

	if (enable)
	{
		if (mDrawer3dNP.is_hidden())
		{
			mDrawer3dNP.show();
		}
		if (mDrawer2dNP.is_hidden())
		{
			mDrawer2dNP.show();
		}
	}
	else
	{
		if (not mDrawer3dNP.is_hidden())
		{
			mDrawer3dNP.hide();
		}
		if (not mDrawer2dNP.is_hidden())
		{
			mDrawer2dNP.hide();
		}
	}
	//set Debug Draw Update
	mEnableDebugDrawUpdate = enable;
	//clear drawers
	mDrawer3d->clear();
	mDrawer2d->clear();
	//
	return Result::OK;
}
#endif

//TypedObject semantics: hardcoded
TypeHandle SteerPlugIn::_type_handle;

} /* namespace ely */
