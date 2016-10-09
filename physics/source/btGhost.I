/**
 * \file btGhost.I
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef BTGHOST_I_
#define BTGHOST_I_


///BTGhost inline definitions

/**
 * Sets the reference NodePath of this BTGhost.
 */
INLINE void BTGhost::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of PhysicsSounds added to this BTGhost.
 */
INLINE int BTGhost::get_num_sounds() const
{
	return (int)mSounds.size();
}

/**
 * Returns the PhysicsSound given its index, or NULL on error.
 */
INLINE PT(PhysicsSound) BTGhost::operator [](int index) const
{
	return get_sound(index);
}

/**
 * Returns the number of PhysicsSounds added to this BTGhost.
 */
INLINE int BTGhost::size() const
{
	return get_num_sounds();
}

/**
 * Returns if this BTGhost is suitable for static objects.
 */
INLINE bool BTGhost::get_static() const
{
	return mStatic;
}

/**
 * Returns the minimum distance of all physics sounds added to this BTGhost's
 * (>=0.0).
 * Controls the distance (in units) that this sound begins to fall off. Also
 * affects the rate it falls off. Default is 1.0 CloserFaster, <1.0
 * FartherSlower, >1.0.
 * Don't forget to change this when you change the DistanceFactor
 * (through the PhysicsManager, ie GamePhysicsManager::get_physics_manager()).
 */
INLINE float BTGhost::get_min_distance() const
{
	return mMinDist;
}

/**
 * Returns the maximum distance of all physics sounds added to this BTGhost's
 * (>=0.0).
 * Controls the maximum distance (in units) that this sound stops falling off.
 * The sound does not stop at that point, it just doesn't get any quieter. You
 * should rarely need to adjust this. Default is 1000000000.0.
 */
INLINE float BTGhost::get_max_distance() const
{
	return mMaxDist;
}

/**
 * Resets the BTGhost.
 * \note Internal use only.
 */
inline void BTGhost::do_reset()
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

INLINE ostream &operator <<(ostream &out, const BTGhost& sound3d)
{
	sound3d.output(out);
	return out;
}

#endif /* BTGHOST_I_ */
