/**
 * \file p3Listener.I
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef P3LISTENER_I_
#define P3LISTENER_I_


///P3Listener inline definitions

/**
 * Sets the reference NodePath of this P3Listener.
 */
INLINE void P3Listener::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns if this P3Listener is suitable for static objects.
 */
INLINE bool P3Listener::get_static() const
{
	return mStatic;
}

/**
 * Resets the P3Listener.
 * \note Internal use only.
 */
inline void P3Listener::do_reset()
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

INLINE ostream &operator <<(ostream &out, const P3Listener& listener)
{
	listener.output(out);
	return out;
}

#endif /* P3LISTENER_I_ */
