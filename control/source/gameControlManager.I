/**
 * \file gameControlManager.I
 *
 * \date 2016-09-18
 * \author consultit
 */

#ifndef GAMECONTROLMANGER_I_
#define GAMECONTROLMANGER_I_

///GameControlManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath GameControlManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void GameControlManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of OSDrivers.
 */
INLINE int GameControlManager::get_num_drivers() const
{
	return (int) mDrivers.size();
}

/**
 * Returns the number of OSChasers.
 */
INLINE int GameControlManager::get_num_chasers() const
{
	return (int) mChasers.size();
}

/**
 * Get singleton pointer.
 */
INLINE GameControlManager* GameControlManager::get_global_ptr()
{
	return Singleton<GameControlManager>::GetSingletonPtr();
}

/**
 * Get the collide mask.
 */
INLINE CollideMask GameControlManager::get_collide_mask() const
{
	return mMask;
}

/**
 * Get the collision root.
 */
INLINE NodePath GameControlManager::get_collision_root() const
{
	return mRoot;
}

/**
 * Get the collision traverser.
 */
INLINE CollisionTraverser* GameControlManager::get_collision_traverser() const
{
	return mCTrav;
}

/**
 * Get the collision handler.
 */
INLINE CollisionHandlerQueue* GameControlManager::get_collision_handler() const
{
	return mCollisionHandler;
}

/**
 * Get the collision ray.
 */
INLINE CollisionRay* GameControlManager::get_collision_ray() const
{
	return mPickerRay;
}

/**
 *
 */
inline int GameControlManager::unique_ref()
{
	return ++mRef;
}

#endif /* GAMECONTROLMANGER_I_ */
