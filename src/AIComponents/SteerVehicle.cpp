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
#include "Game/GameAIManager.h"
#include "AIComponents/OpenSteerLocal/PlugIn_OneTurning.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

SteerVehicle::SteerVehicle()
{
	// TODO Auto-generated constructor stub

}

SteerVehicle::SteerVehicle(SMARTPTR(SteerVehicleTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
	"OpenSteerVehicle::OpenSteerVehicle: invalid GameAIManager")

	mTmpl = tmpl;
	reset();
}

SteerVehicle::~SteerVehicle()
{
	// TODO Auto-generated destructor stub
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
	//mov type
	std::string movType = mTmpl->parameter(std::string("mov_type"));
	if (movType == std::string("kinematic"))
	{
		CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
				"SteerVehicle::initialize: invalid GamePhysicsManager")
		mMovType = OPENSTEER_KINEMATIC
		;
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
		mVehicle = new OneTurning;
	}
	//get settings
	SimpleVehicleSettings settings;
	//radius
	mInputRadius = strtof(mTmpl->parameter(std::string("radius")).c_str(),
	NULL);
	//
	float value;
	//mass
	value = strtof(mTmpl->parameter(std::string("mass")).c_str(),
	NULL);
	settings.m_mass = (value > 0.0 ? value : 1.0);
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
	mVehicle->setSettings(settings);
	//
	return result;
}

void SteerVehicle::onAddToObjectSetup()
{

}

void SteerVehicle::onRemoveFromObjectCleanup()
{
	//
	delete mVehicle;
	reset();
}

void SteerVehicle::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

}

//TypedObject semantics: hardcoded
TypeHandle SteerVehicle::_type_handle;

} /* namespace ely */
