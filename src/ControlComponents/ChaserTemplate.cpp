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
 * \date 11/nov/2012 09:43:16
 * \author marco
 */

#include "ControlComponents/ChaserTemplate.h"

ChaserTemplate::ChaserTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"DriverTemplate::ChaserTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"DriverTemplate::ChaserTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameControlManager::GetSingletonPtr(),
			"DriverTemplate::ChaserTemplate: invalid GameControlManager")
	//
	setParametersDefaults();
}

ChaserTemplate::~ChaserTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType ChaserTemplate::componentType() const
{
	return ComponentType("Chaser");
}

const ComponentFamilyType ChaserTemplate::familyType() const
{
	return ComponentFamilyType("Control");
}

SMARTPTR(Component)ChaserTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("friction", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle ChaserTemplate::_type_handle;
