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
 * \file /Ely/src/AIComponents/CrowdAgentTemplate.cpp
 *
 * \date 06/giu/2013 (19:29:33)
 * \author consultit
 */

#include "AIComponents/CrowdAgentTemplate.h"
#include "AIComponents/CrowdAgent.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

CrowdAgentTemplate::CrowdAgentTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid GameAIManager")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

CrowdAgentTemplate::~CrowdAgentTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType CrowdAgentTemplate::componentType() const
{
	return ComponentType("CrowdAgent");
}

ComponentFamilyType CrowdAgentTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)CrowdAgentTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(CrowdAgent) newCrowdAgent = new CrowdAgent(this);
	newCrowdAgent->setComponentId(compId);
	if (not newCrowdAgent->initialize())
	{
		return NULL;
	}
	return newCrowdAgent.p();
}

void CrowdAgentTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("add_to_navmesh", ""));
	mParameterTable.insert(ParameterNameValue("mov_type", "recast"));
	mParameterTable.insert(ParameterNameValue("move_target", "0.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("move_velocity", "0.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("max_acceleration", "8.0"));
	mParameterTable.insert(ParameterNameValue("max_speed", "3.5"));
	mParameterTable.insert(ParameterNameValue("collision_query_range", "12.0"));
	mParameterTable.insert(ParameterNameValue("path_optimization_range", "30.0"));
	mParameterTable.insert(ParameterNameValue("separation_weight", "2.0"));
	mParameterTable.insert(ParameterNameValue("update_flags", "0x1b"));
	mParameterTable.insert(ParameterNameValue("obstacle_avoidance_type", "3"));
	mParameterTable.insert(ParameterNameValue("ray_mask", "all_on"));
}

//TypedObject semantics: hardcoded
TypeHandle CrowdAgentTemplate::_type_handle;

}  // namespace ely
