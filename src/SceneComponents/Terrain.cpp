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

Terrain::Terrain(SMARTPTR(TerrainTemplate)tmpl)
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
// * - ""			|single|"1.0"
// * - ""			|single|"1.0"
// * - ""				|single|"64"
// * - ""			|single|"0.1"
// * - ""			|single|"1.0"
// * - ""			|single|"true"
// * - ""			|single|"AFM_medium"
// * - "texture_file"			|single|no default
// * - "texture_uscale"			|single|"1.0"
// * - "texture_vscale"			|single|"1.0"

	bool result = true;
	//set scale
	float heightScale = atof(
			mTmpl->parameter(std::string("height_scale")).c_str());
	if (heightScale <= 0.0)
	{
		heightScale = 1.0;
	}
	float widthScale = atof(
			mTmpl->parameter(std::string("width_scale")).c_str());
	if (widthScale <= 0.0)
	{
		widthScale = 1.0;
	}
	//set LOD
	float nearPercent = atof(
			mTmpl->parameter(std::string("near_percent")).c_str());
	float farPercent = atof(
			mTmpl->parameter(std::string("far_percent")).c_str());
	if ((nearPercent <= 0.0) or (farPercent >= 1.0)
			or (nearPercent >= farPercent))
	{
		farPercent = 1.0;
		nearPercent = 0.1;
	}
	//set block size
	int blockSize = atoi(mTmpl->parameter(std::string("block_size")).c_str());
	if (blockSize <= 0)
	{
		blockSize = 64;
	}
	//set brute force
	bool bruteForce = (
			mTmpl->parameter(std::string("brute_force"))
					== std::string("false") ? false : true);
	//set auto flatten
	std::string autoFlatten = mTmpl->parameter(std::string("auto_flatten"));
	GeoMipTerrain::AutoFlattenMode flattenMode;
	if (autoFlatten == "AFM_off")
	{
		flattenMode = GeoMipTerrain::AFM_off;
	}
	if (autoFlatten == "AFM_light")
	{
		flattenMode = GeoMipTerrain::AFM_light;
	}
	if (autoFlatten == "AFM_strong")
	{
		flattenMode = GeoMipTerrain::AFM_strong;
	}
	else
	{
		flattenMode = GeoMipTerrain::AFM_medium;
	}
	//set heightfield file

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
