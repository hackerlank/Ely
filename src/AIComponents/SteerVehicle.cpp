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
	//radius (default: automatically computed)
	std::string radius = mTmpl->parameter(std::string("radius"));
	if (not radius.empty())
	{
		settings.m_radius = strtof(radius.c_str(), NULL);
	}
	//
	return result;
}

void SteerVehicle::onAddToObjectSetup()
{
}

void SteerVehicle::onRemoveFromObjectCleanup()
{
	//
	reset();
}

void SteerVehicle::onAddToSceneSetup()
{
}

void SteerVehicle::onRemoveFromSceneCleanup()
{
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
