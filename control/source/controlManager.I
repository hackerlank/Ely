/**
 * \file controlManager.I
 *
 * \date 2016-09-18
 * \author consultit
 */

#ifndef CONTROLMANGER_I_
#define CONTROLMANGER_I_

///ControlManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath ControlManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void ControlManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of OSDrivers.
 */
INLINE int ControlManager::get_num_drivers() const
{
	return (int) mDrivers.size();
}

/**
 * Returns the number of OSChasers.
 */
INLINE int ControlManager::get_num_chasers() const
{
	return (int) mChasers.size();
}

/**
 * Get singleton pointer.
 */
INLINE ControlManager* ControlManager::get_global_ptr()
{
	return Singleton<ControlManager>::GetSingletonPtr();
}

/**
 * Get the collide mask.
 */
INLINE CollideMask ControlManager::get_collide_mask() const
{
	return mMask;
}

/**
 * Get the collision root.
 */
INLINE NodePath ControlManager::get_collision_root() const
{
	return mRoot;
}

/**
 * Get the collision traverser.
 */
INLINE CollisionTraverser* ControlManager::get_collision_traverser() const
{
	return mCTrav;
}

/**
 * Get the collision handler.
 */
INLINE CollisionHandlerQueue* ControlManager::get_collision_handler() const
{
	return mCollisionHandler;
}

/**
 * Get the collision ray.
 */
INLINE CollisionRay* ControlManager::get_collision_ray() const
{
	return mPickerRay;
}

/**
 *
 */
inline int ControlManager::unique_ref()
{
	return ++mRef;
}

#endif /* CONTROLMANGER_I_ */
