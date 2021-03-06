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
 * \file /Ely/src/AIComponents/SteerVehicle.cpp
 *
 * \date 2013-12-04 
 * \author consultit
 */
#include "AIComponents/SteerVehicle.h"
#include "AIComponents/SteerPlugIn.h"
#include "Support/OpenSteerLocal/PlugIn_OneTurning.h"
#include "Support/OpenSteerLocal/PlugIn_Pedestrian.h"
#include "Support/OpenSteerLocal/PlugIn_Boids.h"
#include "Support/OpenSteerLocal/PlugIn_MultiplePursuit.h"
#include "Support/OpenSteerLocal/PlugIn_Soccer.h"
#include "Support/OpenSteerLocal/PlugIn_CaptureTheFlag.h"
#include "Support/OpenSteerLocal/PlugIn_LowSpeedTurn.h"
#include "Support/OpenSteerLocal/PlugIn_MapDrive.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

//VehicleAddOn typedef.
typedef VehicleAddOnMixin<SimpleVehicle, SteerVehicle> VehicleAddOn;

SteerVehicle::SteerVehicle(SMARTPTR(SteerVehicleTemplate)tmpl):
		mHitResult(BulletClosestHitRayResult::empty())
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
	"OpenSteerVehicle::OpenSteerVehicle: invalid GameAIManager")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"CrowdAgent::CrowdAgent: invalid GamePhysicsManager")

	mTmpl = tmpl;
	mSteerPlugIn.clear();
	reset();
}

SteerVehicle::~SteerVehicle()
{
}

bool SteerVehicle::initialize()
{
	bool result = true;
	//
	std::string param;
	float value;
	//external update
	mExternalUpdate = (
			mTmpl->parameter(std::string("external_update"))
					== std::string("true") ? true : false);
	//type
	param = mTmpl->parameter(std::string("type"));
	if (param == std::string("pedestrian"))
	{
		not mExternalUpdate ?
				mVehicle = new Pedestrian<SteerVehicle> :
				mVehicle = new ExternalPedestrian<SteerVehicle>;
	}
	else if (param == std::string("boid"))
	{
		not mExternalUpdate ?
				mVehicle = new Boid<SteerVehicle> :
				mVehicle = new ExternalBoid<SteerVehicle>;
	}
	else if (param == std::string("mp_wanderer"))
	{
		not mExternalUpdate ?
				mVehicle = new MpWanderer<SteerVehicle> :
				mVehicle = new ExternalMpWanderer<SteerVehicle>;
	}
	else if (param == std::string("mp_pursuer"))
	{
		not mExternalUpdate ?
				mVehicle = new MpPursuer<SteerVehicle> :
				mVehicle = new ExternalMpPursuer<SteerVehicle>;
	}
	else if (param == std::string("player"))
	{
		not mExternalUpdate ?
				mVehicle = new Player<SteerVehicle> :
				mVehicle = new ExternalPlayer<SteerVehicle>;
	}
	else if (param == std::string("ball"))
	{
		not mExternalUpdate ?
				mVehicle = new Ball<SteerVehicle> :
				mVehicle = new ExternalBall<SteerVehicle>;
	}
	else if (param == std::string("ctf_seeker"))
	{
		not mExternalUpdate ?
				mVehicle = new CtfSeeker<SteerVehicle> :
				mVehicle = new ExternalCtfSeeker<SteerVehicle>;
	}
	else if (param == std::string("ctf_enemy"))
	{
		not mExternalUpdate ?
				mVehicle = new CtfEnemy<SteerVehicle> :
				mVehicle = new ExternalCtfEnemy<SteerVehicle>;
	}
	else if (param == std::string("low_speed_turn"))
	{
		not mExternalUpdate ?
				mVehicle = new LowSpeedTurn<SteerVehicle> :
				mVehicle = new ExternalLowSpeedTurn<SteerVehicle>;
	}
	else if (param == std::string("map_driver"))
	{
		not mExternalUpdate ?
				mVehicle = new MapDriver<SteerVehicle> :
				mVehicle = new ExternalMapDriver<SteerVehicle>;
	}
	else
	{
		//default: one_turning
		not mExternalUpdate ?
				mVehicle = new OneTurning<SteerVehicle> :
				mVehicle = new ExternalOneTurning<SteerVehicle>;
	}
	//register to SteerPlugIn objectId
	mSteerPlugInObjectId = ObjectId(
			mTmpl->parameter(std::string("add_to_plugin")));
	//mov type
	param = mTmpl->parameter(std::string("mov_type"));
	if (param == std::string("kinematic"))
	{
		CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
				"SteerVehicle::initialize: invalid GamePhysicsManager")
		mMovType = OPENSTEER_KINEMATIC;
	}
	else
	{
		mMovType = OPENSTEER;
	}
	//up axis fixed
	mUpAxisFixed = (
			mTmpl->parameter(std::string("up_axis_fixed"))
					== std::string("true") ? true : false);
	//get settings
	VehicleSettings settings;
	//mass
	value = strtof(mTmpl->parameter(std::string("mass")).c_str(),
	NULL);
	settings.m_mass = (value >= 0.0 ? value : 1.0);
	//radius
	mInputRadius = strtof(mTmpl->parameter(std::string("radius")).c_str(),
	NULL);
	//speed
	value = strtof(mTmpl->parameter(std::string("speed")).c_str(),
	NULL);
	settings.m_speed = (value >= 0.0 ? value : -value);
	//max force
	value = strtof(mTmpl->parameter(std::string("max_force")).c_str(),
	NULL);
	settings.m_maxForce = (value >= 0.0 ? value : -value);
	//max speed
	value = strtof(mTmpl->parameter(std::string("max_speed")).c_str(),
	NULL);
	settings.m_maxSpeed = (value >= 0.0 ? value : 1.0);
	//ray mask
	param = mTmpl->parameter(std::string("ray_mask"));
	if (param == std::string("all_on"))
	{
		mRayMask = BitMask32::all_on();
	}
	else if (param == std::string("all_off"))
	{
		mRayMask = BitMask32::all_off();
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(param.c_str(), NULL, 0);
		mRayMask.set_word(mask);
	}
	//set vehicle settings
	dynamic_cast<VehicleAddOn*>(mVehicle)->setSettings(settings);
	//thrown events
	mThrownEventsParam = mTmpl->parameter(std::string("thrown_events"));
	//
	return result;
}

void SteerVehicle::onAddToObjectSetup()
{
	LVecBase3f modelDims;
	LVector3f modelDeltaCenter;
	float modelRadius;
	GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
			mOwnerObject->getNodePath(), modelDims, modelDeltaCenter,
			modelRadius);
	//set definitive radius
	if (mInputRadius <= 0.0)
	{
		// store new radius into settings
		VehicleSettings settings =
				dynamic_cast<VehicleAddOn*>(mVehicle)->getSettings();
		settings.m_radius = modelRadius;
		dynamic_cast<VehicleAddOn*>(mVehicle)->setSettings(settings);
	}
	//set physics parameters
	mMaxError = modelDims.get_z();
	mDeltaRayOrig = LVector3f(0, 0, mMaxError);
	mDeltaRayDown = LVector3f(0, 0, -10 * mMaxError);
	//correct height if there is a Physics or PhysicsControl component
	//for raycast into update
	if (mOwnerObject->getComponent(ComponentFamilyType("Physics"))
			or mOwnerObject->getComponent(
					ComponentFamilyType("PhysicsControl")))
	{
		mCorrectHeightRigidBody = modelDims.get_z() / 2.0;
	}
	else
	{
		mCorrectHeightRigidBody = 0.0;
	}
	//set entity and its related update method
	dynamic_cast<VehicleAddOn*>(mVehicle)->setEntity(this);
	//
	not mExternalUpdate ?
			dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityUpdateMethod(
					&SteerVehicle::doUpdateSteerVehicle) :
			dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityUpdateMethod(
					&SteerVehicle::doExternalUpdateSteerVehicle);

	//set the bullet physics
	mBulletWorld = GamePhysicsManager::GetSingletonPtr()->bulletWorld();

	//set thrown events if any
	unsigned int idx1, valueNum1;
	std::vector<std::string> paramValuesStr1, paramValuesStr2;
	if (mThrownEventsParam != std::string(""))
	{
		//events specified
		//event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]
		paramValuesStr1 = parseCompoundString(mThrownEventsParam, ':');
		valueNum1 = paramValuesStr1.size();
		for (idx1 = 0; idx1 < valueNum1; ++idx1)
		{
			//eventX@[event_nameX]@[frequencyX]
			paramValuesStr2 = parseCompoundString(paramValuesStr1[idx1], '@');
			if (paramValuesStr2.size() >= 3)
			{
				EventThrown event;
				ThrowEventData eventData;
				//get default name prefix
				std::string objectType = std::string(
						mOwnerObject->objectTmpl()->objectType());
				//get name
				std::string name = paramValuesStr2[1];
				//get frequency
				float frequency = strtof(paramValuesStr2[2].c_str(), NULL);
				if (frequency <= 0.0)
				{
					frequency = 30.0;
				}
				//get event
				if (paramValuesStr2[0] == "move")
				{
					event = MOVEEVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_SteerVehicle_Move";
					}
				}
				else if (paramValuesStr2[0] == "steady")
				{
					event = STEADYEVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_SteerVehicle_Steady";
					}
				}
				else if (paramValuesStr2[0] == "path_following")
				{
					event = PATHFOLLOWINGEVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_SteerVehicle_PathFollowing";
					}
				}
				else if (paramValuesStr2[0] == "avoid_obstacle")
				{
					event = AVOIDOBSTACLEEVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_SteerVehicle_AvoidObstacle";
					}
				}
				else if (paramValuesStr2[0] == "avoid_close_neighbor")
				{
					event = AVOIDCLOSENEIGHBOREVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_SteerVehicle_AvoidCloseNeighbor";
					}
				}
				else if (paramValuesStr2[0] == "avoid_neighbor")
				{
					event = AVOIDNEIGHBOREVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_SteerVehicle_AvoidNeighbor";
					}
				}
				else
				{
					//paramValuesStr2[0] is not a suitable event:
					//continue with the next event
					continue;
				}
				//set event data
				eventData.mEnable = true;
				eventData.mEventName = name;
				eventData.mTimeElapsed = 0;
				eventData.mFrequency = frequency;
				//enable the event
				doEnableSteerVehicleEvent(event, eventData);
			}
		}
	}

	//set the callbacks
	//Path Following
	dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityPathFollowingMethod(
			&SteerVehicle::doPathFollowing);
	//Avoid Obstacle
	dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidObstacleMethod(
			&SteerVehicle::doAvoidObstacle);
	//Avoid Close Neighbor
	dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidCloseNeighborMethod(
			&SteerVehicle::doAvoidCloseNeighbor);
	//Avoid Neighbor
	dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidNeighborMethod(
			&SteerVehicle::doAvoidNeighbor);
	//clear all no more needed "Param" variables
	mThrownEventsParam.clear();
}

void SteerVehicle::onRemoveFromObjectCleanup()
{
	//
	delete mVehicle;
	reset();
}

void SteerVehicle::onAddToSceneSetup()
{
	//set vehicle's forward,( side,) up and position
	NodePath ownerNP = mOwnerObject->getNodePath();
	VehicleSettings settings =
			dynamic_cast<VehicleAddOn*>(mVehicle)->getSettings();
	settings.m_forward =
			LVecBase3fToOpenSteerVec3(
					ownerNP.get_parent().get_relative_vector(
							mOwnerObject->getNodePath(), -LVector3f::forward())).normalize();
	settings.m_up = LVecBase3fToOpenSteerVec3(
			ownerNP.get_parent().get_relative_vector(
					mOwnerObject->getNodePath(), LVector3f::up())).normalize();
	settings.m_position = LVecBase3fToOpenSteerVec3(
			mOwnerObject->getNodePath().get_pos());
	dynamic_cast<VehicleAddOn*>(mVehicle)->setSettings(settings);

	//set SteerPlugIn object (if any)
	SMARTPTR(Object)steerPlugInObject =
			ObjectTemplateManager::GetSingleton().getCreatedObject(mSteerPlugInObjectId);
	//Add to SteerPlugIn update
	if (steerPlugInObject)
	{
		SMARTPTR(Component)aiComp = steerPlugInObject->getComponent(componentFamilyType());
		//
		if (aiComp and (aiComp->componentType() == ComponentType("SteerPlugIn")))
		{
			DCAST(SteerPlugIn, aiComp)->addSteerVehicle(this);
		}
	}
}

void SteerVehicle::onRemoveFromSceneCleanup()
{
	//lock (guard) the SteerVehicle SteerPlugIn mutex
	HOLD_REMUTEX(mSteerPlugInMutex)

	//Remove from SteerPlugIn update (if previously added)
	//mSteerPlugIn will be cleared during removing, so
	//remove through a temporary pointer
	SMARTPTR(SteerPlugIn) steerPlugIn = mSteerPlugIn;
	if (steerPlugIn)
	{
#ifdef ELY_THREAD
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//removeSteerVehicle will return if mDestroying, so
		//disable it and re-enable afterwards
		mDestroying = false;
#endif
		steerPlugIn->removeSteerVehicle(this);
#ifdef ELY_THREAD
		mDestroying = true;
#endif
	}
}

void SteerVehicle::setSettings(const VehicleSettings& settings)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	//set vehicle settings
	dynamic_cast<VehicleAddOn*>(mVehicle)->setSettings(settings);
}

VehicleSettings SteerVehicle::getSettings()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, VehicleSettings())

	//get vehicle settings
	return dynamic_cast<VehicleAddOn*>(mVehicle)->getSettings();
}

SMARTPTR(SteerPlugIn) SteerVehicle::getSteerPlugIn() const
{
	//lock (guard) the SteerVehicle SteerPlugIn mutex
	HOLD_REMUTEX(mSteerPlugInMutex)

	return mSteerPlugIn;
}

void SteerVehicle::doUpdateSteerVehicle(const float currentTime,
		const float elapsedTime)
{
	if (mVehicle->speed() > 0.0)
	{
		NodePath ownerObjectNP = mOwnerObject->getNodePath();
		LPoint3f updatedPos = OpenSteerVec3ToLVecBase3f(mVehicle->position());
		switch (mMovType)
		{
		case OPENSTEER:
			break;
		case OPENSTEER_KINEMATIC:
		{
			//correct updatedPos.z if needed
			HOLD_REMUTEX(GamePhysicsManager::GetSingletonPtr()->getMutex())
			{
				//ray down
				mHitResult = mBulletWorld->ray_test_closest(
						updatedPos + mDeltaRayOrig, updatedPos + mDeltaRayDown,
						mRayMask);
			}
			if (mHitResult.has_hit())
			{
				//updatedPos.z needs correction
				updatedPos.set_z(mHitResult.get_hit_pos().get_z());
				//correct vehicle position
				mVehicle->setPosition(LVecBase3fToOpenSteerVec3(updatedPos));
			}
		}
			break;
		default:
			break;
		}
		//correct z if there is a kinematic rigid body
		updatedPos.set_z(updatedPos.get_z() + mCorrectHeightRigidBody);
		//update node path pos
		ownerObjectNP.set_pos(updatedPos);
		//update node path dir
		mUpAxisFixed ?
			//up axis fixed: z
			ownerObjectNP.heads_up(
					updatedPos - OpenSteerVec3ToLVecBase3f(mVehicle->forward()),
					LVector3f::up()):
			//up axis free: from mVehicle
			ownerObjectNP.heads_up(
						updatedPos - OpenSteerVec3ToLVecBase3f(mVehicle->forward()),
						OpenSteerVec3ToLVecBase3f(mVehicle->up()));

		//handle Move/Steady events
		//throw Move event (if enabled)
		if (mMove.mEnable)
		{
			doThrowEvent(mMove);
		}
		//reset Steady event (if enabled and if thrown)
		if (mSteady.mEnable and mSteady.mThrown)
		{
			mSteady.mThrown = false;
			mSteady.mTimeElapsed = 0.0;
		}
	}
	else
	{
		//handle Move/Steady events
		//mVehicle.speed == 0.0
		//reset Move event (if enabled and if thrown)
		if (mMove.mEnable and mMove.mThrown)
		{
			mMove.mThrown = false;
			mMove.mTimeElapsed = 0.0;
		}
		//throw Steady event (if enabled)
		if (mSteady.mEnable)
		{
			doThrowEvent(mSteady);
		}
	}

	//handle SteerLibrary events
	doHandleSteerLibraryEvent(mPathFollowing, mPFCallbackCalled);
	doHandleSteerLibraryEvent(mAvoidObstacle, mAOCallbackCalled);
	doHandleSteerLibraryEvent(mAvoidCloseNeighbor, mACNCallbackCalled);
	doHandleSteerLibraryEvent(mAvoidNeighbor, mANCallbackCalled);
}

void SteerVehicle::doExternalUpdateSteerVehicle(const float currentTime,
		const float elapsedTime)
{
	OpenSteer::Vec3 oldPos = mVehicle->position();
	//update vehicle's
	//position,
	mVehicle->setPosition(
			LVecBase3fToOpenSteerVec3(
					mOwnerObject->getNodePath().get_pos()
							- LVector3f(0.0, 0.0, mCorrectHeightRigidBody)));
	//forward,
	mVehicle->setForward(
			LVecBase3fToOpenSteerVec3(
					mOwnerObject->getNodePath().get_parent().get_relative_vector(
							mOwnerObject->getNodePath(), -LVector3f::forward())).normalize());
	//up,
	mVehicle->setUp(
			LVecBase3fToOpenSteerVec3(
					mOwnerObject->getNodePath().get_parent().get_relative_vector(
							mOwnerObject->getNodePath(), LVector3f::up())).normalize());
	//side,
	mVehicle->setUnitSideFromForwardAndUp();
	//speed (elapsedTime should be != 0)
	mVehicle->setSpeed((mVehicle->position() - oldPos).length() / elapsedTime);
	//
	//no event thrown: external updating sub-system will do, if expected
}

void SteerVehicle::doEnableSteerVehicleEvent(EventThrown event, ThrowEventData eventData)
{
	//some checks
	RETURN_ON_COND(eventData.mEventName == std::string(""),)
	if (eventData.mFrequency <= 0.0)
	{
		eventData.mFrequency = 30.0;
	}

	switch (event)
	{
	case MOVEEVENT:
		if(mMove.mEnable != eventData.mEnable)
		{
			mMove = eventData;
			mMove.mTimeElapsed = 0;
		}
		break;
	case STEADYEVENT:
		if(mSteady.mEnable != eventData.mEnable)
		{
			mSteady = eventData;
			mSteady.mTimeElapsed = 0;
		}
		break;
	case PATHFOLLOWINGEVENT:
		if(mPathFollowing.mEnable != eventData.mEnable)
		{
			mPathFollowing = eventData;
			mPathFollowing.mTimeElapsed = 0;
			mPFCallbackCalled = false;
		}
		break;
	case AVOIDOBSTACLEEVENT:
		if(mAvoidObstacle.mEnable != eventData.mEnable)
		{
			mAvoidObstacle = eventData;
			mAvoidObstacle.mTimeElapsed = 0;
			mAOCallbackCalled = false;
		}
		break;
	case AVOIDCLOSENEIGHBOREVENT:
		if(mAvoidCloseNeighbor.mEnable != eventData.mEnable)
		{
			mAvoidCloseNeighbor = eventData;
			mAvoidCloseNeighbor.mTimeElapsed = 0;
			mACNCallbackCalled = false;
		}
		break;
	case AVOIDNEIGHBOREVENT:
		if(mAvoidNeighbor.mEnable != eventData.mEnable)
		{
			mAvoidNeighbor = eventData;
			mAvoidNeighbor.mTimeElapsed = 0;
			mANCallbackCalled = false;
		}
		break;
	default:
		break;
	}
}

//TypedObject semantics: hardcoded
TypeHandle SteerVehicle::_type_handle;

///Template

SteerVehicleTemplate::SteerVehicleTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"OpenSteerVehicleTemplate::OpenSteerVehicleTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"OpenSteerVehicleTemplate::OpenSteerVehicleTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"OpenSteerVehicleTemplate::OpenSteerVehicleTemplate: invalid GameAIManager")
	//
	setParametersDefaults();
}

SteerVehicleTemplate::~SteerVehicleTemplate()
{
	
}

ComponentType SteerVehicleTemplate::componentType() const
{
	return ComponentType(SteerVehicle::get_class_type().get_name());
}

ComponentFamilyType SteerVehicleTemplate::componentFamilyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)SteerVehicleTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(SteerVehicle) newOpenSteerVehicle = new SteerVehicle(this);
	newOpenSteerVehicle->setComponentId(compId);
	if (not newOpenSteerVehicle->initialize())
	{
		return NULL;
	}
	return newOpenSteerVehicle.p();
}

void SteerVehicleTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("type", "one_turning"));
	mParameterTable.insert(ParameterNameValue("external_update", "false"));
	mParameterTable.insert(ParameterNameValue("mov_type", "opensteer"));
	mParameterTable.insert(ParameterNameValue("up_axis_fixed", "false"));
	mParameterTable.insert(ParameterNameValue("mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("speed", "0.0"));
	mParameterTable.insert(ParameterNameValue("max_force", "0.1"));
	mParameterTable.insert(ParameterNameValue("max_speed", "1.0"));
	mParameterTable.insert(ParameterNameValue("ray_mask", "all_on"));
}

//TypedObject semantics: hardcoded
TypeHandle SteerVehicleTemplate::_type_handle;

} /* namespace ely */
