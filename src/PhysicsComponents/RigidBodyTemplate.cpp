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
 * \author consultit
 */

#include "PhysicsComponents/RigidBodyTemplate.h"
#include "PhysicsComponents/RigidBody.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

RigidBodyTemplate::RigidBodyTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"RigidBodyTemplate::RigidBodyTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"RigidBodyTemplate::RigidBodyTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"RigidBodyTemplate::RigidBodyTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

RigidBodyTemplate::~RigidBodyTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType RigidBodyTemplate::componentType() const
{
	return ComponentType("RigidBody");
}

ComponentFamilyType RigidBodyTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

SMARTPTR(Component)RigidBodyTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(RigidBody) newRigidBody = new RigidBody(this);
	newRigidBody->setComponentId(compId);
	if (not newRigidBody->initialize())
	{
		return NULL;
	}
	return newRigidBody.p();
}

void RigidBodyTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("body_type", "dynamic"));
	mParameterTable.insert(ParameterNameValue("body_mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("body_friction", "0.8"));
	mParameterTable.insert(ParameterNameValue("body_restitution", "0.1"));
	mParameterTable.insert(ParameterNameValue("shape_type", "sphere"));
	mParameterTable.insert(ParameterNameValue("shape_size", "medium"));
	mParameterTable.insert(ParameterNameValue("collide_mask", "all_on"));
	mParameterTable.insert(ParameterNameValue("shape_height", "1.0"));
	mParameterTable.insert(ParameterNameValue("shape_up", "z"));
	mParameterTable.insert(ParameterNameValue("shape_scale_w", "1.0"));
	mParameterTable.insert(ParameterNameValue("shape_scale_d", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle RigidBodyTemplate::_type_handle;

} // namespace ely
