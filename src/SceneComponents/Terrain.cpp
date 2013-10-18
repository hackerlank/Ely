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
 * \author consultit
 */

#include "SceneComponents/Terrain.h"
#include "SceneComponents/TerrainTemplate.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameSceneManager.h"
#include <texturePool.h>
#include <cmath>

namespace ely
{

Terrain::Terrain()
{
	// TODO Auto-generated constructor stub
}

Terrain::Terrain(SMARTPTR(TerrainTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameSceneManager::GetSingletonPtr(),
			"Terrain::Terrain: invalid GameSceneManager")

	mTmpl = tmpl;
	reset();
}

Terrain::~Terrain()
{
}

ComponentFamilyType Terrain::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Terrain::componentType() const
{
	return mTmpl->componentType();
}

bool Terrain::initialize()
{
	bool result = true;
	//
	float value;
	int valueInt;
	//height scale
	value = strtof(mTmpl->parameter(std::string("height_scale")).c_str(), NULL);
	mHeightScale = (value >= 0.0 ? value : -value);
	//width scale
	value = strtof(mTmpl->parameter(std::string("width_scale")).c_str(), NULL);
	mWidthScale = (value >= 0.0 ? value : -value);
	//do scale
	mDoScale = (
			mTmpl->parameter(std::string("do_scale")) == std::string("false") ?
					false : true);
	//LOD
	//near percent [0.0, 1.0]
	value = strtof(mTmpl->parameter(std::string("near_percent")).c_str(),
	NULL);
	mNearPercent = (value >= 0.0 ? value - floor(value) : ceil(value) - value);
	//far percent [0.0, 1.0]
	value = strtof(mTmpl->parameter(std::string("far_percent")).c_str(),
	NULL);
	mFarPercent = (value >= 0.0 ? value - floor(value) : ceil(value) - value);
	if (mNearPercent > mFarPercent)
	{
		mFarPercent = mNearPercent;
	}
	//block size
	valueInt = strtol(mTmpl->parameter(std::string("block_size")).c_str(),
	NULL, 0);
	mBlockSize = (valueInt >= 0.0 ? valueInt : -valueInt);
	//brute force
	mBruteForce = (
			mTmpl->parameter(std::string("brute_force"))
					== std::string("false") ? false : true);
	//auto flatten
	std::string autoFlatten = mTmpl->parameter(std::string("auto_flatten"));
	if (autoFlatten == std::string("AFM_off"))
	{
		mFlattenMode = GeoMipTerrain::AFM_off;
	}
	else if (autoFlatten == std::string("AFM_light"))
	{
		mFlattenMode = GeoMipTerrain::AFM_light;
	}
	else if (autoFlatten == std::string("AFM_strong"))
	{
		mFlattenMode = GeoMipTerrain::AFM_strong;
	}
	else
	{
		mFlattenMode = GeoMipTerrain::AFM_medium;
	}
	//focal point
	mFocalPointObject = ObjectId(mTmpl->parameter(std::string("focal_point")));
	//minimum level
	valueInt = strtol(mTmpl->parameter(std::string("minimum_level")).c_str(),
			NULL, 0);
	mMinimumLevel = (valueInt >= 0.0 ? valueInt : -valueInt);
	//heightfield file
	mHeightField = PNMImage(
			Filename(mTmpl->parameter(std::string("heightfield_file"))));
	//texture
	mTextureImage = TexturePool::load_texture(
			Filename(mTmpl->parameter(std::string("texture_file"))));
	//texture uscale
	value = strtof(mTmpl->parameter(std::string("texture_uscale")).c_str(),
			NULL);
	mTextureUscale = (value >= 0.0 ? value : -value);
	//texture vscale
	value = strtof(mTmpl->parameter(std::string("texture_vscale")).c_str(),
			NULL);
	mTextureVscale = (value >= 0.0 ? value : -value);
	//
	return result;
}

void Terrain::onAddToObjectSetup()
{
	//create the current terrain
	//terrain definition
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
	mTerrain = new GeoMipTerrainRef(name);
	//set height field
	if (not mTerrain->set_heightfield(mHeightField))
	{
		//heightField the image is set to an
		//empty (black) image with 512x512 dimensions.
		mTerrain->set_heightfield(PNMImage(512, 512));
	}
	//sizing
	float environmentWidthX = (mHeightField.get_x_size() - 1) * mWidthScale;
	float environmentWidthY = (mHeightField.get_y_size() - 1) * mWidthScale;
	float environmentWidth = (environmentWidthX + environmentWidthY) / 2.0;
	//set terrain properties effectively
	mTerrain->set_block_size(mBlockSize);
	mTerrain->set_near(mNearPercent * environmentWidth);
	mTerrain->set_far(mFarPercent * environmentWidth);
	//other properties
	float terrainLODmin = min<float>(mMinimumLevel, mTerrain->get_max_level());
	mTerrain->set_min_level(terrainLODmin);
	mTerrain->set_auto_flatten(mFlattenMode);
	mTerrain->set_bruteforce(mBruteForce);
	if (mDoScale)
	{
		mTerrain->get_root().set_sx(mWidthScale);
		mTerrain->get_root().set_sy(mWidthScale);
		mTerrain->get_root().set_sz(mHeightScale);
	}
	//terrain texturing
	SMARTPTR(TextureStage) textureStage0 =
			new TextureStage(COMPONENT_STANDARD_NAME + "_TextureStage0");
	mTerrain->get_root().set_tex_scale(textureStage0,
			mTextureUscale, mTextureVscale);
	if (mTextureImage != NULL)
	{
		mTerrain->get_root().set_texture(textureStage0,	mTextureImage, 1);
	}

	//set the object node path to this terrain of node path
	mOldObjectNodePath = mOwnerObject->getNodePath();
	mOwnerObject->setNodePath(mTerrain->get_root());

	//set the focal point
	SMARTPTR(Object)createdObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mFocalPointObject);
	if (createdObject == NULL)
	{
		//set render as focal point
		createdObject =
				ObjectTemplateManager::GetSingleton().getCreatedObject(
				"render");
	}
	mFocalPointNP = createdObject->getNodePath();
	//Generate the terrain
	mTerrain->generate();
}

void Terrain::onRemoveFromObjectCleanup()
{
	//set the object node path to the old one
	mOwnerObject->setNodePath(mOldObjectNodePath);
	//
	reset();
}

void Terrain::onAddToSceneSetup()
{
	//save the net pos of terrain root
	mTerrainRootNetPos = mTerrain->get_root().get_net_transform()->get_pos();

	//Add to the scene manager update if not brute force
	if(not mBruteForce)
	{
		//Add to the scene manager update if not brute force
		GameSceneManager::GetSingletonPtr()->addToSceneUpdate(this);
	}
}

void Terrain::onRemoveFromSceneCleanup()
{
	//check if not brute force and if game scene manager exists
	if (not mBruteForce)
	{
		//remove from the scene manager update
		GameSceneManager::GetSingletonPtr()->removeFromSceneUpdate(this);
	}
}

void Terrain::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//set focal point
	///see https://www.panda3d.org/forums/viewtopic.php?t=5384
	LPoint3 focalPointNetPos = mFocalPointNP.get_net_transform()->get_pos();
	mTerrain->set_focal_point(focalPointNetPos - mTerrainRootNetPos);
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

} // namespace ely
