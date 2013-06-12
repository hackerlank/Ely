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

CrowdAgentTemplate::CrowdAgentTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid GameAIManager")
	//
	setParametersDefaults();
}

CrowdAgentTemplate::~CrowdAgentTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType CrowdAgentTemplate::componentType() const
{
	return ComponentType("CrowdAgent");
}

const ComponentFamilyType CrowdAgentTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)CrowdAgentTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("throw_events", "false"));
	mParameterTable.insert(ParameterNameValue("controlled_type", "nodepath"));
}

//TypedObject semantics: hardcoded
TypeHandle CrowdAgentTemplate::_type_handle;
