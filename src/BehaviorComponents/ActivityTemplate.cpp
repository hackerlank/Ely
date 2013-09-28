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
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"ActivityTemplate::ActivityTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"ActivityTemplate::ActivityTemplate: invalid WindowFramework")
	//
	setParametersDefaults();
}

ActivityTemplate::~ActivityTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType ActivityTemplate::componentType() const
{
	return ComponentType("Activity");
}

ComponentFamilyType ActivityTemplate::familyType() const
{
	return ComponentFamilyType("Behavior");
}

SMARTPTR(Component)ActivityTemplate::makeComponent(const ComponentId& compId)
{
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
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("instance_update", ""));
}

//TypedObject semantics: hardcoded
TypeHandle ActivityTemplate::_type_handle;

} // namespace ely
