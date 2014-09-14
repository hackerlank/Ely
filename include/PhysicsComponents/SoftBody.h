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
 * \file /Ely/include/PhysicsComponents/SoftBody.h
 *
 * \date 08/ott/20132 (08:00:35)
 * \author consultit
 */

#ifndef SOFTBODY_H_
#define SOFTBODY_H_

#include "PhysicsComponents/BulletLocal/bulletRigidBodyNode.h"
#include "PhysicsComponents/BulletLocal/bulletSoftBodyNode.h"
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

namespace ely
{
class SoftBodyTemplate;

/**
 * \brief Component representing a single soft body attached to an object.
 *
 * It constructs a soft body with basic parameters.\n
 * Others settings could be made in programs by directly referring to the
 * underlying BulletSoftBodyNode.\n
 * \note rope is visualized as nurbs curse; patch is visualized as GeomNode.\n
 *
 * Soft body types are:
 * - *rope*
 * - *patch*
 * - *ellipsoid*
 * - *tri(angle) mesh*
 * - *tetra(hedron) mesh*
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *body_type*				|single| *rope* | values: rope,patch,ellipsoid,tri_mesh,tetra_mesh
 * | *collide_mask*				|single| *all_on* | -
 * | *body_total_mass*			|single| 1.0 | -
 * | *body_mass_from_faces*		|single| *false* | -
 * | *air_density*				|single| 1.2 | -
 * | *water_density*  			|single| 0.0 | -
 * | *water_offset*  			|single| 0.0 | -
 * | *water_normal*  			|single| 0.0,0.0,0.0 | -
 * | *show_model*				|single| *false* | for patch,ellipsoid,tetra_mesh
 * | *points*  					|single| - | for rope,patch,ellipsoid specified as "x1,y1,z1[:x2,y2,z2:...:xN,yN,zN]" with N=1..4
 * | *res*  					|single| - | for rope,patch,ellipsoid specified as "res1[:res2]"
 * | *fixeds*  					|single| - | for rope,patch
 * | *gendiags*  				|single| *true* | for patch
 * | *radius*					|single| 1.0,1.0,1.0 | for ellipsoid
 * | *randomize_constraints*	|single| *true* | for tri_mesh
 * | *tetra_data_files*			|single| - | for tetra_mesh specified as "elems,faces,nodes"
 *
 * \note parts inside [] are optional.\n
 */
class SoftBody: public Component
{
protected:
	friend class SoftBodyTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	SoftBody();
	SoftBody(SMARTPTR(SoftBodyTemplate)tmpl);
	virtual ~SoftBody();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief The current component's type.
	 *
	 * It may change during the component's lifetime.
	 */
	enum BodyType
	{
		ROPE,
		PATCH,
		ELLIPSOID,
		TRIMESH,
		TETRAMESH
	};

	/**
	 * \brief Gets/sets the node path of this soft body.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \name BulletSoftBodyNode reference getter & conversion function.
	 */
	///@{
	BulletSoftBodyNode& getBulletSoftBodyNode();
	operator BulletSoftBodyNode&();
	///@}

private:
	///The NodePath associated to this soft body.
	NodePath mNodePath;
	///The underlying BulletSoftBodyNode (read-only after creation & before destruction).
	SMARTPTR(BulletSoftBodyNode) mSoftBodyNode;

	///@{
	///Physical parameters.
	BodyType mBodyType;
	BitMask32 mCollideMask;
	float mBodyTotalMass, mAirDensity, mWaterDensity, mWaterOffset;
	bool mBodyMassFromFaces;
	LVector3f mWaterNormal;
	///@}

	///@{
	///Geometric, structural and other parameters.
	bool mShowModel;
	std::vector<LPoint3f> mPoints;
	std::vector<int> mRes;
	int mFixeds;
	bool mGendiags;
	LVecBase3f mRadius;
	bool mRandomizeConstraints;
	std::map<std::string, std::string> mTetraDataFileNames;
	///@}

	///HACK: rope node's parent node path correction (see bullet samples).
	NodePath mRopeNodePath;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "SoftBody", Component::get_class_type());
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

inline void SoftBody::reset()
{
	//
	mNodePath = NodePath();
	mSoftBodyNode.clear();
	mBodyType = ROPE;
	mCollideMask = BitMask32::all_off();
	mBodyTotalMass = mAirDensity = mWaterDensity = mWaterOffset = 0.0;
	mBodyMassFromFaces = false;
	mWaterNormal = LVector3f::zero();
	mShowModel = false;
	mPoints.clear();
	mRes.clear();
	mFixeds = 0;
	mGendiags = true;
	mRadius = LVecBase3f::zero();
	mRandomizeConstraints = true;
	mTetraDataFileNames.clear();
	mRopeNodePath = NodePath();
}

inline NodePath SoftBody::getNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mNodePath;
}

inline void SoftBody::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mNodePath = nodePath;
}

inline BulletSoftBodyNode& SoftBody::getBulletSoftBodyNode()
{
	return *mSoftBodyNode;
}

inline SoftBody::operator BulletSoftBodyNode&()
{
	return *mSoftBodyNode;
}

} /* namespace ely */
#endif /* SOFTBODY_H_ */
