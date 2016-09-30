/**
 * \file gameAudioManager.I
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef GAMEAUDIOMANGER_I_
#define GAMEAUDIOMANGER_I_

///GameAudioManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath GameAudioManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void GameAudioManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of OSSound3ds.
 */
INLINE int GameAudioManager::get_num_sound3ds() const
{
	return (int) mSound3ds.size();
}

/**
 * Returns the number of OSListeners.
 */
INLINE int GameAudioManager::get_num_listeners() const
{
	return (int) mListeners.size();
}

/**
 * Get singleton pointer.
 */
INLINE GameAudioManager* GameAudioManager::get_global_ptr()
{
	return Singleton<GameAudioManager>::GetSingletonPtr();
}

/**
 * Get the collide mask.
 */
INLINE CollideMask GameAudioManager::get_collide_mask() const
{
	return mMask;
}

/**
 * Get the collision root.
 */
INLINE NodePath GameAudioManager::get_collision_root() const
{
	return mRoot;
}

/**
 * Get the collision traverser.
 */
INLINE CollisionTraverser* GameAudioManager::get_collision_traverser() const
{
	return mCTrav;
}

/**
 * Get the collision handler.
 */
INLINE CollisionHandlerQueue* GameAudioManager::get_collision_handler() const
{
	return mCollisionHandler;
}

/**
 * Get the collision ray.
 */
INLINE CollisionRay* GameAudioManager::get_collision_ray() const
{
	return mPickerRay;
}

/**
 *
 */
inline int GameAudioManager::unique_ref()
{
	return ++mRef;
}

#endif /* GAMEAUDIOMANGER_I_ */
