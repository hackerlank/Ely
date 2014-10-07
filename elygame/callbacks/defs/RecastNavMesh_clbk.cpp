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
 * \file /Ely/elygame/callbacks/defs/RecastNavMesh_clbk.cpp
 *
 * \date 13/gen/2014 (08:47:01)
 * \author consultit
 */

#include "../common_configs.h"
#include "AIComponents/NavMesh.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Support/Raycaster.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///RecastNavMesh + NavMesh related
CALLBACK add_crowd_agent_NavMesh_RecastNavMesh;
CALLBACK remove_crowd_agent_NavMesh_RecastNavMesh;
#ifdef ELY_DEBUG
CALLBACK navMeshesToggleDebug;
#endif

#ifdef __cplusplus
}
#endif

#define TOBECLONEDOBJECT "crowdAgentToBeCloned"

///RecastNavMesh + NavMesh related CALLBACKs
void add_crowd_agent_NavMesh_RecastNavMesh(const Event* event, void* data)
{
	//get data
	SMARTPTR(NavMesh)navMesh = reinterpret_cast<NavMesh*>(data);

	//get object to be cloned
	SMARTPTR(Object)toBeClonedObject =
			ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(ObjectId(TOBECLONEDOBJECT));
	RETURN_ON_COND(not toBeClonedObject,)

	//get NavMesh ObjectId
	std::string navMeshObjectId = navMesh->getOwnerObject()->objectId();

	//get the object-to-be-cloned parameter table
	ParameterTable objParams = toBeClonedObject->getStoredObjTmplParams();
	//get the object-to-be-cloned components' parameter tables
	ParameterTableMap compParams = toBeClonedObject->getStoredCompTmplParams();

	//get position under mouse pointer
	Raycaster* rayCaster = Raycaster::GetSingletonPtr();
	RETURN_ON_COND(not rayCaster,)
	LPoint3f hitPos;
	{
		HOLD_REMUTEX(rayCaster->getMutex())

		rayCaster->rayCast(BitMask32::all_on());
		hitPos = rayCaster->getHitPos();
	}

	///create the clone
	//tweak clone object's parameters
	//set clone object store_params
	objParams.erase("store_params");
	objParams.insert(std::make_pair("store_params", "false"));
	//set clone object pos
	objParams.erase("pos");
	std::ostringstream pos;
	pos << hitPos.get_x() << "," << hitPos.get_y() << "," << hitPos.get_z();
	objParams.insert(std::make_pair("pos", pos.str()));

	//tweak clone components' parameter tables
	//set CrowdAgent add_to_plugin
	compParams["CrowdAgent"].erase("add_to_navmesh");
	compParams["CrowdAgent"].insert(std::make_pair("add_to_navmesh", navMeshObjectId));
	//create actually the clone
	ObjectTemplateManager::GetSingletonPtr()->
	createObject(toBeClonedObject->objectTmpl()->objectType(), ObjectId(),
			objParams, compParams, false);
}

void remove_crowd_agent_NavMesh_RecastNavMesh(const Event* event, void* data)
{
	//get data
	SMARTPTR(NavMesh)navMesh = reinterpret_cast<NavMesh*>(data);

	//get object under mouse pointer
	Raycaster* rayCaster = Raycaster::GetSingletonPtr();
	RETURN_ON_COND(not rayCaster,)
	SMARTPTR(Object) hitObject = rayCaster->rayCast(BitMask32::all_on());
	if(hitObject)
	{
		//check if it is has a CrowdAgent component
		SMARTPTR(Component) aiComp = hitObject->getComponent(ComponentFamilyType("AI"));
		if(aiComp and (aiComp->componentType() == ComponentType("CrowdAgent")))
		{
			//try to remove this CrowdAgent from this NavMesh
			if(navMesh->removeCrowdAgent(DCAST(CrowdAgent, aiComp))
					== NavMesh::Result::OK)
			{
				//the CrowdAgent belonged to this NavMesh:
				//remove actually the clone
				ObjectTemplateManager::GetSingletonPtr()->destroyObject(hitObject->objectId());
			}
		}
	}
}

#ifdef ELY_DEBUG
void navMeshesToggleDebug(const Event* event, void* data)
{
	//get data
	SMARTPTR(NavMesh)navMesh = reinterpret_cast<NavMesh*>(data);

	if (navMesh->getDebugNodePath().is_hidden())
	{
		navMesh->debug(true);
	}
	else
	{
		navMesh->debug(false);
	}
}
#endif

///Init/end functions: see common_configs.cpp
void RecastNavMesh_clbkInit()
{
}
void RecastNavMesh_clbkEnd()
{
}

