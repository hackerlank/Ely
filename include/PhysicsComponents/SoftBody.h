/*
 * SoftBody.h
 *
 *  Created on: 08/ott/2013
 *      Author: marco
 */

#ifndef SOFTBODY_H_
#define SOFTBODY_H_

#include <bulletRigidBodyNode.h>
#include <bulletSoftBodyNode.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

namespace ely
{
class SoftBodyTemplate;

/**
 * \brief Component representing a single soft body attached to an object.
 *
 * It constructs a soft body with relevant parameters. Not all the
 * parameters can be specified on creation. Others settings
 * could be made in programs by directly referring to the underlying
 * BulletSoftBodyNode.\n
 * \note rope is visualized as nurbs curse; patch is visualized as GeomNode.\n
 * Soft body types are:
 * - "rope"
 * - "patch"
 * - "ellipsoid"
 * - "triangles"
 * - "tetrahedron"
 *
 * XML Param(s):
 * - "body_type"  				|single|"rope" (values: rope|patch|
 * ellipsoid|tri_mesh|tet_mesh)
 * - "body_total_mass"  		|single|"1.0"
 * - "collide_mask"  			|single|"all_on"
 * - "air_density"  			|single|"1.2"
 * - "water_density"  			|single|"0.0"
 * - "water_normal"  			|single|"0.0,0.0,0.0"
 * - "water_offset"  			|single|"0.0"
 * - "anchor_objects"			|multiple|no default (each one specified as
 * "objectId1[:objectId2:...:objectIdN]")
 * - "points"  					|single|no default (for rope,patch,ellipsoid
 * specified as "x1,y1,z1[:x2,y2,z2:...:xN,yN,zN]" with N=1..4)
 * - "resolutions"  			|single|no default (for rope,patch specified
 * as "resolution1[:resolution2]")
 * - "fixeds"  					|single|no default (for rope,patch)
 * - "thickness"  				|single|"0.4" (for rope)
 * - "num_slices"  				|single|"8" (for rope)
 * - "num_subdiv"  				|single|"4" (for rope)
 * - "texture_file"				|single|no default
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
		TETMESH
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
	float mBodyTotalMass, air_density, water_density, water_offset;
	LVector3f water_normal;
	BitMask32 mCollideMask;
	///@}

	///@{
	///Geometric and structural parameters.
	std::vector<ObjectId> anchor_objects;
	std::vector<LPoint3f> points;
	std::vector<int> resolutions;
	int fixeds, num_slices, num_subdiv;
	float thickness;
	SMARTPTR(Texture)mTextureImage;
	///@}

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
	mBodyTotalMass = air_density = water_density = water_offset = 0.0;
	water_normal = LVector3::zero();
	mCollideMask = BitMask32::all_off();
	anchor_objects.clear();
	points.clear();
	resolutions.clear();
	fixeds = num_slices = num_subdiv = 0;
	thickness = 0.0;
	mTextureImage.clear();
}

inline void SoftBody::onRemoveFromSceneCleanup()
{
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
