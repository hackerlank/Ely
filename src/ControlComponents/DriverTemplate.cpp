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
 * \file /Ely/src/ControlComponents/DriverTemplate.cpp
 *
 * \date 31/mag/2012 (16:44:06)
 * \author consultit
 */

#include "ControlComponents/DriverTemplate.h"
#include "ControlComponents/Driver.h"
#include "Game/GameControlManager.h"

namespace ely
{

DriverTemplate::DriverTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"DriverTemplate::DriverTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"DriverTemplate::DriverTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameControlManager::GetSingletonPtr(),
			"DriverTemplate::DriverTemplate: invalid GameControlManager")
	//
	setParametersDefaults();
}

DriverTemplate::~DriverTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType DriverTemplate::componentType() const
{
	return ComponentType("Driver");
}

ComponentFamilyType DriverTemplate::familyType() const
{
	return ComponentFamilyType("Control");
}

SMARTPTR(Component)DriverTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Driver) newDriver = new Driver(this);
	newDriver->setComponentId(compId);
	if (not newDriver->initialize())
	{
		return NULL;
	}
	return newDriver.p();
}

void DriverTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("forward", "enabled"));
	mParameterTable.insert(ParameterNameValue("backward", "enabled"));
	mParameterTable.insert(ParameterNameValue("roll_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("roll_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("up", "enabled"));
	mParameterTable.insert(ParameterNameValue("down", "enabled"));
	mParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
	mParameterTable.insert(ParameterNameValue("speed_key", "shift"));
	mParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
	mParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
	mParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
	mParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
	mParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
	mParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
	mParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
	mParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
	mParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
	mParameterTable.insert(ParameterNameValue("inverted_keyboard", "false"));
	mParameterTable.insert(ParameterNameValue("inverted_mouse", "false"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_h", "false"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_p", "false"));
}

//TypedObject semantics: hardcoded
TypeHandle DriverTemplate::_type_handle;

} // namespace ely
