/**
 * \file p3Activity.I
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef P3ACTIVITY_I_
#define P3ACTIVITY_I_


///P3Activity inline definitions

/**
 * Sets the reference NodePath of this P3Activity.
 */
INLINE void P3Activity::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns if this P3Activity is suitable for static objects.
 */
INLINE bool P3Activity::get_static() const
{
	return mStatic;
}

/**
 * Resets the P3Activity.
 * \note Internal use only.
 */
inline void P3Activity::do_reset()
{
	//
	mStatic = false;
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

INLINE ostream &operator <<(ostream &out, const P3Activity& activity)
{
	activity.output(out);
	return out;
}

#endif /* P3ACTIVITY_I_ */
