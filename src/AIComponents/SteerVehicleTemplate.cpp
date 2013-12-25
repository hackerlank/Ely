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
 * \file /Ely/src/AIComponents/SteerVehicleTemplate.cpp
 *
 * \date 04/dic/2013 (09:23:38)
 * \author consultit
 */
#include "AIComponents/SteerVehicleTemplate.h"
#include "AIComponents/SteerVehicle.h"
#include "Game/GameAIManager.h"

namespace ely
{

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
	// TODO Auto-generated destructor stub
}

ComponentType SteerVehicleTemplate::componentType() const
{
	return ComponentType("SteerVehicle");
}

ComponentFamilyType SteerVehicleTemplate::familyType() const
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
	mParameterTable.insert(ParameterNameValue("throw_events", "false"));
	mParameterTable.insert(ParameterNameValue("add_to_plugin", ""));
	mParameterTable.insert(ParameterNameValue("mov_type", "opensteer"));
	mParameterTable.insert(ParameterNameValue("type", "one_turning"));
	mParameterTable.insert(ParameterNameValue("mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("speed", "0.0"));
	mParameterTable.insert(ParameterNameValue("max_force", "0.1"));
	mParameterTable.insert(ParameterNameValue("max_speed", "1.0"));
	mParameterTable.insert(ParameterNameValue("ray_mask", "all_on"));
}

//TypedObject semantics: hardcoded
TypeHandle SteerVehicleTemplate::_type_handle;

} /* namespace ely */
