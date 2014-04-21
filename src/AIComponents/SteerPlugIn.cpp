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
#include "AIComponents/OpenSteerLocal/PlugIn_Boids.h"
#include "AIComponents/OpenSteerLocal/PlugIn_MultiplePursuit.h"
#include "AIComponents/OpenSteerLocal/PlugIn_Soccer.h"
#include "AIComponents/OpenSteerLocal/PlugIn_CaptureTheFlag.h"
#include "AIComponents/OpenSteerLocal/PlugIn_LowSpeedTurn.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"
#include "SceneComponents/Model.h"
#include "SceneComponents/InstanceOf.h"

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
	//type
	mPlugInTypeParam = mTmpl->parameter(std::string("type"));
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
	else if (mPlugInTypeParam == std::string("boid"))
	{
		mPlugIn = new BoidsPlugIn<SteerVehicle>;
	}
	else if (mPlugInTypeParam == std::string("multiple_pursuit"))
	{
		mPlugIn = new MpPlugIn<SteerVehicle>;
	}
	else if (mPlugInTypeParam == std::string("soccer"))
	{
		mPlugIn = new MicTestPlugIn<SteerVehicle>;
	}
	else if (mPlugInTypeParam == std::string("capture_the_flag"))
	{
		mPlugIn = new CtfPlugIn<SteerVehicle>;
	}
	else if (mPlugInTypeParam == std::string("low_speed_turn"))
	{
		mPlugIn = new LowSpeedTurnPlugIn<SteerVehicle>;
	}
	else
	{
		///default: "one_turning"
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
		dynamic_cast<PlugIn*>(mPlugIn)->setPathway(numPoints, points, true,
				&radius, closedCycle);
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
		dynamic_cast<PlugIn*>(mPlugIn)->setPathway(numPoints, points, false,
				radii, closedCycle);
		delete[] radii;
	}
	delete[] points;
}

void SteerPlugIn::onAddToSceneSetup()
{
	//set mOwnerObject's parent node path as reference
	mReferenceNP = mOwnerObject->getNodePath().get_parent();

	//build pathway
	doBuildPathway();

	//set the plugin local obstacles reference
	dynamic_cast<PlugIn*>(mPlugIn)->localObstacles = &mLocalObstacles;
	//set the plugin global obstacles reference
	dynamic_cast<PlugIn*>(mPlugIn)->obstacles = &mObstacles;
	//add its own obstacles
	if (not mReferenceNP.is_empty())
	{
		doAddObstacles();
	}

	//Add to the AI manager update
	GameAIManager::GetSingletonPtr()->addToAIUpdate(this);

#ifdef ELY_DEBUG
	mDrawer3dNP = ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(
			ObjectId("render"))->getNodePath().attach_new_node(
			"Drawer3dNP_" + COMPONENT_STANDARD_NAME);
	mDrawer2dNP = ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(
			ObjectId("aspect2d"))->getNodePath().attach_new_node(
			"Drawer2dNP_" + COMPONENT_STANDARD_NAME);

	//get the camera object
	SMARTPTR(Object)cameraDebug = ObjectTemplateManager::GetSingletonPtr()->
	getCreatedObject(ObjectId("camera"));
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

#ifdef ELY_THREAD
	//lock (guard) the obstacles' mutex
	HOLD_MUTEX(mObstaclesMutex)

	//check condition var
	while (mUpdateCounter > 0)
	{
		//there are updates: wait
		mObstaclesVar.wait();
	}
#endif

	//remove all local obstacles
	OpenSteer::ObstacleGroup::iterator iterLocal;
	for (iterLocal = mLocalObstacles.begin();
			iterLocal != mLocalObstacles.end(); ++iterLocal)
	{
		//find in global obstacles and remove it
		OpenSteer::ObstacleGroup::iterator iter = std::find(mObstacles.begin(),
				mObstacles.end(), *iterLocal);
		if (iter != mObstacles.end())
		{
			//remove from global obstacles
			mObstacles.erase(iter);
		}
		//delete obstacle
		delete *iterLocal;
	}
	//clear local obstacles
	mLocalObstacles.clear();
}

typedef VehicleAddOnMixin<OpenSteer::SimpleVehicle, SteerVehicle> VehicleAddOn;

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

			//check if SteerVehicle object needs reparenting
			NodePath steerVehicleObjectNP = steerVehicle->getOwnerObject()->getNodePath();
			if(mReferenceNP != steerVehicleObjectNP.get_parent())
			{
				//reparenting is needed
				LPoint3f newPos = steerVehicleObjectNP.get_pos(mReferenceNP);
				LVector3f newForward = mReferenceNP.get_relative_vector(
				steerVehicleObjectNP, LVector3f::forward());
				LVector3f newUp = mReferenceNP.get_relative_vector(
				steerVehicleObjectNP, LVector3f::up());
				//the SteerVehicle owner object is reparented to the SteerPlugIn
				//object reference node path, updating pos/dir
				steerVehicleObjectNP.reparent_to(mReferenceNP);
				steerVehicleObjectNP.set_pos(newPos);
				steerVehicleObjectNP.heads_up(newPos + newForward, newUp);
				//SteerPlugIn object updates SteerVehicle's pos/dir
				//wrt its reference node path
				VehicleSettings settings =
				dynamic_cast<VehicleAddOn*>(steerVehicle->mVehicle)->getSettings();
				settings.m_forward = LVecBase3fToOpenSteerVec3(newForward).normalize();
				settings.m_up = LVecBase3fToOpenSteerVec3(newUp).normalize();
				settings.m_position = LVecBase3fToOpenSteerVec3(newPos);
				dynamic_cast<VehicleAddOn*>(steerVehicle->mVehicle)->setSettings(settings);
			}

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

void SteerPlugIn::setPathway(int numOfPoints, LPoint3f const points[],
		bool singleRadius, float const radii[], bool closedCycle)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//convert to OpenSteer points
	OpenSteer::Vec3* osPoints = new OpenSteer::Vec3[numOfPoints];
	for (int idx = 0; idx < numOfPoints; ++idx)
	{
		osPoints[idx] = LVecBase3fToOpenSteerVec3(points[idx]);
	}
	//set pathway actually
	dynamic_cast<PlugIn*>(mPlugIn)->setPathway(numOfPoints, osPoints, true,
			radii, closedCycle);
	//
	delete[] osPoints;
}


void SteerPlugIn::doAddObstacles()
{
	//
	std::vector<std::string> paramValues1Str, paramValues2Str;
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
			//any obstacle string must have the form:
			//"objectId@shape@seenFromState"
			paramValues2Str = parseCompoundString(*iter, '@');
			if (paramValues2Str.size() < 3)
			{
				continue;
			}
			//get obstacle object
			SMARTPTR(Object)obstacleObject =
			ObjectTemplateManager::GetSingleton().getCreatedObject(
					ObjectId(paramValues2Str[0]));
			if (not obstacleObject)
			{
				continue;
			}
			//add the obstacle
			addObstacle(obstacleObject, paramValues2Str[1], paramValues2Str[2]);
		}
	}
}

OpenSteer::AbstractObstacle* SteerPlugIn::addObstacle(SMARTPTR(Object) object,
		const std::string& type, const std::string& seenFromState,
		float width, float height,	float depth,
		float radius, const LVector3f& side, const LVector3f& up,
		const LVector3f& forward, const LPoint3f& position)
{
#ifdef ELY_THREAD
	//lock (guard) the obstacles' mutex
	HOLD_MUTEX(mObstaclesMutex)

	//check condition var
	while (mUpdateCounter > 0)
	{
		//there are some "updating": wait
		mObstaclesVar.wait();
	}
#endif

	LPoint3f newPos = position;
	LVector3f newSide = side, newUp = up, newForw = forward;
	if (object)
	{
		//get obstacle dimensions wrt the Model or InstanceOf component (if any)
		NodePath obstacleNP;
		SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("Scene"));
		if (not aiComp)
		{
			//no Scene component
			return NULL;
		}
		else if(aiComp->componentType() == ComponentType("Model"))
		{
			obstacleNP = NodePath(DCAST(Model, aiComp)->getNodePath().node());
		}
		else if (aiComp->componentType() == ComponentType("InstanceOf"))
		{
			obstacleNP = NodePath(DCAST(InstanceOf, aiComp)->getNodePath().node());
		}
		//get object dimensions
		LVecBase3f modelDims;
		LVector3f modelDeltaCenter;
		float modelRadius;
		GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
		obstacleNP, modelDims, modelDeltaCenter, modelRadius);
		//correct obstacle's parameters
		newPos = obstacleNP.get_pos(mReferenceNP) - modelDeltaCenter;
		newForw = mReferenceNP.get_relative_vector(obstacleNP, LVector3f::forward());
		newUp = mReferenceNP.get_relative_vector(obstacleNP, LVector3f::up());
		newSide = mReferenceNP.get_relative_vector(obstacleNP, LVector3f::right());
		width = modelDims.get_x();
		height = modelDims.get_z();
		depth = modelDims.get_y();
		radius = modelRadius;
	}
	//set seen from state
	OpenSteer::AbstractObstacle::seenFromState seenFS;
	if (seenFromState == "outside")
	{
		seenFS = OpenSteer::AbstractObstacle::outside;
	}
	else if(seenFromState == "inside")
	{
		seenFS = OpenSteer::AbstractObstacle::inside;
	}
	else
	{
		//default: both
		seenFS = OpenSteer::AbstractObstacle::both;
	}
	///create actually the obstacle
	OpenSteer::AbstractObstacle* obstacle = NULL;
	if (type == std::string("box"))
	{
		BoxObstacle* box = new BoxObstacle(width, height, depth);
		obstacle = box;
		box->setSide(LVecBase3fToOpenSteerVec3(newSide).normalize());
		box->setUp(LVecBase3fToOpenSteerVec3(newUp).normalize());
		box->setForward(LVecBase3fToOpenSteerVec3(newForw).normalize());
		box->setPosition(LVecBase3fToOpenSteerVec3(newPos));
		obstacle->setSeenFrom(seenFS);
	}
	if (type == std::string("plane"))
	{
		obstacle = new PlaneObstacle(LVecBase3fToOpenSteerVec3(newSide).normalize(),
		LVecBase3fToOpenSteerVec3(newUp).normalize(),
		LVecBase3fToOpenSteerVec3(newForw).normalize(),
		LVecBase3fToOpenSteerVec3(newPos));
		obstacle->setSeenFrom(seenFS);
	}
	if (type == std::string("rectangle"))
	{
		obstacle = new RectangleObstacle(width, height,
		LVecBase3fToOpenSteerVec3(newSide).normalize(),
		LVecBase3fToOpenSteerVec3(newUp).normalize(),
		LVecBase3fToOpenSteerVec3(newForw).normalize(),
		LVecBase3fToOpenSteerVec3(newPos), seenFS);
	}
	if (type == std::string("sphere"))
	{
		obstacle = new SphereObstacle(radius, LVecBase3fToOpenSteerVec3(newPos));
		obstacle->setSeenFrom(seenFS);
	}
	//store obstacle
	if (obstacle)
	{
		//add to local obstacles
		mLocalObstacles.push_back(obstacle);
		//add to global obstacles
		mObstacles.push_back(obstacle);
	}
	return obstacle;
}

void SteerPlugIn::removeObstacle(OpenSteer::AbstractObstacle* obstacle)
{
#ifdef ELY_THREAD
	//lock (guard) the obstacles' mutex
	HOLD_MUTEX(mObstaclesMutex)

	//check condition var
	while (mUpdateCounter > 0)
	{
		//there are updates: wait
		mObstaclesVar.wait();
	}
#endif

	//remove only if obstacle is local
	OpenSteer::ObstacleGroup::iterator iterLocal = std::find(
			mLocalObstacles.begin(), mLocalObstacles.end(), obstacle);
	if (iterLocal != mLocalObstacles.end())
	{
		//find in global obstacles and remove it
		OpenSteer::ObstacleGroup::iterator iter = std::find(
				mObstacles.begin(), mObstacles.end(), *iterLocal);
		if (iter != mObstacles.end())
		{
			//remove from global obstacles
			mObstacles.erase(iter);
		}
		//delete obstacle
		delete *iterLocal;
		//remove from local obstacles
		mLocalObstacles.erase(iterLocal);
	}
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

#ifdef ELY_THREAD
	{
		//lock (guard) the obstacles' mutex
		HOLD_MUTEX(mObstaclesMutex)

		//start this update and increment counter
		++mUpdateCounter;
	}
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

				// invoke PlugIn's Update method
				mPlugIn->update(mCurrentTime, dt);

#ifdef ELY_DEBUG
			}
		}
#endif

#ifdef ELY_THREAD
	{
		//lock (guard) the obstacles' mutex
		HOLD_MUTEX(mObstaclesMutex)

		//decrements update counter
		--mUpdateCounter;
		//notify some thread this update is complete
		mObstaclesVar.notify();
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
	RETURN_ON_COND(mDrawer3dNP.is_empty() or mDrawer2dNP.is_empty(),
			Result::ERROR)

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

//defines static members
OpenSteer::ObstacleGroup SteerPlugIn::mObstacles;
#ifdef ELY_THREAD
Mutex SteerPlugIn::mObstaclesMutex;
ConditionVar SteerPlugIn::mObstaclesVar(mObstaclesMutex);
unsigned int SteerPlugIn::mUpdateCounter = 0;
#endif

//TypedObject semantics: hardcoded
TypeHandle SteerPlugIn::_type_handle;

} /* namespace ely */
