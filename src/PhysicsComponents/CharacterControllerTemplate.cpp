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
 * \file /Ely/src/PhysicsComponents/CharacterControllerTemplate.cpp
 *
 * \date 30/ott/2012 17:01:34
 * \author marco
 */

#include "PhysicsComponents/CharacterControllerTemplate.h"

CharacterControllerTemplate::CharacterControllerTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"CharacterTemplate::CharacterTemplate: invalid PandaFramework or WindowFramework");
	}
	if (not GamePhysicsManager::GetSingletonPtr())
	{
		throw GameException(
				"CharacterTemplate::CharacterTemplate: invalid GamePhysicsManager");
	}
	setParametersDefaults();
}

CharacterControllerTemplate::~CharacterControllerTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType CharacterControllerTemplate::componentType() const
{
	return ComponentType("CharacterController");
}

const ComponentFamilyType CharacterControllerTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

SMARTPTR(Component)CharacterControllerTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(CharacterController) newCharacter = new CharacterController(this);
	newCharacter->setComponentId(compId);
	if (not newCharacter->initialize())
	{
		return NULL;
	}
	return newCharacter.p();
}

void CharacterControllerTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
}

//TypedObject semantics: hardcoded
TypeHandle CharacterControllerTemplate::_type_handle;

