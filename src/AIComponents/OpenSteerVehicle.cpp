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
 * \file /Ely/src/AIComponents/OpenSteerVehicle.cpp
 *
 * \date 04/dic/2013 (09:20:42)
 * \author consultit
 */
#include "AIComponents/OpenSteerVehicle.h"
#include "AIComponents/OpenSteerVehicleTemplate.h"

namespace ely
{

OpenSteerVehicle::OpenSteerVehicle()
{
	// TODO Auto-generated constructor stub

}

OpenSteerVehicle::OpenSteerVehicle(SMARTPTR(OpenSteerVehicleTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
	"OpenSteerVehicle::OpenSteerVehicle: invalid GameAIManager")

	mTmpl = tmpl;
	reset();
}

OpenSteerVehicle::~OpenSteerVehicle()
{
	// TODO Auto-generated destructor stub
}

ComponentFamilyType OpenSteerVehicle::familyType() const
{
	return mTmpl->familyType();
}

ComponentType OpenSteerVehicle::componentType() const
{
	return mTmpl->componentType();
}

bool OpenSteerVehicle::initialize()
{
	bool result = true;
	//
	return result;
}

void OpenSteerVehicle::onAddToObjectSetup()
{
}

void OpenSteerVehicle::onRemoveFromObjectCleanup()
{
	//
	reset();
}

void OpenSteerVehicle::onAddToSceneSetup()
{
}

void OpenSteerVehicle::onRemoveFromSceneCleanup()
{
}

void OpenSteerVehicle::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

}

//TypedObject semantics: hardcoded
TypeHandle OpenSteerVehicle::_type_handle;

} /* namespace ely */
