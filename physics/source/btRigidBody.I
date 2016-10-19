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
	do_check_auto_shaping();
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

//	use_shape_of() xxx

/**
 * Sets the shape radius (>=0.0).
 */
INLINE void BTRigidBody::set_shape_radius(float value)
{
	mDim1 = abs(value);
	do_check_auto_shaping();
}

/**
 * Returns the shape radius (>=0.0).
 */
INLINE float BTRigidBody::get_shape_radius() const
{
	if (mDim1 > 0)
	{
		return mDim1;
	}
	return 0.0;
}

/**
 * Sets the shape norm (length>=0.0).
 */
INLINE void BTRigidBody::set_shape_norm(const LVector3f& value)
{
	mDim1 = value.get_x();
	mDim2 = value.get_y();
	mDim3 = value.get_z();
	do_check_auto_shaping();
}

/**
 * Returns the shape norm (length>=0.0).
 */
INLINE LVector3f BTRigidBody::get_shape_norm() const
{
	return LVector3f(mDim1, mDim2, mDim3).normalized();
}

/**
 * Sets the shape d.
 */
INLINE void BTRigidBody::set_shape_d(float value)
{
	mDim4 = value;
	do_check_auto_shaping();
}

/**
 * Returns the shape d.
 */
INLINE float BTRigidBody::get_shape_d() const
{
	return mDim4;
}

/**
 * Sets the shape half (each dim>=0.0).
 */
INLINE void BTRigidBody::set_shape_half_dims(const LVecBase3f& value)
{
	mDim1 = abs(value.get_x());
	mDim2 = abs(value.get_y());
	mDim3 = abs(value.get_z());
	do_check_auto_shaping();
}

/**
 * Returns the shape half (each dim>=0.0).
 */
INLINE LVecBase3f BTRigidBody::get_shape_half_dims() const
{
	if ((mDim1 > 0.0) && (mDim2 > 0.0) && (mDim3 > 0.0))
	{
		return LVecBase3f(mDim1, mDim2, mDim3);
	}
	return LVecBase3f::zero();
}

/**
 * Sets the shape height (>=0.0).
 */
INLINE void BTRigidBody::set_shape_height(float value)
{
	mDim2 = abs(value);
	do_check_auto_shaping();
}

/**
 * Returns the shape height (>=0.0).
 */
INLINE float BTRigidBody::get_shape_height() const
{
	if (mDim2 > 0)
	{
		return mDim2;
	}
	return 0.0;
}

/**
 * Sets the shape up (>=0.0).
 */
INLINE void BTRigidBody::set_shape_up(BulletUpAxis value)
{
	mUpAxis = value;
}

/**
 * Returns the shape up (>=0.0).
 */
INLINE BulletUpAxis BTRigidBody::get_shape_up() const
{
	return mUpAxis;
}

/**
 * Sets the shape height-field file.
 */
INLINE void BTRigidBody::set_shape_heightfield_file(const string& value)
{
	mHeightfieldFile = Filename(value);
}

/**
 * Returns the shape height-field file.
 */
INLINE string BTRigidBody::get_shape_heightfield_file() const
{
	return mHeightfieldFile.get_fullpath();
}

/**
 * Sets the shape scale width and depth (each value>=0.0).
 */
INLINE void BTRigidBody::set_shape_scale_width_depth(const LVecBase2f& value)
{
	mDim3 = abs(value.get_x());
	mDim4 = abs(value.get_y());
	do_check_auto_shaping();
}

/**
 * Returns the shape scale width and depth (each value>=0.0).
 */
INLINE LVecBase2f BTRigidBody::get_shape_scale_width_depth() const
{
	if ((mDim2 > 0.0) && (mDim3 > 0.0))
	{
		return LVecBase2f(mDim2, mDim3);
	}
	return LVecBase2f::zero();
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
