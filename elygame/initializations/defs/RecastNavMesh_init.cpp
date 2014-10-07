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
 * \file /Ely/elygame/initializations/defs/RecastNavMesh_init.cpp
 *
 * \date 22/mar/2014 (10:14:24)
 * \author consultit
 */

#include "../common_configs.h"
#include "AIComponents/NavMesh.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GamePhysicsManager.h"

///RecastNavMesh objects related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION course2_initialization;

#ifdef __cplusplus
}
#endif

///course2
#define TOBECLONEDOBJECT "crowdAgentToBeCloned"

void course2_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//find the crowd agent TOBECLONEDOBJECT
	SMARTPTR(Object) crowdAgentObject = ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject(ObjectId(TOBECLONEDOBJECT));
	if(crowdAgentObject)
	{
		//get the TOBECLONEDOBJECT dimensions
		LVecBase3f modelDims;
		LVector3f modelDeltaCenter;
		float modelRadius;
		GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
				crowdAgentObject->getNodePath(), modelDims, modelDeltaCenter,
				modelRadius);
		//get NavMesh component
		SMARTPTR(NavMesh) navMeshComp = DCAST(NavMesh,
				object->getComponent(ComponentFamilyType("AI")));
		//set crowd agent dimensions
		NavMeshSettings navMeshSettings = navMeshComp->getNavMeshSettings();
		navMeshSettings.m_agentRadius = modelRadius;
		navMeshSettings.m_agentHeight = modelDims.get_z();
		navMeshComp->setNavMeshSettings(navMeshSettings);
		//now setup the underlying NavMeshType
		if (navMeshComp->navMeshSetup() != NavMesh::Result::OK)
		{
			PRINT_ERR_DEBUG("Error: navMeshSetup()");
		}
	}
}

///init/end
void RecastNavMesh_initInit()
{
}

void RecastNavMesh_initEnd()
{
}

