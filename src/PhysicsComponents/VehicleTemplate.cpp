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
 * \file /Ely/src/PhysicsComponents/VehicleTemplate.cpp
 *
 * \date 22/set/2013 (11:04:35)
 * \author consultit
 */

#include "PhysicsComponents/VehicleTemplate.h"
#include "PhysicsComponents/Vehicle.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

VehicleTemplate::VehicleTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"VehicleTemplate::VehicleTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"VehicleTemplate::VehicleTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"VehicleTemplate::VehicleTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

VehicleTemplate::~VehicleTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType VehicleTemplate::componentType() const
{
	return ComponentType("Vehicle");
}

ComponentFamilyType VehicleTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

SMARTPTR(Component)VehicleTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Vehicle) newVehicle = new Vehicle(this);
	newVehicle->setComponentId(compId);
	if (not newVehicle->initialize())
	{
		return NULL;
	}
	return newVehicle.p();
}

void VehicleTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("throw_events", "false"));
	mParameterTable.insert(ParameterNameValue("chassis_scale", "1.0"));
	mParameterTable.insert(ParameterNameValue("chassis_shape_type", "box"));
	mParameterTable.insert(ParameterNameValue("chassis_shape_size", "medium"));
	mParameterTable.insert(ParameterNameValue("chassis_mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("chassis_friction", "0.8"));
	mParameterTable.insert(ParameterNameValue("chassis_restitution", "0.1"));
	mParameterTable.insert(ParameterNameValue("chassis_collide_mask", "all_on"));
	mParameterTable.insert(ParameterNameValue("wheels_number", "4"));
}

//TypedObject semantics: hardcoded
TypeHandle VehicleTemplate::_type_handle;

} // namespace ely
