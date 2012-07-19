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
 * \file /Ely/src/PhysicsComponents/RigidBodyTemplate.cpp
 *
 * \date 07/lug/2012 (15:56:20)
 * \author marco
 */

#include "PhysicsComponents/RigidBodyTemplate.h"

RigidBodyTemplate::RigidBodyTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"RigidBodyTemplate::RigidBodyTemplate: invalid PandaFramework or WindowFramework");
	}
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
	if (not GamePhysicsManager::GetSingletonPtr())
	{
		throw GameException(
				"RigidBodyTemplate::RigidBodyTemplate: invalid GamePhysicsManager");
	}
	setParametersDefaults();
}

RigidBodyTemplate::~RigidBodyTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType RigidBodyTemplate::componentType() const
{
	return ComponentType("RigidBody");
}

const ComponentFamilyType RigidBodyTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

Component* RigidBodyTemplate::makeComponent(const ComponentId& compId)
{
	RigidBody* newRigidBody = new RigidBody(this);
	newRigidBody->componentId() = compId;
	if (not newRigidBody->initialize())
	{
		return NULL;
	}
	return newRigidBody;
}

void RigidBodyTemplate::setParametersDefaults()
{
	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("body_type","dynamic"));
	mParameterTable.insert(ParameterNameValue("body_mass","1.0"));
	mParameterTable.insert(ParameterNameValue("body_friction","0.8"));
	mParameterTable.insert(ParameterNameValue("body_restitution","0.1"));
	mParameterTable.insert(ParameterNameValue("shape_type","sphere"));
	mParameterTable.insert(ParameterNameValue("collide_mask","all_on"));
}

GamePhysicsManager* RigidBodyTemplate::gamePhysicsMgr()
{
	return GamePhysicsManager::GetSingletonPtr();
}

PandaFramework*& RigidBodyTemplate::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& RigidBodyTemplate::windowFramework()
{
	return mWindowFramework;
}

//TypedObject semantics: hardcoded
TypeHandle RigidBodyTemplate::_type_handle;

