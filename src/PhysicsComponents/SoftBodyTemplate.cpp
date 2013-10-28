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
 * \file /Ely/src/PhysicsComponents/SoftBodyTemplate.cpp
 *
 * \date 08/ott/20132 (08:00:35)
 * \author consultit
 */

#include "PhysicsComponents/SoftBodyTemplate.h"
#include "PhysicsComponents/SoftBody.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

SoftBodyTemplate::SoftBodyTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"SoftBodyTemplate::SoftBodyTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"SoftBodyTemplate::SoftBodyTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"SoftBodyTemplate::SoftBodyTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

SoftBodyTemplate::~SoftBodyTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType SoftBodyTemplate::componentType() const
{
	return ComponentType("SoftBody");
}

ComponentFamilyType SoftBodyTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

SMARTPTR(Component)SoftBodyTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(SoftBody) newSoftBody = new SoftBody(this);
	newSoftBody->setComponentId(compId);
	if (not newSoftBody->initialize())
	{
		return NULL;
	}
	return newSoftBody.p();
}

void SoftBodyTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("body_type", "rope"));
	mParameterTable.insert(ParameterNameValue("collide_mask", "all_on"));
	mParameterTable.insert(ParameterNameValue("body_total_mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("body_mass_from_faces", "false"));
	mParameterTable.insert(ParameterNameValue("air_density", "1.2"));
	mParameterTable.insert(ParameterNameValue("water_density", "0.0"));
	mParameterTable.insert(ParameterNameValue("water_offset", "0.0"));
	mParameterTable.insert(ParameterNameValue("water_normal", "0.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("gendiags", "true"));
	mParameterTable.insert(ParameterNameValue("radius", "1.0,1.0,1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle SoftBodyTemplate::_type_handle;

} /* namespace ely */
