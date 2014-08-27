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
 * \file /Ely/src/PhysicsComponents/GhostTemplate.cpp
 *
 * \date 13/ago/2014 (11:39:39)
 * \author consultit
 */

#include "PhysicsComponents/GhostTemplate.h"
#include "PhysicsComponents/Ghost.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

GhostTemplate::GhostTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"GhostTemplate::GhostTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"GhostTemplate::GhostTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"GhostTemplate::GhostTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

GhostTemplate::~GhostTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType GhostTemplate::componentType() const
{
	return ComponentType(Ghost::get_class_type().get_name());
}

ComponentFamilyType GhostTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

SMARTPTR(Component)GhostTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Ghost) newGhost = new Ghost(this);
	newGhost->setComponentId(compId);
	if (not newGhost->initialize())
	{
		return NULL;
	}
	return newGhost.p();
}

void GhostTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("ghost_type", "static"));
	mParameterTable.insert(ParameterNameValue("ghost_friction", "0.8"));
	mParameterTable.insert(ParameterNameValue("ghost_restitution", "0.1"));
	mParameterTable.insert(ParameterNameValue("shape_type", "sphere"));
	mParameterTable.insert(ParameterNameValue("shape_size", "medium"));
	mParameterTable.insert(ParameterNameValue("collide_mask", "all_on"));
	mParameterTable.insert(ParameterNameValue("shape_height", "1.0"));
	mParameterTable.insert(ParameterNameValue("shape_up", "z"));
	mParameterTable.insert(ParameterNameValue("shape_scale_w", "1.0"));
	mParameterTable.insert(ParameterNameValue("shape_scale_d", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle GhostTemplate::_type_handle;

} /* namespace ely */
