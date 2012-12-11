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
 * \author marco
 */

#include "AIComponents/SteeringTemplate.h"

SteeringTemplate::SteeringTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"SteeringTemplate::SteeringTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"SteeringTemplate::SteeringTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"SteeringTemplate::SteeringTemplate: invalid GameAIManager")
	//
	setParametersDefaults();
}

SteeringTemplate::~SteeringTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType SteeringTemplate::componentType() const
{
	return ComponentType("Steering");
}

const ComponentFamilyType SteeringTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)SteeringTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("controlled_type", "nodepath"));
	mParameterTable.insert(ParameterNameValue("behavior", "seek"));
	mParameterTable.insert(ParameterNameValue("mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("movt_force", "1.0"));
	mParameterTable.insert(ParameterNameValue("max_force", "1.0"));
	mParameterTable.insert(ParameterNameValue("seek_wt", "1.0"));
	mParameterTable.insert(ParameterNameValue("panic_distance", "10.0"));
	mParameterTable.insert(ParameterNameValue("relax_distance", "10.0"));
	mParameterTable.insert(ParameterNameValue("flee_wt", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle SteeringTemplate::_type_handle;