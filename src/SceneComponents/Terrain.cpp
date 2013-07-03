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

namespace ely
{

Terrain::Terrain()
{
	// TODO Auto-generated constructor stub
}

Terrain::Terrain(SMARTPTR(TerrainTemplate)tmpl)
{
	CHECKEXISTENCE(GameSceneManager::GetSingletonPtr(),
			"Terrain::Terrain: invalid GameSceneManager")
	mTmpl = tmpl;
}

Terrain::~Terrain()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//check if game physics manager exists
	if (GameSceneManager::GetSingletonPtr())
	{
		//remove from the scene manager update
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
	return mTmpl->componentType();
}

bool Terrain::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//get scale
	mHeightScale = strtof(
			mTmpl->parameter(std::string("height_scale")).c_str(), NULL);
	if (mHeightScale <= 0.0)
	{
		mHeightScale = 1.0;
	}
	mWidthScale = strtof(
			mTmpl->parameter(std::string("width_scale")).c_str(), NULL);
	if (mWidthScale <= 0.0)
	{
		mWidthScale = 1.0;
	}
	mDoScale = (
			mTmpl->parameter(std::string("do_scale"))
					== std::string("false") ? false : true);
	//get LOD
	mNearPercent = strtof(
			mTmpl->parameter(std::string("near_percent")).c_str(), NULL);
	mFarPercent = strtof(
			mTmpl->parameter(std::string("far_percent")).c_str(), NULL);
	if ((mNearPercent <= 0.0) or (mFarPercent >= 1.0)
			or (mNearPercent >= mFarPercent))
	{
		mFarPercent = 1.0;
		mNearPercent = 0.1;
	}
	//get block size
	mBlockSize = strtol(mTmpl->parameter(std::string("block_size")).c_str(),
			NULL, 0);
	if (mBlockSize <= 0)
	{
		mBlockSize = 64;
	}
	//get brute force
	mBruteForce = (
			mTmpl->parameter(std::string("brute_force"))
					== std::string("false") ? false : true);
	//get auto flatten
	std::string autoFlatten = mTmpl->parameter(std::string("auto_flatten"));
	if (autoFlatten == "AFM_off")
	{
		mFlattenMode = GeoMipTerrain::AFM_off;
	}
	else if (autoFlatten == "AFM_light")
	{
		mFlattenMode = GeoMipTerrain::AFM_light;
	}
	else if (autoFlatten == "AFM_strong")
	{
		mFlattenMode = GeoMipTerrain::AFM_strong;
	}
	else
	{
		mFlattenMode = GeoMipTerrain::AFM_medium;
	}
	//get focal point
	mFocalPointObject = ObjectId(mTmpl->parameter(std::string("focal_point")));
	//get minimum level
	mMinimumLevel = strtol(
			mTmpl->parameter(std::string("minimum_level")).c_str(), NULL, 0);
	if (mMinimumLevel <= 0)
	{
		mMinimumLevel = 0;
	}
	//get heightfield image
	mHeightField = PNMImage(
			Filename(mTmpl->parameter(std::string("heightfield_file"))));
	//get texture scale
	mTextureUscale = strtof(
			mTmpl->parameter(std::string("texture_uscale")).c_str(), NULL);
	if (mTextureUscale <= 0.0)
	{
		mTextureUscale = 1.0;
	}
	mTextureVscale = strtof(
			mTmpl->parameter(std::string("texture_vscale")).c_str(), NULL);
	if (mTextureVscale <= 0.0)
	{
		mTextureVscale = 1.0;
	}
	//get texture
	mTextureImage = TexturePool::load_texture(
			Filename(mTmpl->parameter(std::string("texture_file"))));
	//
	return result;
}

void Terrain::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
	mTerrain->get_root().set_tex_scale(TextureStage::get_default(),
			mTextureUscale, mTextureVscale);
	if (mTextureImage != NULL)
	{
		mTerrain->get_root().set_texture(TextureStage::get_default(),
				mTextureImage, 1);
	}

	//set the node path of the object to the
	//node path of this model
	mOwnerObject->setNodePath(mTerrain->get_root());
	//set the focal point
	SMARTPTR(Object)createdObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mFocalPointObject);
	if (createdObject == NULL)
	{
		//set render as focal point
		createdObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
				"render");
	}
	mFocalPointNP = createdObject->getNodePath();
	//Generate the terrain
	mTerrain->generate();
	//Add to the scene manager update if not brute force
	GameSceneManager::GetSingletonPtr()->addToSceneUpdate(this);

	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void Terrain::onAddToSceneSetup()
{
	//save the net pos of terrain root
	mTerrainRootNetPos = mTerrain->get_root().get_net_transform()->get_pos();
}

void Terrain::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

//	float dt = *(reinterpret_cast<float*>(data));

//#ifdef TESTING
//	dt = 0.016666667; //60 fps
//#endif

	//set focal point
	///see https://www.panda3d.org/forums/viewtopic.php?t=5384
	LPoint3 focalPointNetPos = mFocalPointNP.get_net_transform()->get_pos();
	mTerrain->set_focal_point(focalPointNetPos - mTerrainRootNetPos);
	//update every frame
	mTerrain->update();
}

SMARTPTR(GeoMipTerrainRef)Terrain::getGeoMipTerrain() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mTerrain;
}

float Terrain::getWidthScale() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mWidthScale;
}
float Terrain::getHeightScale() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mHeightScale;
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
