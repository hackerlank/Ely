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
 * \file /Ely/src/BehaviorComponents/ActivityTemplate.cpp
 *
 * \date 17/ago/2012 (09:27:08)
 * \author consultit
 */

#include "BehaviorComponents/ActivityTemplate.h"
#include "BehaviorComponents/Activity.h"

namespace ely
{

ActivityTemplate::ActivityTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"ActivityTemplate::ActivityTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"ActivityTemplate::ActivityTemplate: invalid WindowFramework")
	//
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
	return ComponentFamilyType("Behavior");
}

SMARTPTR(Component)ActivityTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(Activity) newActivity = new Activity(this);
	newActivity->setComponentId(compId);
	if (not newActivity->initialize())
	{
		return NULL;
	}
	return newActivity.p();
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

} // namespace ely
