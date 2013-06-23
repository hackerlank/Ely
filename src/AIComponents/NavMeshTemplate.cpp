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
 * \file /Ely/src/AIComponents/NavMeshTemplate.cpp
 *
 * \date 23/giu/2013 (18:59:22)
 * \author consultit
 */

#include "AIComponents/NavMeshTemplate.h"

using namespace ely;

NavMeshTemplate::NavMeshTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"NavMeshTemplate::NavMeshTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"NavMeshTemplate::NavMeshTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"NavMeshTemplate::NavMeshTemplate: invalid GameAIManager")
	//
	setParametersDefaults();
}

NavMeshTemplate::~NavMeshTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType NavMeshTemplate::componentType() const
{
	return ComponentType("NavMesh");
}

const ComponentFamilyType NavMeshTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)NavMeshTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(NavMesh) newNavMesh = new NavMesh(this);
	newNavMesh->setComponentId(compId);
	if (not newNavMesh->initialize())
	{
		return NULL;
	}
	return newNavMesh.p();
}

void NavMeshTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
}

//TypedObject semantics: hardcoded
TypeHandle NavMeshTemplate::_type_handle;
