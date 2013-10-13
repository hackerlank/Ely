/*
 * SoftBody.h
 *
 *  Created on: 08/ott/2013
 *      Author: marco
 */

#ifndef SOFTBODY_H_
#define SOFTBODY_H_

#include <bulletSoftBodyNode.h>
#include "ObjectModel/Component.h"

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
 * - "point_1"  				|single|no default (for rope,patch,ellipsoid)
 * - "point_2"  				|single|no default (for rope,patch)
 * - "point_3"  				|single|no default (for patch)
 * - "point_4"  				|single|no default (for patch)
 * - "res_1"  					|single|no default (for rope,patch)
 * - "res_2"  					|single|no default (for patch)
 * - "fixeds"  					|single|no default (for rope,patch)
 * - "num_thickness"  			|single|"0.4" (for rope)
 * - "num_slices"  				|single|"8" (for rope)
 * - "num_subdiv"  				|single|"4" (for rope)
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
	float mBodyTotalMass, mBodyFriction, mBodyRestitution;
	BodyType mBodyType;
	GamePhysicsManager::ShapeType mShapeType;
	GamePhysicsManager::ShapeSize mShapeSize;
	BitMask32 mCollideMask;
	//ccd stuff
	float mCcdMotionThreshold, mCcdSweptSphereRadius;
	bool mCcdEnabled;

 * - "body_total_mass"  		|single|"1.0"
 * - "collide_mask"  			|single|"all_on"
 * - "air_density"  			|single|"1.2"
 * - "water_density"  			|single|"0.0"
 * - "water_normal"  			|single|"0.0,0.0,0.0"
 * - "water_offset"  			|single|"0.0"
 * - "anchor_objects"			|multiple|no default (each one specified as
 * "objectId1[:objectId2:...:objectIdN]")
 * - "point_1"  				|single|no default (for rope,patch,ellipsoid)
 * - "point_2"  				|single|no default (for rope,patch)
 * - "point_3"  				|single|no default (for patch)
 * - "point_4"  				|single|no default (for patch)
 * - "res_1"  					|single|no default (for rope,patch)
 * - "res_2"  					|single|no default (for patch)
 * - "fixeds"  					|single|no default (for rope,patch)
 * - "num_thickness"  			|single|"0.4" (for rope)
 * - "num_slices"  				|single|"8" (for rope)
 * - "num_subdiv"  				|single|"4" (for rope)
 * - "num_subdiv"  				|single|"4" (for rope)
 * - "texture_file"				|single|no default

	/**
	 * \brief Sets physical parameters of a bullet soft body node (helper function).
	 */
	void doSetPhysicalParameters();
	///@}

	/**
	 * \brief Sets body type.
	 * @param bodyType The body type.
	 */
	void doSwitchBodyType(BodyType bodyType);

	///Geometric functions and parameters.
	///@{
	/**
	 * \brief Create a shape given its type.
	 * @param shapeType The shape type.
	 * @return The created shape.
	 */
	SMARTPTR(BulletShape) doCreateShape(GamePhysicsManager::ShapeType shapeType);
	LVector3 mModelDims;
	float mModelRadius;
	//use shape of (another object).
	ObjectId mUseShapeOfId;
	//any model has a local frame and the tight bounding box is computed
	//wrt it; so mModelDeltaCenter represents a transform (translation) to
	//be applied to the model node path so that the middle point of the
	//bounding box will overlap the frame center of the parent's node path .
	LVector3 mModelDeltaCenter;
	bool mAutomaticShaping;
	float mDim1, mDim2, mDim3, mDim4;
	Filename mHeightfieldFile;
	BulletUpAxis mUpAxis;
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
	mBodyTotalMass = mBodyFriction = mBodyRestitution = 0.0;
	mBodyType = ROPE;
	mShapeType = GamePhysicsManager::SPHERE;
	mShapeSize = GamePhysicsManager::MEDIUM;
	mCollideMask = BitMask32::all_off();
	mCcdMotionThreshold = mCcdSweptSphereRadius = 0.0;
	mCcdEnabled = false;
	mModelDims = LVector3::zero();
	mModelRadius = 0.0;
	mUseShapeOfId = ObjectId();
	mModelDeltaCenter = LVector3::zero();
	mAutomaticShaping = false;
	mDim1 = mDim2 = mDim3 = mDim4 = 0.0;
	mHeightfieldFile = Filename();
	mUpAxis = Z_up;
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
