/**
 * \file audioManager.I
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef AUDIOMANGER_I_
#define AUDIOMANGER_I_

///AudioManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath AudioManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void AudioManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of OSSound3ds.
 */
INLINE int AudioManager::get_num_sound3ds() const
{
	return (int) mSound3ds.size();
}

/**
 * Returns the number of OSListeners.
 */
INLINE int AudioManager::get_num_listeners() const
{
	return (int) mListeners.size();
}

/**
 * Get singleton pointer.
 */
INLINE AudioManager* AudioManager::get_global_ptr()
{
	return Singleton<AudioManager>::GetSingletonPtr();
}

/**
 * Get the collide mask.
 */
INLINE CollideMask AudioManager::get_collide_mask() const
{
	return mMask;
}

/**
 * Get the collision root.
 */
INLINE NodePath AudioManager::get_collision_root() const
{
	return mRoot;
}

/**
 * Get the collision traverser.
 */
INLINE CollisionTraverser* AudioManager::get_collision_traverser() const
{
	return mCTrav;
}

/**
 * Get the collision handler.
 */
INLINE CollisionHandlerQueue* AudioManager::get_collision_handler() const
{
	return mCollisionHandler;
}

/**
 * Get the collision ray.
 */
INLINE CollisionRay* AudioManager::get_collision_ray() const
{
	return mPickerRay;
}

/**
 *
 */
inline int AudioManager::unique_ref()
{
	return ++mRef;
}

#endif /* AUDIOMANGER_I_ */
