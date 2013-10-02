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
 * \file /Ely/src/PhysicsControlComponents/VehicleTemplate.cpp
 *
 * \date 22/set/2013 (11:04:35)
 * \author consultit
 */

#include "PhysicsControlComponents/VehicleTemplate.h"
#include "PhysicsControlComponents/Vehicle.h"
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
	return ComponentFamilyType("PhysicsControl");
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
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("throw_events", "false"));
	mParameterTable.insert(ParameterNameValue("up_axis", "z"));
	mParameterTable.insert(ParameterNameValue("wheels_number", "4"));
	mParameterTable.insert(ParameterNameValue("wheel_scale", "1.0"));
	mParameterTable.insert(ParameterNameValue("wheel_is_front", "false"));
	mParameterTable.insert(ParameterNameValue("wheel_connection_point_ratio", "1.0,1.0,1.0"));
	mParameterTable.insert(ParameterNameValue("wheel_axle", "1.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("wheel_direction", "0.0,0.0,-1.0"));
	mParameterTable.insert(ParameterNameValue("wheel_suspension_travel", "40.0"));
	mParameterTable.insert(ParameterNameValue("wheel_suspension_stiffness", "40.0"));
	mParameterTable.insert(ParameterNameValue("wheel_damping_relaxation", "2.0"));
	mParameterTable.insert(ParameterNameValue("wheel_damping_compression", "4.0"));
	mParameterTable.insert(ParameterNameValue("wheel_friction_slip", "100.0"));
	mParameterTable.insert(ParameterNameValue("wheel_roll_influence", "0.1"));
	mParameterTable.insert(ParameterNameValue("steering_clamp", "45.0"));
	mParameterTable.insert(ParameterNameValue("steering_increment", "120.0"));
	mParameterTable.insert(ParameterNameValue("steering_decrement", "60.0"));
	mParameterTable.insert(ParameterNameValue("forward", "enabled"));
	mParameterTable.insert(ParameterNameValue("backward", "enabled"));
	mParameterTable.insert(ParameterNameValue("turn_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("turn_right", "enabled"));
}

//TypedObject semantics: hardcoded
TypeHandle VehicleTemplate::_type_handle;

} // namespace ely
