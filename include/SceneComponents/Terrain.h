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
 * \file /Ely/include/SceneComponents/Terrain.h
 *
 * \date 15/ago/2012 (10:00:07)
 * \author consultit
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <geoMipTerrain.h>
#include <pnmImage.h>
#include <texture.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

namespace ely
{
class TerrainTemplate;

class GeoMipTerrainRef;

/**
 * \brief Component representing the terrain model of an object.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *heightfield_file*  		|single| - | -
 * | *height_scale*				|single| 1.0 | -
 * | *width_scale*				|single| 1.0 | -
 * | *do_scale*					|single| *true* | hack for rigid body
 * | *block_size*				|single| 64 | -
 * | *near_percent*				|single| 0.1 | -
 * | *far_percent*				|single| 1.0 | -
 * | *brute_force*				|single| *true* | -
 * | *auto_flatten*				|single| *AFM_medium* | -
 * | *focal_point*				|single| *camera* | -
 * | *minimum_level*			|single| 0 | -
 * | *texture_file*				|single| - | -
 * | *texture_uscale*			|single| 1.0 | -
 * | *texture_vscale*			|single| 1.0 | -
 *
 * \note parts inside [] are optional.\n
 */
class Terrain: public Component
{
protected:
	friend class TerrainTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Terrain();
	Terrain(SMARTPTR(TerrainTemplate)tmpl);
	virtual ~Terrain();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates the terrain associated to this component.
	 *
	 * Will be called automatically by an scene manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \brief Gets width and height scale.
	 */
	///@{
	float getWidthScale() const;
	float getHeightScale() const;
	///@}

	/**
	 * \name GeoMipTerrain reference getter & conversion function.
	 */
	///@{
	GeoMipTerrainRef& getGeoMipTerrain();
	operator GeoMipTerrainRef&();
	///@}

private:
	///The underlying GeoMipTerrain (read-only after creation & before destruction).
	SMARTPTR(GeoMipTerrainRef) mTerrain;

	/**
	 * \name Main parameters.
	 */
	///@{
	///Heightfield image.
	PNMImage mHeightField;
	///Scale.
	float mHeightScale, mWidthScale;
	bool mDoScale;
	///LOD.
	float mNearPercent, mFarPercent;
	///Block size.
	int mBlockSize;
	///Auto flatten.
	GeoMipTerrain::AutoFlattenMode mFlattenMode;
	///Minimum level.
	int mMinimumLevel;
	///Texture.
	SMARTPTR(Texture)mTextureImage;
	///Texture scale.
	float mTextureUscale, mTextureVscale;
	///@}

	/**
	 * \name Focal point stuff.
	 */
	///@{
	///The object to be focal point .
	ObjectId mFocalPointObject;
	///The focal point NodePath.
	///see https://www.panda3d.org/forums/viewtopic.php?t=5384
	NodePath mFocalPointNP;
	LPoint3f mTerrainRootNetPos;
	///@}
	///Flag if brute force is enabled.
	bool mBruteForce;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Terrain", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

/**
 * \brief GeoMipTerrain class with possibility to be reference counted.
 */
class GeoMipTerrainRef: public GeoMipTerrain, public ReferenceCount
{
public:
	GeoMipTerrainRef(const std::string& name);

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		ReferenceCount::init_type();
		register_type(_type_handle, "GeoMipTerrainRef",
				ReferenceCount::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;
};

///inline definitions

inline void Terrain::reset()
{
	//
	mTerrain.clear();
	mHeightField.clear();
	mHeightScale = mWidthScale = 1.0;
	mDoScale = false;
	mNearPercent = 0.0;
	mFarPercent = 1.0;
	mBlockSize = 0;
	mFlattenMode = GeoMipTerrain::AFM_off;
	mMinimumLevel = 0;
	mTextureImage.clear();
	mTextureUscale = mTextureVscale = 1.0;
	mFocalPointObject = ObjectId();
	mFocalPointNP = NodePath();
	mTerrainRootNetPos = LPoint3f::zero();
	mBruteForce = false;
}

inline float Terrain::getWidthScale() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mWidthScale;
}
inline float Terrain::getHeightScale() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeightScale;
}

inline GeoMipTerrainRef& Terrain::getGeoMipTerrain()
{
	return *mTerrain;
}

inline Terrain::operator GeoMipTerrainRef&()
{
	return *mTerrain;
}

}  // namespace ely

#endif /* TERRAIN_H_ */
