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
#include "AIComponents/OpenSteerLocal/PlugIn_Pedestrian.h"
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
	//plugin type
	mPlugInTypeParam = mTmpl->parameter(std::string("plugin_type"));
	//pathway
	mPathwayParam = mTmpl->parameter(std::string("pathway"));
	//obstacles
	mObstacleListParam = mTmpl->parameterList(std::string("obstacles"));
	//
	return result;
}

void SteerPlugIn::onAddToObjectSetup()
{
	//create the plug in
	if (mPlugInTypeParam == std::string("pedestrian"))
	{
		mPlugIn = new PedestrianPlugIn<SteerVehicle>;
	}
	else if (mPlugInTypeParam == std::string(""))
	{
	}
	else
	{
		//default: OneTurningPlugIn
		mPlugIn = new OneTurningPlugIn<SteerVehicle>;
	}
	//open the plug in
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
	//close the plug in
	mPlugIn->close();
	//delete the plug in
	delete mPlugIn;
	reset();
}

inline void SteerPlugIn::doBuildPathway()
{
	//
	std::vector<std::string> paramValues1Str, paramValues2Str, paramValues3Str;
	unsigned int idx, valueNum;
	//build pathway
	paramValues1Str = parseCompoundString(mPathwayParam, '$');
	valueNum = paramValues1Str.size();
	if (valueNum != 3)
	{
		//there aren't all mandatory parameters: set hardcoded defaults
		paramValues1Str = parseCompoundString(
				std::string("0.0,0.0,0.0:1.0,1.0,1.0$1.0$false"), '$');
	}
	//pathway::points (forced to at least 2)
	paramValues2Str = parseCompoundString(paramValues1Str[0], ':');
	valueNum = paramValues2Str.size();
	if (valueNum == 0)
	{
		paramValues2Str = parseCompoundString(
				std::string("0.0,0.0,0.0:1.0,1.0,1.0"), ':');
	}
	else if (valueNum == 1)
	{
		paramValues2Str.push_back(std::string("1.0,1.0,1.0"));
	}
	unsigned int numPoints = paramValues2Str.size();
	OpenSteer::Vec3* points = new OpenSteer::Vec3[numPoints];
	for (idx = 0; idx < numPoints; ++idx)
	{
		paramValues3Str = parseCompoundString(paramValues2Str[idx], ',');
		if (paramValues3Str.size() < 3)
		{
			paramValues3Str.resize(3, "0.0");
		}
		LVector3f values;
		for (unsigned int i = 0; i < 3; ++i)
		{
			values[i] = strtof(paramValues3Str[i].c_str(), NULL);
		}
		points[idx] = LVecBase3fToOpenSteerVec3(values);
	}
	//get pathway::closedCycle
	bool closedCycle =
			(paramValues1Str[2] == std::string("true") ? true : false);
	//get pathway::radii (forced to at least 1) and set pathway
	paramValues2Str = parseCompoundString(paramValues1Str[1], ':');
	valueNum = paramValues2Str.size();
	if (valueNum == 0)
	{
		paramValues2Str.push_back(std::string("1.0"));
	}
	unsigned int numRadii = paramValues2Str.size();	//radii specified
	if (numRadii == 1)
	{
		//single radius
		float radius = strtof(paramValues2Str[0].c_str(), NULL);
		if (radius < 0.0)
		{
			radius = -radius;
		}
		else if (radius == 0.0)
		{
			radius = 1.0;
		}
		//set pathway: single radius
		dynamic_cast<PlugIn*>(mPlugIn)->setPathway(points, true, &radius,
				closedCycle);
	}
	else
	{
		//several radii
		unsigned int numRadiiAllocated = (
				closedCycle ? numPoints : numPoints - 1);
		float *radii = new float[numRadiiAllocated];
		for (idx = 0; idx < numRadiiAllocated; ++idx)
		{
			float value;
			if (idx < numRadii - 1)
			{
				value = strtof(paramValues2Str[idx].c_str(), NULL);
				if (value < 0.0)
				{
					value = -value;
				}
				else if (value == 0.0)
				{
					value = 1.0;
				}
			}
			else
			{
				//radii allocated > radii specified
				value = 1.0;
			}
			radii[idx] = value;
		}
		//set pathway: several radius
		dynamic_cast<PlugIn*>(mPlugIn)->setPathway(points, false, radii,
				closedCycle);
		delete[] radii;
	}
}

inline void SteerPlugIn::doAddObstacles()
{
	//
	std::vector<std::string> paramValues1Str;
	//add obstacles
	std::list<std::string>::iterator iterList;
	for (iterList = mObstacleListParam.begin();
			iterList != mObstacleListParam.end(); ++iterList)
	{
		//any "obstacles" string is a "compound" one, i.e. could have the form:
		// "objectId1@shape1@seenFromState1:objectId2@shape2@seenFromState2:...:objectIdN@shapeN@seenFromStateN"
		paramValues1Str = parseCompoundString(*iterList, ':');
		std::vector<std::string>::const_iterator iter;
		for (iter = paramValues1Str.begin(); iter != paramValues1Str.end();
				++iter)
		{
			//any obstacle string should have the form: "objectId@shape@seenFromState"
			if (paramValues1Str.size() != 3)
			{
				continue;
			}
			//get obstacle object
			SMARTPTR(Object)obstacleObject =
			ObjectTemplateManager::GetSingleton().getCreatedObject(
					paramValues1Str[0]);
			if (not obstacleObject)
			{
				continue;
			}
			//get seenFromState (default = both)
			OpenSteer::AbstractObstacle::seenFromState seenFromState;
			if (paramValues1Str[2] == std::string("outside"))
			{
				seenFromState = OpenSteer::AbstractObstacle::outside;
			}
			else if (paramValues1Str[2] == std::string("inside"))
			{
				seenFromState = OpenSteer::AbstractObstacle::inside;
			}
			else
			{
				seenFromState = OpenSteer::AbstractObstacle::both;
			}
			//get obstacle dimensions
			NodePath obstacleNP = obstacleObject->getNodePath();
			LVector3 modelDims, modelDeltaCenter;
			float modelRadius;
			GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
					obstacleNP, modelDims, modelDeltaCenter, modelRadius);
			//get obstacle position/orientation (wrt reference node path)
			LPoint3f pos = obstacleNP.get_pos(mReferenceNP);
			LVector3f forward = mReferenceNP.get_relative_vector(obstacleNP,
					LVector3f::forward());
			LVector3f up = mReferenceNP.get_relative_vector(obstacleNP,
					LVector3f::up());
			LVector3f side = mReferenceNP.get_relative_vector(obstacleNP,
					LVector3f::right());
			//build the obstacle (default shape = sphere)
			OpenSteer::AbstractObstacle* obstacle;
			if(paramValues1Str[1] == std::string("box"))
			{
				obstacle = dynamic_cast<PlugIn*>(mPlugIn)->addObstacle("box");
				OpenSteer::BoxObstacle* box =
						dynamic_cast<OpenSteer::BoxObstacle*>(obstacle);
				box->width = modelDims.get_x();
				box->height = modelDims.get_z();
				box->depth = modelDims.get_y();
				box->setForward(LVecBase3fToOpenSteerVec3(forward).normalize());
				box->setSide(LVecBase3fToOpenSteerVec3(side).normalize());
				box->setUp(LVecBase3fToOpenSteerVec3(up).normalize());
			}
			///TODO
			else if (paramValues1Str[1] == std::string("plane"))
			{

			}
			else if (paramValues1Str[1] == std::string("rectangle"))
			{

			}
			else
			{
				//sphere default

			}
			//set position
			//set seenFromState

		}
	}
}

void SteerPlugIn::onAddToSceneSetup()
{
	//set mOwnerObject's parent node path as reference
	mReferenceNP = mOwnerObject->getNodePath().get_parent();
	//build pathway
	doBuildPathway();
	//add obstacles
	if (not mReferenceNP.is_empty())
	{
		doAddObstacles();
	}

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

	//clear all no more needed "Param" variables
	mPlugInTypeParam.clear();
	mPathwayParam.clear();
	mObstacleListParam.clear();
}

void SteerPlugIn::onRemoveFromSceneCleanup()
{
	//remove from AI manager update
	GameAIManager::GetSingletonPtr()->removeFromAIUpdate(this);
}

SteerPlugIn::Result SteerPlugIn::addSteerVehicle(SMARTPTR(SteerVehicle)steerVehicle)
{
	RETURN_ON_COND((not steerVehicle) or mReferenceNP.is_empty(), Result::ERROR)

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
	RETURN_ON_COND((not steerVehicle) or mReferenceNP.is_empty(), Result::ERROR)

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
