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

/**
 * Sets the shape type (>=0.0).
 */
INLINE void BTRigidBody::set_shape_type(GamePhysicsManager::ShapeType value)
{
	mShapeType = value;
}

/**
 * Returns the shape type (>=0.0).
 */
INLINE GamePhysicsManager::ShapeType BTRigidBody::get_shape_type() const
{
	return mShapeType;
}

/**
 * Sets the shape size (>=0.0).
 */
INLINE void BTRigidBody::set_shape_size(GamePhysicsManager::ShapeSize value)
{
	mShapeSize = value;
}

/**
 * Returns the shape type (>=0.0).
 */
INLINE GamePhysicsManager::ShapeSize BTRigidBody::get_shape_size() const
{
	return mShapeSize;
}

//	use_shape_of() XXX ?

/**
 * Sets the shape radius (>=0.0).
 */
INLINE void BTRigidBody::set_shape_radius(float value)
{
	mDim1 = value;
}

/**
 * Returns the shape radius (>=0.0).
 */
INLINE float BTRigidBody::get_shape_radius() const
{
	return mDim1;
}

/**
 * Sets the shape norm (>=0.0).
 */
INLINE void BTRigidBody::set_shape_norm(const LVector3f& value)
{
}

/**
 * Returns the shape norm (>=0.0).
 */
INLINE LVector3f BTRigidBody::get_shape_norm() const
{
}

/**
 * Sets the shape d (>=0.0).
 */
INLINE void BTRigidBody::set_shape_d(float value)
{
}

/**
 * Returns the shape d (>=0.0).
 */
INLINE float BTRigidBody::get_shape_d() const
{
}

/**
 * Sets the shape half (>=0.0).
 */
INLINE void BTRigidBody::set_shape_half(const LVecBase3f& value)
{
}

/**
 * Returns the shape half (>=0.0).
 */
INLINE LVecBase3f BTRigidBody::get_shape_half() const
{
}

/**
 * Sets the shape height (>=0.0).
 */
INLINE void BTRigidBody::set_shape_height(float value)
{
}

/**
 * Returns the shape height (>=0.0).
 */
INLINE float BTRigidBody::get_shape_height() const
{
}

/**
 * Sets the shape up (>=0.0).
 */
INLINE void BTRigidBody::set_shape_up(BulletUpAxis value)
{
}

/**
 * Returns the shape up (>=0.0).
 */
INLINE BulletUpAxis BTRigidBody::get_shape_up() const
{
}

/**
 * Sets the shape heightfield_file (>=0.0).
 */
INLINE void BTRigidBody::set_shape_heightfield_file(const string& value)
{
}

/**
 * Returns the shape heightfield_file (>=0.0).
 */
INLINE string BTRigidBody::get_shape_heightfield_file() const
{
}

/**
 * Sets the shape scale (>=0.0).
 */
INLINE void BTRigidBody::set_shape_scale(const LVecBase3f& value)
{
}

/**
 * Returns the shape scale (>=0.0).
 */
INLINE LVecBase3f BTRigidBody::get_shape_scale() const
{
}

/**
 * Resets the BTRigidBody.
 * \note Internal use only.
 */
inline void BTRigidBody::do_reset()
{
	//
	mSetup = false;
	mBodyType = DYNAMIC;
	mShapeType = GamePhysicsManager::SPHERE;
	mShapeSize = GamePhysicsManager::MEDIUM;
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
