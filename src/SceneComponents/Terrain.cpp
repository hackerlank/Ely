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
#include "ObjectModel/ObjectTemplateManager.h"

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
	//Remove from the scene manager update
	//first check if game scene manager exists
	if (GameSceneManager::GetSingletonPtr())
	{
		GameSceneManager::GetSingletonPtr()->removeFromSceneUpdate(this);
	}
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

	bool result = true;
	//get scale
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
	//get LOD
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
	//get block size
	int blockSize = atoi(mTmpl->parameter(std::string("block_size")).c_str());
	if (blockSize <= 0)
	{
		blockSize = 64;
	}
	//get brute force
	mBruteForce = (
			mTmpl->parameter(std::string("brute_force"))
					== std::string("false") ? false : true);
	//get auto flatten
	std::string autoFlatten = mTmpl->parameter(std::string("auto_flatten"));
	GeoMipTerrain::AutoFlattenMode flattenMode;
	if (autoFlatten == "AFM_off")
	{
		flattenMode = GeoMipTerrain::AFM_off;
	}
	else if (autoFlatten == "AFM_light")
	{
		flattenMode = GeoMipTerrain::AFM_light;
	}
	else if (autoFlatten == "AFM_strong")
	{
		flattenMode = GeoMipTerrain::AFM_strong;
	}
	else
	{
		flattenMode = GeoMipTerrain::AFM_medium;
	}
	//get focal point
	mFocalPoint = ObjectId(mTmpl->parameter(std::string("focal_point")));
	//get heightfield image
	PNMImage heightField(
			Filename(mTmpl->parameter(std::string("heightfield_file"))));
	//get texture scale
	float textureUscale = atof(
			mTmpl->parameter(std::string("texture_uscale")).c_str());
	if (textureUscale <= 0.0)
	{
		textureUscale = 1.0;
	}
	float textureVscale = atof(
			mTmpl->parameter(std::string("texture_vscale")).c_str());
	if (textureVscale <= 0.0)
	{
		textureVscale = 1.0;
	}
	//get texture
	SMARTPTR(Texture)textureImage = TexturePool::load_texture(
			Filename(mTmpl->parameter(std::string("texture_file"))));
	//create the actual terrain
	//terrain definition
	mTerrain = new GeoMipTerrainRef(
			std::string("terrain") + std::string(mComponentId));
	//set height field
	if (not mTerrain->set_heightfield(heightField))
	{
		result = false;
	}
	//sizing
	float environmentWidthX = (heightField.get_x_size() - 1) * widthScale;
	float environmentWidthY = (heightField.get_y_size() - 1) * widthScale;
	float environmentWidth = (environmentWidthX + environmentWidthY) / 2.0;
	//set terrain properties effectively
	mTerrain->set_block_size(blockSize);
	mTerrain->set_near(nearPercent * environmentWidth);
	mTerrain->set_far(farPercent * environmentWidth);
	//other properties
	float terrainLODmin = min<float>(0, mTerrain->get_max_level());
	mTerrain->set_min_level(terrainLODmin);
	mTerrain->set_auto_flatten(flattenMode);
	mTerrain->set_bruteforce(mBruteForce);
	mTerrain->get_root().set_sx(widthScale);
	mTerrain->get_root().set_sy(widthScale);
	mTerrain->get_root().set_sz(heightScale);
	//terrain texturing
	mTerrain->get_root().set_tex_scale(TextureStage::get_default(),
			textureUscale, textureVscale);
	mTerrain->get_root().set_texture(TextureStage::get_default(), textureImage,
			1);
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
	//set the focal point
	SMARTPTR(Object)createdObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mFocalPoint);
	if (createdObject != NULL)
	{
		mTerrain->set_focal_point(createdObject->getNodePath());
	}
	else
	{
		//set render as focal point
		createdObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
				"render");
		mTerrain->set_focal_point(createdObject->getNodePath());
	}
	//Generate the terrain
	mTerrain->generate();
	//Add to the scene manager update if not brute force
	//first check if game scene manager exists
	if (GameSceneManager::GetSingletonPtr() and (not mBruteForce))
	{
		GameSceneManager::GetSingletonPtr()->addToSceneUpdate(this);
	}
	//register event callbacks if any
	registerEventCallbacks();
}

void Terrain::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

//	float dt = *(reinterpret_cast<float*>(data));

	//update every frame
	mTerrain->update();
}

//TypedObject semantics: hardcoded
TypeHandle Terrain::_type_handle;

///GeoMipTerrainRef stuff
GeoMipTerrainRef::GeoMipTerrainRef(const std::string& name) :
		GeoMipTerrain(name)
{
}

//TypedObject semantics: hardcoded
TypeHandle GeoMipTerrainRef::_type_handle;

