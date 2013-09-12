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
 * \file /Ely/src/ControlComponents/ChaserTemplate.cpp
 *
 * \date 11/nov/2012 (09:43:16)
 * \author consultit
 */

#include "ControlComponents/ChaserTemplate.h"
#include "ControlComponents/Chaser.h"
#include "Game/GameControlManager.h"

namespace ely
{

ChaserTemplate::ChaserTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"DriverTemplate::ChaserTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"DriverTemplate::ChaserTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameControlManager::GetSingletonPtr(),
			"DriverTemplate::ChaserTemplate: invalid GameControlManager")
	//
	setParametersDefaults();
}

ChaserTemplate::~ChaserTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType ChaserTemplate::componentType() const
{
	return ComponentType("Chaser");
}

ComponentFamilyType ChaserTemplate::familyType() const
{
	return ComponentFamilyType("Control");
}

SMARTPTR(Component)ChaserTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Chaser) newChaser = new Chaser(this);
	newChaser->setComponentId(compId);
	if (not newChaser->initialize())
	{
		return NULL;
	}
	return newChaser.p();
}

void ChaserTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("backward", "true"));
	mParameterTable.insert(ParameterNameValue("fixed_relative_position", "true"));
	mParameterTable.insert(ParameterNameValue("friction", "1.0"));
	mParameterTable.insert(ParameterNameValue("fixed_lookat", "true"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_h", "false"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_p", "false"));
	mParameterTable.insert(ParameterNameValue("head_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("head_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
	mParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
	mParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
	mParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
	mParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
}

//TypedObject semantics: hardcoded
TypeHandle ChaserTemplate::_type_handle;

} // namespace ely
