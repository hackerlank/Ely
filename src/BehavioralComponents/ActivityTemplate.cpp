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
 * \file /Ely/src/BehavioralComponents/ActivityTemplate.cpp
 *
 * \date 17/ago/2012 (09:27:08)
 * \author marco
 */

#include "BehavioralComponents/ActivityTemplate.h"

ActivityTemplate::ActivityTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ActivityTemplate::ActivityTemplate: invalid PandaFramework or WindowFramework");
	}
	setParametersDefaults();
}

ActivityTemplate::~ActivityTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType ActivityTemplate::componentType() const
{
	return ComponentType("Activity");
}

const ComponentFamilyType ActivityTemplate::familyType() const
{
	return ComponentFamilyType("Behavioral");
}

Component* ActivityTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	Activity* newActivity = new Activity(this);
	newActivity->setComponentId(compId);
	if (not newActivity->initialize())
	{
		return NULL;
	}
	return newActivity;
}

void ActivityTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
}

//TypedObject semantics: hardcoded
TypeHandle ActivityTemplate::_type_handle;
