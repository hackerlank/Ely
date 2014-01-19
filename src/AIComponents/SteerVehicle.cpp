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
#include <throw_event.h>

namespace
{
const std::string STARTEVENT("OnStartSteerVehicle");
const std::string STOPEVENT("OnStopSteerVehicle");
}  // namespace

namespace ely
{

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
	//throw events setting
	mThrowEvents = (
			mTmpl->parameter(std::string("throw_events"))
					== std::string("true") ? true : false);
	//external update
	mExternalUpdate = (
			mTmpl->parameter(std::string("external_update"))
					== std::string("true") ? true : false);
	//type
	std::string type = mTmpl->parameter(std::string("type"));
	if (type == std::string("pedestrian"))
	{
		not mExternalUpdate ?
				mVehicle = new Pedestrian<SteerVehicle> :
				mVehicle = new ExternalPedestrian<SteerVehicle>;
	}
	else if (type == std::string(""))
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
	std::string movType = mTmpl->parameter(std::string("mov_type"));
	if (movType == std::string("kinematic"))
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
	std::string rayMask = mTmpl->parameter(std::string("ray_mask"));
	if (rayMask == std::string("all_on"))
	{
		mRayMask = BitMask32::all_on();
	}
	else if (rayMask == std::string("all_off"))
	{
		mRayMask = BitMask32::all_off();
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(rayMask.c_str(), NULL, 0);
		mRayMask.set_word(mask);
	}
	//set vehicle settings
	dynamic_cast<VehicleAddOn*>(mVehicle)->setSettings(settings);
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

		//throw OnStartSteerVehicle event (if enabled)
		if (mThrowEvents and (not mSteerVehicleStartSent))
		{
			throw_event(STARTEVENT, EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mSteerVehicleStartSent = true;
			mSteerVehicleStopSent = false;
		}
	}
	else
	{
		//mVehicle.speed == 0.0
		//throw OnStopSteerVehicle event (if enabled)
		if (mThrowEvents and (not mSteerVehicleStopSent))
		{
			throw_event(STOPEVENT, EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mSteerVehicleStopSent = true;
			mSteerVehicleStartSent = false;
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

//TypedObject semantics: hardcoded
TypeHandle SteerVehicle::_type_handle;

} /* namespace ely */
