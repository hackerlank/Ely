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
 * \file /Ely/src/AIComponents/SteeringTemplate.cpp
 *
 * \date 03/dic/2012 (13:52:08)
 * \author consultit
 */

#include "AIComponents/SteeringTemplate.h"
#include "Game/GameAIManager.h"
#include "AIComponents/Steering.h"

namespace ely
{

SteeringTemplate::SteeringTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE(pandaFramework,
			"SteeringTemplate::SteeringTemplate: invalid PandaFramework")
	CHECK_EXISTENCE(windowFramework,
			"SteeringTemplate::SteeringTemplate: invalid WindowFramework")
	CHECK_EXISTENCE(GameAIManager::GetSingletonPtr(),
			"SteeringTemplate::SteeringTemplate: invalid GameAIManager")
	//
	setParametersDefaults();
}

SteeringTemplate::~SteeringTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType SteeringTemplate::componentType() const
{
	return ComponentType("Steering");
}

ComponentFamilyType SteeringTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)SteeringTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Steering) newSteering = new Steering(this);
	newSteering->setComponentId(compId);
	if (not newSteering->initialize())
	{
		return NULL;
	}
	return newSteering.p();
}

void SteeringTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("throw_events", "false"));
	mParameterTable.insert(ParameterNameValue("controlled_type", "nodepath"));
	mParameterTable.insert(ParameterNameValue("mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("movt_force", "1.0"));
	mParameterTable.insert(ParameterNameValue("max_force", "1.0"));
	mParameterTable.insert(ParameterNameValue("obstacle_hit_mask", "0x80000000"));
	mParameterTable.insert(ParameterNameValue("obstacle_max_distance_fraction", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle SteeringTemplate::_type_handle;

}  // namespace ely
