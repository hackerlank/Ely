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
 * \file /Ely/src/AIComponents/OpenSteerVehicleTemplate.cpp
 *
 * \date 04/dic/2013 (09:23:38)
 * \author consultit
 */
#include "AIComponents/OpenSteerVehicleTemplate.h"
#include "AIComponents/OpenSteerVehicle.h"
#include "Game/GameAIManager.h"

namespace ely
{

OpenSteerVehicleTemplate::OpenSteerVehicleTemplate(
		PandaFramework* pandaFramework, WindowFramework* windowFramework) :
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

OpenSteerVehicleTemplate::~OpenSteerVehicleTemplate()
{
	// TODO Auto-generated destructor stub
}


ComponentType OpenSteerVehicleTemplate::componentType() const
{
	return ComponentType("OpenSteerVehicle");
}

ComponentFamilyType OpenSteerVehicleTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)OpenSteerVehicleTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(OpenSteerVehicle) newOpenSteerVehicle = new OpenSteerVehicle(this);
	newOpenSteerVehicle->setComponentId(compId);
	if (not newOpenSteerVehicle->initialize())
	{
		return NULL;
	}
	return newOpenSteerVehicle.p();
}

void OpenSteerVehicleTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
}

//TypedObject semantics: hardcoded
TypeHandle OpenSteerVehicleTemplate::_type_handle;

} /* namespace ely */
