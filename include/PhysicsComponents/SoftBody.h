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
 * It constructs a soft body with the single specified collision shape_type
 * along with relevant parameters.\n
 * Collision shapes are:
 * - "sphere"
 * - "plane"
 * - "box"
 * - "cylinder"
 * - "capsule"
 * - "cone"
 * - "heightfield"
 * In case of "sphere", "box", "cylinder", "capsule", "cone", if any of
 * the relevant parameters is missing, the shape is automatically
 * constructed by guessing them through calculation of a tight bounding volume
 * of object geometry (supposedly specified by the model component).\n
 * For "plane" shape, in case of missing parameters, the default is
 * a plane with normal = (0,0,1) and d = 0.
 *
 * XML Param(s):
 * - "body_type"  				|single|"dynamic" ("static","kinematic")
 * - "body_mass"  				|single|"1.0"
 * - "body_friction"  			|single|"0.8"
 * - "body_restitution"  		|single|"0.1"
 * - "collide_mask"  			|single|"all_on"
 * - "shape_type"  				|single|"sphere"
 * - "shape_size"  				|single|"medium"  (minimum, medium, maximum)
 * - "use_shape_of"				|single|no default
 * - "shape_radius"  			|single|no default (for sphere,cylinder,capsule,cone)
 * - "shape_norm_x"  			|single|no default (for plane)
 * - "shape_norm_y"  			|single|no default (for plane)
 * - "shape_norm_z"  			|single|no default (for plane)
 * - "shape_d"  				|single|no default (for plane)
 * - "shape_half_x"  			|single|no default (for box)
 * - "shape_half_y"  			|single|no default (for box)
 * - "shape_half_z"  			|single|no default (for box)
 * - "shape_height"  			|single|"1.0" (for cylinder,capsule,cone,heightfield)
 * - "shape_up"  				|single|"z" ((x,y,z) for cylinder,capsule,cone,heightfield)
 * - "shape_heightfield_file" 	|single|no default (heightfield)
 * - "shape_scale_w"  			|single|"1.0" (heightfield)
 * - "shape_scale_d"  			|single|"1.0" (heightfield)
 * - "ccd_motion_threshold"  	|single|no default
 * - "ccd_swept_sphere_radius" 	|single|no default
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
		TRIANGLES,
		TETRAHEDRON
	};

	/**
	 * \brief Switches the current component's type.
	 *
	 * It sets the rigid body mass too.
	 * @param bodyType The new component's type.
	 */
	void switchType(BodyType bodyType);

	/**
	 * \brief Gets/sets the node path of this rigid body.
	 * @return The node path of this rigid body.
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
	///The NodePath associated to this rigid body.
	NodePath mNodePath;
	///The underlying BulletSoftBodyNode (read-only after creation & before destruction).
	SMARTPTR(BulletSoftBodyNode) mSoftBodyNode;
	///@{
	///Physical parameters.
	float mBodyMass, mBodyFriction, mBodyRestitution;
	BodyType mBodyType;
	GamePhysicsManager::ShapeType mShapeType;
	GamePhysicsManager::ShapeSize mShapeSize;
	BitMask32 mCollideMask;
	//ccd stuff
	float mCcdMotionThreshold, mCcdSweptSphereRadius;
	bool mCcdEnabled;
	/**
	 * \brief Sets physical parameters of a bullet rigid body node (helper function).
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
	mBodyMass = mBodyFriction = mBodyRestitution = 0.0;
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
