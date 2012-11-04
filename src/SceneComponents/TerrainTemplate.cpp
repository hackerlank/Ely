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
 * \file /Ely/src/SceneComponents/TerrainTemplate.cpp
 *
 * \date 15/ago/2012 (09:55:34)
 * \author marco
 */

#include "SceneComponents/TerrainTemplate.h"

TerrainTemplate::TerrainTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"TerrainTemplate::TerrainTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"TerrainTemplate::TerrainTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameSceneManager::GetSingletonPtr(),
			"TerrainTemplate::TerrainTemplate: invalid GameSceneManager")
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"TerrainTemplate::TerrainTemplate: invalid PandaFramework or WindowFramework");
	}
	setParametersDefaults();
}

TerrainTemplate::~TerrainTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType TerrainTemplate::componentType() const
{
	return ComponentType("Terrain");
}

const ComponentFamilyType TerrainTemplate::familyType() const
{
	return ComponentFamilyType("Scene");
}

SMARTPTR(Component)TerrainTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(Terrain) newTerrain = new Terrain(this);
	newTerrain->setComponentId(compId);
	if (not newTerrain->initialize())
	{
		return NULL;
	}
	return newTerrain.p();
}

void TerrainTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("height_scale", "1.0"));
	mParameterTable.insert(ParameterNameValue("width_scale", "1.0"));
	mParameterTable.insert(ParameterNameValue("block_size", "64"));
	mParameterTable.insert(ParameterNameValue("near_percent", "0.1"));
	mParameterTable.insert(ParameterNameValue("far_percent", "1.0"));
	mParameterTable.insert(ParameterNameValue("brute_force", "true"));
	mParameterTable.insert(ParameterNameValue("auto_flatten", "AFM_medium"));
	mParameterTable.insert(ParameterNameValue("focal_point", "camera"));
	mParameterTable.insert(ParameterNameValue("minimum_level", "0"));
	mParameterTable.insert(ParameterNameValue("texture_uscale", "1.0"));
	mParameterTable.insert(ParameterNameValue("texture_vscale", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle TerrainTemplate::_type_handle;

