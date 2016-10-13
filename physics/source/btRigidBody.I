/**
 * \file btRigidBody.I
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef BTRIGIDBODY_I_
#define BTRIGIDBODY_I_

///BTRigidBody inline definitions

/**
 * Sets the reference NodePath of this BTRigidBody.
 */
INLINE void BTRigidBody::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

INLINE NodePath BTRigidBody::getNodePath() const
{
	return mNodePath;
}

INLINE void BTRigidBody::setNodePath(const NodePath& nodePath)
{
	mNodePath = nodePath;
}

/**
 * Sets the body mass (>=0.0).
 */
INLINE void set_body_mass(float value);

/**
 * Returns the body mass (>=0.0).
 */
/**
 * Returns the body_mass (>=0.0).
 */
INLINE float BTRigidBody::get_body_mass() const
{
}

/**
 * Sets the body_friction (>=0.0).
 */
INLINE void BTRigidBody::set_body_friction(float value)
{
}

/**
 * Returns the body_friction (>=0.0).
 */
INLINE float BTRigidBody::get_body_friction() const
{
}

/**
 * Sets the body_restitution (>=0.0).
 */
INLINE void BTRigidBody::set_body_restitution(float value)
{
}

/**
 * Returns the body_restitution (>=0.0).
 */
INLINE float BTRigidBody::get_body_restitution() const
{
}

/**
 * Sets the collide_mask (>=0.0).
 */
INLINE void BTRigidBody::set_collide_mask(CollideMask value)
{
}

/**
 * Returns the collide_mask (>=0.0).
 */
INLINE CollideMask BTRigidBody::get_collide_mask() const
{
}

/**
 * Sets the shape_type (>=0.0).
 */
INLINE void BTRigidBody::set_shape_type(GamePhysicsManager::ShapeType value)
{
}

/**
 * Returns the shape_type (>=0.0).
 */
INLINE GamePhysicsManager::ShapeType BTRigidBody::get_shape_type() const
{

}

/**
 * Sets the shape_size (>=0.0).
 */
INLINE void BTRigidBody::set_shape_size(GamePhysicsManager::ShapeSize value)
{
}

/**
 * Returns the shape_type (>=0.0).
 */
INLINE GamePhysicsManager::ShapeSize BTRigidBody::get_shape_size() const
{

}

//	use_shape_of() XXX ?

/**
 * Sets the shape_radius (>=0.0).
 */
INLINE void BTRigidBody::set_shape_radius(float value)
{
}

/**
 * Returns the shape_radius (>=0.0).
 */
INLINE float BTRigidBody::get_shape_radius() const
{
}

/**
 * Sets the shape_norm (>=0.0).
 */
INLINE void BTRigidBody::set_shape_norm(const LVector3f& value)
{
}

/**
 * Returns the shape_norm (>=0.0).
 */
INLINE LVector3f BTRigidBody::get_shape_norm() const
{
}

/**
 * Sets the shape_d (>=0.0).
 */
INLINE void BTRigidBody::set_shape_d(float value)
{
}

/**
 * Returns the shape_d (>=0.0).
 */
INLINE float BTRigidBody::get_shape_d() const
{
}

/**
 * Sets the shape_half (>=0.0).
 */
INLINE void BTRigidBody::set_shape_half(const LVecBase3f& value)
{
}

/**
 * Returns the shape_half (>=0.0).
 */
INLINE LVecBase3f BTRigidBody::get_shape_half() const
{
}

/**
 * Sets the shape_height (>=0.0).
 */
INLINE void BTRigidBody::set_shape_height(float value)
{
}

/**
 * Returns the shape_height (>=0.0).
 */
INLINE float BTRigidBody::get_shape_height() const
{
}

/**
 * Sets the shape_up (>=0.0).
 */
INLINE void BTRigidBody::set_shape_up(BulletUpAxis value)
{
}

/**
 * Returns the shape_up (>=0.0).
 */
INLINE BulletUpAxis BTRigidBody::get_shape_up() const
{
}

/**
 * Sets the shape_heightfield_file (>=0.0).
 */
INLINE void BTRigidBody::set_shape_heightfield_file(const string& value)
{
}

/**
 * Returns the shape_heightfield_file (>=0.0).
 */
INLINE string BTRigidBody::get_shape_heightfield_file() const
{
}

/**
 * Sets the shape_scale (>=0.0).
 */
INLINE void BTRigidBody::set_shape_scale(const LVecBase3f& value)
{
}

/**
 * Returns the shape_scale (>=0.0).
 */
INLINE LVecBase3f BTRigidBody::get_shape_scale() const
{
}

/**
 * Sets the ccd_motion_threshold (>=0.0).
 */
INLINE void BTRigidBody::set_ccd_motion_threshold(float value)
{
}

/**
 * Returns the ccd_motion_threshold (>=0.0).
 */
INLINE float BTRigidBody::get_ccd_motion_threshold() const
{
}

/**
 * Sets the ccd_swept_sphere_radius (>=0.0).
 */
INLINE void BTRigidBody::set_ccd_swept_sphere_radius(float value)
{
}

/**
 * Returns the ccd_swept_sphere_radius (>=0.0).
 */
INLINE float BTRigidBody::get_ccd_swept_sphere_radius() const
{
}

INLINE BulletRigidBodyNode& BTRigidBody::getBulletRigidBodyNode() const
{
	return *mRigidBodyNode;
}

inline BTRigidBody::operator BulletRigidBodyNode&() const
{
	return *mRigidBodyNode;
}

/**
 * Resets the BTRigidBody.
 * \note Internal use only.
 */
inline void BTRigidBody::do_reset()
{
	//
	mNodePath = NodePath(); // delete?
	mRigidBodyNode.clear();
	mSetup = false;
	mBodyMass = mBodyFriction = mBodyRestitution = 0.0;
	mBodyType = DYNAMIC;
	mShapeType = GamePhysicsManager::SPHERE;
	mShapeSize = GamePhysicsManager::MEDIUM;
	mCollideMask = BitMask32::all_off();
	mCcdMotionThreshold = mCcdSweptSphereRadius = 0.0;
	mCcdEnabled = false;
	mModelDims = LVector3f::zero();
	mModelRadius = 0.0;
//	mUseShapeOfId = ObjectId();xxx
	mModelDeltaCenter = LVector3f::zero();
	mAutomaticShaping = false;
	mDim1 = mDim2 = mDim3 = mDim4 = 0.0;
	mHeightfieldFile = Filename();
	mUpAxis = Z_up;
	mReferenceNP.clear();
#ifdef PYTHON_BUILD
	mSelf = NULL;
	mUpdateCallback = NULL;
	mUpdateArgList = NULL;
#else
	mUpdateCallback = NULL;
#endif //PYTHON_BUILD
}

INLINE ostream &operator <<(ostream &out, const BTRigidBody& rigid_body)
{
	rigid_body.output(out);
	return out;
}

#endif /* BTRIGIDBODY_I_ */
