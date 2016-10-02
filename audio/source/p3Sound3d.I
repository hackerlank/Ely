/**
 * \file p3Sound3d.I
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef P3SOUND3D_I_
#define P3SOUND3D_I_


///P3Sound3d inline definitions

/**
 * Sets the reference NodePath of this P3Sound3d.
 */
INLINE void P3Sound3d::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of AudioSounds added to this P3Sound3d.
 */
INLINE int P3Sound3d::get_num_sounds() const
{
	return (int)mSounds.size();
}

/**
 * Returns the AudioSound given its index, or NULL on error.
 */
INLINE PT(AudioSound) P3Sound3d::operator [](int index) const
{
	return get_sound(index);
}

/**
 * Returns the number of AudioSounds added to this P3Sound3d.
 */
INLINE int P3Sound3d::size() const
{
	return get_num_sounds();
}

/**
 * Returns if this P3Sound3d is suitable for static objects.
 */
INLINE bool P3Sound3d::get_static() const
{
	return mStatic;
}

/**
 * Returns the minimum distance of P3Sound3d's audio sounds (>=0.0).
 * This is the distance (in units) that these sounds begin to fall off.
 * Also affects the rate it falls off. Default is 3.28 (in feet, this is equal
 * to 1 meter). Don't forget to change this when you change the DistanceFactor
 * (through the AudioManager, ie GameAudioManager::get_audio_manager()).
 */
INLINE float P3Sound3d::get_min_distance() const
{
	return mMinDist;
}

/**
 * Returns the maximum distance of P3Sound3d's audio sounds (>=0.0).
 * This is the maximum distance (in units) that these sounds stop falling off.
 * The sounds do not stop at that point, they just don't get any quieter.
 * You should rarely need to adjust this. Default is 1000000000.0
 */
INLINE float P3Sound3d::get_max_distance() const
{
	return mMaxDist;
}

/**
 * Resets the P3Sound3d.
 * \note Internal use only.
 */
inline void P3Sound3d::do_reset()
{
	//
	mStatic = false;
	mSounds.clear();
	mMinDist = FLT_MIN;
	mMaxDist = FLT_MAX;
	mPosition = LPoint3f::zero();
	mReferenceNP.clear();
#ifdef PYTHON_BUILD
	mSelf = NULL;
	mUpdateCallback = NULL;
	mUpdateArgList = NULL;
#else
	mUpdateCallback = NULL;
#endif //PYTHON_BUILD
}

INLINE ostream &operator <<(ostream &out, const P3Sound3d& sound3d)
{
	sound3d.output(out);
	return out;
}

#endif /* P3SOUND3D_I_ */
