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
 * \date 04/dic/2013 (09:20:42)
 * \author consultit
 */
#include "AIComponents/SteerVehicle.h"
#include "AIComponents/SteerVehicleTemplate.h"
#include "AIComponents/SteerPlugIn.h"
#include "AIComponents/OpenSteerLocal/PlugIn_OneTurning.h"
#include "AIComponents/OpenSteerLocal/PlugIn_Pedestrian.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"
#include "PhysicsComponents/RigidBody.h"

namespace ely
{

//VehicleAddOn typedef.
typedef VehicleAddOnMixin<OpenSteer::SimpleVehicle, SteerVehicle> VehicleAddOn;

SteerVehicle::SteerVehicle() :
		mHitResult(BulletClosestHitRayResult::empty())
{
}

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

ComponentFamilyType SteerVehicle::familyType() const
{
	return mTmpl->familyType();
}

ComponentType SteerVehicle::componentType() const
{
	return mTmpl->componentType();
}

bool SteerVehicle::initialize()
{
	bool result = true;
	//
	std::string param;
	unsigned int idx1, valueNum1;
	std::vector<std::string> paramValuesStr1, paramValuesStr2;
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
	else if (param == std::string(""))
	{
	}
	else
	{
		//default: OneTurning
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
	//get settings
	VehicleSettings settings;
	float value;
	//mass
	value = strtof(mTmpl->parameter(std::string("mass")).c_str(),
	NULL);
	settings.m_mass = (value > 0.0 ? value : 1.0);
	//radius
	mInputRadius = strtof(mTmpl->parameter(std::string("radius")).c_str(),
	NULL);
	//speed
	value = strtof(mTmpl->parameter(std::string("speed")).c_str(),
	NULL);
	settings.m_speed = (value > 0.0 ? value : -value);
	//max force
	value = strtof(mTmpl->parameter(std::string("max_force")).c_str(),
	NULL);
	settings.m_maxForce = (value > 0.0 ? value : -value);
	//max speed
	value = strtof(mTmpl->parameter(std::string("max_speed")).c_str(),
	NULL);
	settings.m_maxSpeed = (value > 0.0 ? value : 1.0);
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
	param = mTmpl->parameter(std::string("thrown_events"));
	if(param != std::string(""))
	{
		//events specified
		//event1[@event_name1[@delta_frame1]][:...[:eventN[@event_nameN[@delta_frameN]]]]
		paramValuesStr1 = parseCompoundString(param, ':');
		valueNum1 = paramValuesStr1.size();
		for (idx1 = 0; idx1 < valueNum1; ++idx1)
		{
			//eventX[@event_nameX[@delta_frameX]]
			paramValuesStr2 = parseCompoundString(paramValuesStr1[idx1], '@');
			if (paramValuesStr2.size() >= 1)
			{
				//get delta frame if any
				int deltaFrame = 1;
				if (paramValuesStr2.size() >= 3)
				{
					deltaFrame = strtof(paramValuesStr2[2].c_str(), NULL);
					if (deltaFrame < 1)
					{
						deltaFrame = 1;
					}
				}
				//
				std::string name;
				//get event
				if (paramValuesStr2[0] == "start")
				{
					//get name if any
					name = std::string(mOwnerObject->objectId())
							+ "_SteerVehicle_Start";
					if ((paramValuesStr2.size() >= 2)
							and (paramValuesStr2[1] != ""))
					{
						name = paramValuesStr2[1];
					}
					//set values
					mStart.mEnable = true;
					mStart.mEventName = name;
					mStart.mFrameCount = 0;
					mStart.mDeltaFrame = deltaFrame;
					//enable the event
					doEnableSteerVehicleEvent(STARTEVENT, mStart);
				}
				else if (paramValuesStr2[0] == "stop")
				{
					//get name if any
					name = std::string(mOwnerObject->objectId())
							+ "_SteerVehicle_Stop";
					if ((paramValuesStr2.size() >= 2)
							and (paramValuesStr2[1] != ""))
					{
						name = paramValuesStr2[1];
					}
					//set values
					mStop.mEnable = true;
					mStop.mEventName = name;
					mStop.mFrameCount = 0;
					mStop.mDeltaFrame = deltaFrame;
					//enable the event
					doEnableSteerVehicleEvent(STOPEVENT, mStop);
				}
				else if (paramValuesStr2[0] == "path_following")
				{
					//get name if any
					name = std::string(mOwnerObject->objectId())
							+ "_SteerVehicle_PathFollowing";
					if ((paramValuesStr2.size() >= 2)
							and (paramValuesStr2[1] != ""))
					{
						name = paramValuesStr2[1];
					}
					//set values
					mPathFollowing.mEnable = true;
					mPathFollowing.mEventName = name;
					mPathFollowing.mFrameCount = 0;
					mPathFollowing.mDeltaFrame = deltaFrame;
					//enable the event
					doEnableSteerVehicleEvent(PATHFOLLOWINGEVENT, mPathFollowing);
				}
				else if (paramValuesStr2[0] == "avoid_obstacle")
				{
					//get name if any
					name = std::string(mOwnerObject->objectId())
							+ "_SteerVehicle_AvoidObstacle";
					if ((paramValuesStr2.size() >= 2)
							and (paramValuesStr2[1] != ""))
					{
						name = paramValuesStr2[1];
					}
					//set values
					mAvoidObstacle.mEnable = true;
					mAvoidObstacle.mEventName = name;
					mAvoidObstacle.mFrameCount = 0;
					mAvoidObstacle.mDeltaFrame = deltaFrame;
					//enable the event
					doEnableSteerVehicleEvent(AVOIDOBSTACLEEVENT, mAvoidObstacle);
				}
				else if (paramValuesStr2[0] == "avoid_close_neighbor")
				{
					//get name if any
					name = std::string(mOwnerObject->objectId())
							+ "_SteerVehicle_AvoidCloseNeighbor";
					if ((paramValuesStr2.size() >= 2)
							and (paramValuesStr2[1] != ""))
					{
						name = paramValuesStr2[1];
					}
					//set values
					mAvoidCloseNeighbor.mEnable = true;
					mAvoidCloseNeighbor.mEventName = name;
					mAvoidCloseNeighbor.mFrameCount = 0;
					mAvoidCloseNeighbor.mDeltaFrame = deltaFrame;
					//enable the event
					doEnableSteerVehicleEvent(AVOIDCLOSENEIGHBOREVENT, mAvoidCloseNeighbor);
				}
				else if (paramValuesStr2[0] == "avoid_neighbor")
				{
					//get name if any
					name = std::string(mOwnerObject->objectId())
							+ "_SteerVehicle_AvoidNeighbor";
					if ((paramValuesStr2.size() >= 2)
							and (paramValuesStr2[1] != ""))
					{
						name = paramValuesStr2[1];
					}
					//set values
					mAvoidNeighbor.mEnable = true;
					mAvoidNeighbor.mEventName = name;
					mAvoidNeighbor.mFrameCount = 0;
					mAvoidNeighbor.mDeltaFrame = deltaFrame;
					//enable the event
					doEnableSteerVehicleEvent(AVOIDNEIGHBOREVENT, mAvoidNeighbor);
				}
				else
				{
					//paramValuesStr2[0] is not a suitable event
					break;
				}
			}
		}
	}

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
			&SteerVehicle::doUpdateSteerVehicle):
			dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityUpdateMethod(
			&SteerVehicle::doExternalUpdateSteerVehicle);

	//set the bullet physics
	mBulletWorld = GamePhysicsManager::GetSingletonPtr()->bulletWorld();
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
	SMARTPTR(Object)steerPlugInObject = ObjectTemplateManager::
	GetSingleton().getCreatedObject(mSteerPlugInObjectId);
	///Add to SteerPlugIn update
	if (steerPlugInObject)
	{
		SMARTPTR(SteerPlugIn)steerPlugIn = DCAST(SteerPlugIn,
				steerPlugInObject->getComponent(familyType()));
		//
		if (steerPlugIn)
		{
			steerPlugIn->addSteerVehicle(this);
		}
	}
}

void SteerVehicle::onRemoveFromSceneCleanup()
{
	//lock (guard) the SteerVehicle SteerPlugIn mutex
	HOLD_REMUTEX(mSteerPlugInMutex)

	///Remove from SteerPlugIn update (if previously added)
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
			//correct updatedPos.z if needed
			//ray down
			mHitResult = mBulletWorld->ray_test_closest(
					updatedPos + mDeltaRayOrig, updatedPos + mDeltaRayDown,
					mRayMask);
			if (mHitResult.has_hit())
			{
				//physic mesh is below
				updatedPos.set_z(mHitResult.get_hit_pos().get_z());
				//correct vehicle position
				mVehicle->setPosition(LVecBase3fToOpenSteerVec3(updatedPos));
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
		ownerObjectNP.heads_up(
				updatedPos - OpenSteerVec3ToLVecBase3f(mVehicle->forward()),
				OpenSteerVec3ToLVecBase3f(mVehicle->up()));

		//throw Start event (if enabled)
		int frameCount = ClockObject::get_global_clock()->get_frame_count();
		if (mStart.mEnable
				and (frameCount > mStart.mFrameCount + mStart.mDeltaFrame))
		{
			//enough frames are passed: throw the event
			throw_event(mStart.mEventName, EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			//update frame count
			mStart.mFrameCount = frameCount;
		}
	}
	else
	{
		//mVehicle.speed == 0.0
		//throw Stop event (if enabled)
		int frameCount = ClockObject::get_global_clock()->get_frame_count();
		if (mStop.mEnable
				and (frameCount > mStop.mFrameCount + mStop.mDeltaFrame))
		{
			//enough frames are passed: throw the event
			throw_event(mStop.mEventName, EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			//update frame count
			mStop.mFrameCount = frameCount;
		}
	}
}

void SteerVehicle::doExternalUpdateSteerVehicle(const float currentTime,
		const float elapsedTime)
{
	//set vehicle's position, forward,( side,) up, speed
	mVehicle->setPosition(
			LVecBase3fToOpenSteerVec3(
					mOwnerObject->getNodePath().get_pos()
							- LVector3f(0.0, 0.0, mCorrectHeightRigidBody)));
	mVehicle->setForward(
			LVecBase3fToOpenSteerVec3(
					mOwnerObject->getNodePath().get_parent().get_relative_vector(
							mOwnerObject->getNodePath(), -LVector3f::forward())).normalize());
	mVehicle->setUp(
			LVecBase3fToOpenSteerVec3(
					mOwnerObject->getNodePath().get_parent().get_relative_vector(
							mOwnerObject->getNodePath(), LVector3f::up())).normalize());
	//
	//no event thrown: external updating sub-system will do, if expected
}

void SteerVehicle::doEnableSteerVehicleEvent(SteerVehicleEvent event, ThrowEventData eventData)
{
	//some checks
	RETURN_ON_COND(eventData.mEventName == std::string(""),)
	if (eventData.mDeltaFrame < 1)
	{
		eventData.mDeltaFrame = 1;
	}

	switch (event)
	{
	case STARTEVENT:
		if(mStart.mEnable != eventData.mEnable)
		{
			mStart = eventData;
			mStart.mFrameCount = 0;
		}
		break;
	case STOPEVENT:
		if(mStop.mEnable != eventData.mEnable)
		{
			mStop = eventData;
			mStop.mFrameCount = 0;
		}
		break;
	case PATHFOLLOWINGEVENT:
		if(mPathFollowing.mEnable != eventData.mEnable)
		{
			mPathFollowing = eventData;
			mPathFollowing.mFrameCount = 0;
			mPathFollowing.mEnable ?
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityPathFollowingMethod(
							&SteerVehicle::doThrowPathFollowing) :
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityPathFollowingMethod(
					NULL);
		}
		break;
	case AVOIDOBSTACLEEVENT:
		if(mAvoidObstacle.mEnable != eventData.mEnable)
		{
			mAvoidObstacle = eventData;
			mAvoidObstacle.mFrameCount = 0;
			mAvoidObstacle.mEnable ?
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidObstacleMethod(
							&SteerVehicle::doThrowAvoidObstacle) :
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidObstacleMethod(
					NULL);
		}
		break;
	case AVOIDCLOSENEIGHBOREVENT:
		if(mAvoidCloseNeighbor.mEnable != eventData.mEnable)
		{
			mAvoidCloseNeighbor = eventData;
			mAvoidCloseNeighbor.mFrameCount = 0;
			mAvoidCloseNeighbor.mEnable ?
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidCloseNeighborMethod(
							&SteerVehicle::doThrowAvoidCloseNeighbor) :
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidCloseNeighborMethod(
					NULL);
		}
		break;
	case AVOIDNEIGHBOREVENT:
		if(mAvoidNeighbor.mEnable != eventData.mEnable)
		{
			mAvoidNeighbor = eventData;
			mAvoidNeighbor.mFrameCount = 0;
			mAvoidNeighbor.mEnable ?
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidNeighborMethod(
							&SteerVehicle::doThrowAvoidNeighbor) :
					dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityAvoidNeighborMethod(
					NULL);
		}
		break;
	default:
		break;
	}
}

//TypedObject semantics: hardcoded
TypeHandle SteerVehicle::_type_handle;

} /* namespace ely */
