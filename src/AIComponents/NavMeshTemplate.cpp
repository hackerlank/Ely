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
#include "AIComponents/NavMesh.h"
#include "Game/GameAIManager.h"

namespace ely
{

NavMeshTemplate::NavMeshTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"NavMeshTemplate::NavMeshTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"NavMeshTemplate::NavMeshTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"NavMeshTemplate::NavMeshTemplate: invalid GameAIManager")
	//
	setParametersDefaults();
}

NavMeshTemplate::~NavMeshTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType NavMeshTemplate::componentType() const
{
	return ComponentType("NavMesh");
}

ComponentFamilyType NavMeshTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)NavMeshTemplate::makeComponent(const ComponentId& compId)
{
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
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("navmesh_type", "solo"));
	mParameterTable.insert(ParameterNameValue("auto_setup", "true"));
	mParameterTable.insert(ParameterNameValue("mov_type", "recast"));
	mParameterTable.insert(ParameterNameValue("cell_size", "0.3"));
	mParameterTable.insert(ParameterNameValue("cell_height", "0.2"));
	mParameterTable.insert(ParameterNameValue("agent_height", "2.0"));
	mParameterTable.insert(ParameterNameValue("agent_radius", "0.6"));
	mParameterTable.insert(ParameterNameValue("agent_max_climb", "0.9"));
	mParameterTable.insert(ParameterNameValue("agent_max_slope", "45.0"));
	mParameterTable.insert(ParameterNameValue("region_min_size", "8"));
	mParameterTable.insert(ParameterNameValue("region_merge_size", "20"));
	mParameterTable.insert(
			ParameterNameValue("monotone_partitioning", "false"));
	mParameterTable.insert(ParameterNameValue("edge_max_len", "12.0"));
	mParameterTable.insert(ParameterNameValue("edge_max_error", "1.3"));
	mParameterTable.insert(ParameterNameValue("verts_per_poly", "6.0"));
	mParameterTable.insert(ParameterNameValue("detail_sample_dist", "6.0"));
	mParameterTable.insert(
			ParameterNameValue("detail_sample_max_error", "1.0"));
	//nav mesh tile
	mParameterTable.insert(ParameterNameValue("build_all_tiles", "false"));
	mParameterTable.insert(ParameterNameValue("max_tiles", "128"));
	mParameterTable.insert(ParameterNameValue("max_polys_per_tile", "32768"));
	mParameterTable.insert(ParameterNameValue("tile_size", "32"));
}

//TypedObject semantics: hardcoded
TypeHandle NavMeshTemplate::_type_handle;

}  // namespace ely
