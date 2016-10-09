/**
 * \file gameBehaviorManager.I
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef GAMEBEHAVIORMANGER_I_
#define GAMEBEHAVIORMANGER_I_

///GameBehaviorManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath GameBehaviorManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void GameBehaviorManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of P3Sound3ds.
 */
INLINE int GameBehaviorManager::get_num_sound3ds() const
{
	return (int) mSound3ds.size();
}

/**
 * Returns the number of P3Activitys.
 */
INLINE int GameBehaviorManager::get_num_activities() const
{
	return (int) mActivitys.size();
}

/**
 * Returns the singleton pointer.
 */
INLINE GameBehaviorManager* GameBehaviorManager::get_global_ptr()
{
	return Singleton<GameBehaviorManager>::GetSingletonPtr();
}

/**
 * Returns a reference to the BehaviorManager.
 */
INLINE PT(BehaviorManager) GameBehaviorManager::get_behavior_manager() const
{
	return mBehaviorMgr;
}

/**
 * Returns the collide mask.
 */
INLINE CollideMask GameBehaviorManager::get_collide_mask() const
{
	return mMask;
}

/**
 * Returns the collision root.
 */
INLINE NodePath GameBehaviorManager::get_collision_root() const
{
	return mRoot;
}

/**
 * Returns the collision traverser.
 */
INLINE CollisionTraverser* GameBehaviorManager::get_collision_traverser() const
{
	return mCTrav;
}

/**
 * Returns the collision handler.
 */
INLINE CollisionHandlerQueue* GameBehaviorManager::get_collision_handler() const
{
	return mCollisionHandler;
}

/**
 * Returns the collision ray.
 */
INLINE CollisionRay* GameBehaviorManager::get_collision_ray() const
{
	return mPickerRay;
}

/**
 *
 */
inline int GameBehaviorManager::unique_ref()
{
	return ++mRef;
}

#endif /* GAMEBEHAVIORMANGER_I_ */
