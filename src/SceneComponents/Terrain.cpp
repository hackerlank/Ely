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
 * \file /Ely/src/SceneComponents/Terrain.cpp
 *
 * \date 15/ago/2012 (10:00:08)
 * \author marco
 */

#include "SceneComponents/Terrain.h"
#include "SceneComponents/TerrainTemplate.h"

Terrain::Terrain()
{
	// TODO Auto-generated constructor stub
}

Terrain::Terrain(SMARTPTR(TerrainTemplate) tmpl)
{
	mTmpl = tmpl;
}

Terrain::~Terrain()
{
	mTerrain->get_root().remove_node();
}

const ComponentFamilyType Terrain::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Terrain::componentType() const
{
	return mTmpl.p()->componentType();
}

bool Terrain::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

// * - "heightfield_file"  		|single|no default
// * - "height_scale"			|single|"1.0"
// * - "width_scale"			|single|"1.0"
// * - "update_interval"		|single|"0.1"
// * - "block_size"				|single|"64"
// * - "near_percent"			|single|"0.1"
// * - "far_percent"			|single|"1.0"
// * - "brute_force"			|single|"true"
// * - "auto_flatten"			|single|"AFM_medium"
// * - "texture_file"			|single|no default
// * - "texture_uscale"			|single|"1.0"
// * - "texture_vscale"			|single|"1.0"

	//setup physical parameters


	bool result = true;
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void Terrain::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set the node path of the object to the
	//node path of this model
	mOwnerObject->setNodePath(mTerrain->get_root());
	//register event callbacks if any
	registerEventCallbacks();
}

//TypedObject semantics: hardcoded
TypeHandle Terrain::_type_handle;

//TypedObject semantics: hardcoded
TypeHandle GeoMipTerrainRef::_type_handle;
