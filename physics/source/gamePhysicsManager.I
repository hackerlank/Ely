/**
 * \file gamePhysicsManager.I
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef GAMEPHYSICSMANGER_I_
#define GAMEPHYSICSMANGER_I_

///GamePhysicsManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath GamePhysicsManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void GamePhysicsManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of BTRigidBodys.
 */
INLINE int GamePhysicsManager::get_num_rigid_bodies() const
{
	return (int) mRigidBodies.size();
}

/**
 * Returns the number of BTSoftBodys.
 */
INLINE int GamePhysicsManager::get_num_soft_bodies() const
{
	return (int) mSoftBodies.size();
}

/**
 * Returns the number of BTGhosts.
 */
INLINE int GamePhysicsManager::get_num_ghosts() const
{
	return (int) mGhosts.size();
}

/**
 * Returns the singleton pointer.
 */
INLINE GamePhysicsManager* GamePhysicsManager::get_global_ptr()
{
	return Singleton<GamePhysicsManager>::GetSingletonPtr();
}

/**
 * Returns the collide mask.
 */
INLINE CollideMask GamePhysicsManager::get_collide_mask() const
{
	return mMask;
}

/**
 * Returns the collision root.
 */
INLINE NodePath GamePhysicsManager::get_collision_root() const
{
	return mRoot;
}

/**
 * Returns the collision traverser.
 */
INLINE CollisionTraverser* GamePhysicsManager::get_collision_traverser() const
{
	return mCTrav;
}

/**
 * Returns the collision handler.
 */
INLINE CollisionHandlerQueue* GamePhysicsManager::get_collision_handler() const
{
	return mCollisionHandler;
}

/**
 * Returns the collision ray.
 */
INLINE CollisionRay* GamePhysicsManager::get_collision_ray() const
{
	return mPickerRay;
}

/**
 *
 */
inline int GamePhysicsManager::unique_ref()
{
	return ++mRef;
}

#endif /* GAMEPHYSICSMANGER_I_ */
