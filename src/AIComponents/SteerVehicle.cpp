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
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"
#include <throw_event.h>

namespace
{
const std::string STARTEVENT("OnStartSteerVehicle");
const std::string STOPEVENT("OnStopSteerVehicle");
}  // namespace

namespace ely
{

typedef VehicleAddOnMixin<OpenSteer::SimpleVehicle, SteerVehicle> VehicleAddOn;

SteerVehicle::SteerVehicle()
{
}

SteerVehicle::SteerVehicle(SMARTPTR(SteerVehicleTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
	"OpenSteerVehicle::OpenSteerVehicle: invalid GameAIManager")

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
	//type
	std::string type = mTmpl->parameter(std::string("type"));
	if (type == std::string(""))
	{
	}
	else if (type == std::string(""))
	{
	}
	else
	{
		//default: OneTurning
		mVehicle = new OneTurning<SteerVehicle>;
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
	//set vehicle settings
	dynamic_cast<VehicleAddOn*>(mVehicle)->setSettings(settings);
	//
	return result;
}

void SteerVehicle::onAddToObjectSetup()
{
	//set definitive radius
	if (mInputRadius <= 0.0)
	{
		//auto compute radius
		LPoint3f minP, maxP;
		mOwnerObject->getNodePath().calc_tight_bounds(minP, maxP);
		float radius = (maxP - minP).length() / 2.0;
		// store new radius into settings
		VehicleSettings settings =
				dynamic_cast<VehicleAddOn*>(mVehicle)->getSettings();
		settings.m_radius = radius;
		dynamic_cast<VehicleAddOn*>(mVehicle)->setSettings(settings);
		// update radius into vehicle
		mVehicle->setRadius(radius);
	}
	//set entity and its related update method
	dynamic_cast<VehicleAddOn*>(mVehicle)->setEntity(this);
	dynamic_cast<VehicleAddOn*>(mVehicle)->setEntityUpdateMethod(
			&SteerVehicle::doUpdateSteerVehicle);
}

void SteerVehicle::onRemoveFromObjectCleanup()
{
	//
	delete mVehicle;
	mSteerPlugIn.clear();
	reset();
}

void SteerVehicle::onAddToSceneSetup()
{
	//set SteerPlugIn object (if any)
	SMARTPTR(Object)steerPlugInObject = ObjectTemplateManager::
	GetSingleton().getCreatedObject(mSteerPlugInObjectId);
	///Add to SteerPlugIn update
	if (steerPlugInObject)
	{
		mSteerPlugIn = DCAST(SteerPlugIn,
				steerPlugInObject->getComponent(familyType()));
		//
		if (mSteerPlugIn)
		{
			mSteerPlugIn->addSteerVehicle(this);
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
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//removeSteerVehicle will return if mDestroying, so
		//disable it and re-enable afterwards
		mDestroying = false;
		steerPlugIn->removeSteerVehicle(this);
		mDestroying = true;
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
		//update node path pos
		LPoint3f pos = OpenSteerVec3ToLVecBase3f(mVehicle->position());
		mOwnerObject->getNodePath().set_pos(pos);
		//update node path dir
		mOwnerObject->getNodePath().heads_up(
				pos - OpenSteerVec3ToLVecBase3f(mVehicle->forward()),
				OpenSteerVec3ToLVecBase3f(mVehicle->up()));

		//throw SteerVehicleStart event (if enabled)
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
		//throw SteerVehicleStop event (if enabled)
		if (mThrowEvents and (not mSteerVehicleStopSent))
		{
			throw_event(STOPEVENT, EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mSteerVehicleStopSent = true;
			mSteerVehicleStartSent = false;
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle SteerVehicle::_type_handle;

} /* namespace ely */
