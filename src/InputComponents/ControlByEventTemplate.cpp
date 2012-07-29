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
 * \file /Ely/src/InputComponents/ControlByEventTemplate.cpp
 *
 * \date 31/mag/2012 (16:44:06)
 * \author marco
 */

#include "InputComponents/ControlByEventTemplate.h"

ControlByEventTemplate::ControlByEventTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ControlByEventTemplate::ControlByEventTemplate: invalid PandaFramework or WindowFramework");
	}
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
	setParametersDefaults();
}

ControlByEventTemplate::~ControlByEventTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType ControlByEventTemplate::componentType() const
{
	return ComponentType("ControlByEvent");
}

const ComponentFamilyType ControlByEventTemplate::familyType() const
{
	return ComponentFamilyType("Input");
}

Component* ControlByEventTemplate::makeComponent(const ComponentId& compId)
{
	ControlByEvent* newControl = new ControlByEvent(this);
	newControl->componentId() = compId;
	if (not newControl->initialize())
	{
		return NULL;
	}
	return newControl;
}

void ControlByEventTemplate::setParametersDefaults()
{
	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("enabled","true"));
	mParameterTable.insert(ParameterNameValue("forward","w"));
	mParameterTable.insert(ParameterNameValue("backward","s"));
	mParameterTable.insert(ParameterNameValue("roll_left","a"));
	mParameterTable.insert(ParameterNameValue("roll_right","d"));
	mParameterTable.insert(ParameterNameValue("strafe_left","q"));
	mParameterTable.insert(ParameterNameValue("strafe_right","e"));
	mParameterTable.insert(ParameterNameValue("up","r"));
	mParameterTable.insert(ParameterNameValue("down","f"));
	mParameterTable.insert(ParameterNameValue("speed_key","shift"));
	mParameterTable.insert(ParameterNameValue("speed","5.0"));
	mParameterTable.insert(ParameterNameValue("fast_factor","5.0"));
	mParameterTable.insert(ParameterNameValue("mov_sens","2.0"));
	mParameterTable.insert(ParameterNameValue("roll_sens","15.0"));
	mParameterTable.insert(ParameterNameValue("sens_x","0.2"));
	mParameterTable.insert(ParameterNameValue("sens_y","0.2"));
	mParameterTable.insert(ParameterNameValue("inverted_keyboard","false"));
	mParameterTable.insert(ParameterNameValue("inverted_mouse","false"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_h","false"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_p","false"));
}

PandaFramework*& ControlByEventTemplate::pandaFramework()
{
	return mPandaFramework;
}

GameInputManager* ControlByEventTemplate::gameInputMgr()
{
	return GameInputManager::GetSingletonPtr();
}

WindowFramework*& ControlByEventTemplate::windowFramework()
{
	return mWindowFramework;
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEventTemplate::_type_handle;

