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

Component* TerrainTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	Terrain* newTerrain = new Terrain(this);
	newTerrain->setComponentId(compId);
	if (not newTerrain->initialize())
	{
		return NULL;
	}
	return newTerrain;
}

void TerrainTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
}

PandaFramework* const TerrainTemplate::pandaFramework() const
{
	return mPandaFramework;
}

WindowFramework* const TerrainTemplate::windowFramework() const
{
	return mWindowFramework;
}

//TypedObject semantics: hardcoded
TypeHandle TerrainTemplate::_type_handle;

