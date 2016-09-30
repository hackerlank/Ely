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
 * Sets the chased object.
 */
INLINE void P3Listener::set_chased_object(const NodePath& objectNP)
{
	mChasedNP = objectNP;
	// set listener initial position/orientation
	mThisNP.set_pos(mChasedNP, mListenerPosition);
	mThisNP.look_at(mChasedNP, mLookAtPosition,
			LVector3f::up());
}

/**
 * Reurns the chased object.
 */
INLINE NodePath P3Listener::get_chased_object() const
{
	return mChasedNP;
}

/**
 * Returns if the P3Listener is enabled (default: enabled).
 */
INLINE bool P3Listener::is_enabled() const
{
	return mEnabled;
}

/**
 * Enables/disables head left rotation (default: enabled).
 */
INLINE void P3Listener::enable_head_left(bool enable)
{
	mHeadLeftKey = enable;
	if (!enable)
	{
		mHeadLeft = false;
	}
}

/**
 * Returns if head left rotation is enabled/disabled (default: enabled).
 */
INLINE bool P3Listener::is_head_left_enabled() const
{
	return mHeadLeftKey;
}

/**
 * Enables/disables head right rotation (default: enabled).
 */
INLINE void P3Listener::enable_head_right(bool enable)
{
	mHeadRightKey = enable;
	if (!enable)
	{
		mHeadRight = false;
	}
}

/**
 * Returns if head right rotation is enabled/disabled (default: enabled).
 */
INLINE bool P3Listener::is_head_right_enabled() const
{
	return mHeadRightKey;
}

/**
 * Enables/disables pitch up rotation (default: enabled).
 */
INLINE void P3Listener::enable_pitch_up(bool enable)
{
	mPitchUpKey = enable;
	if (!enable)
	{
		mPitchUp = false;
	}
}

/**
 * Returns if pitch up rotation is enabled/disabled (default: enabled).
 */
INLINE bool P3Listener::is_pitch_up_enabled() const
{
	return mPitchUpKey;
}

/**
 * Enables/disables pitch down rotation (default: enabled).
 */
INLINE void P3Listener::enable_pitch_down(bool enable)
{
	mPitchDownKey = enable;
	if (!enable)
	{
		mPitchDown = false;
	}
}

/**
 * Enables/disables mouse head rotation (default: disabled).
 */
INLINE void P3Listener::enable_mouse_head(bool enable)
{
	RETURN_ON_COND(mMouseEnabledH == enable,)

	mMouseEnabledH = enable;
	// handle mouse if possible
	do_handle_mouse();
}

/**
 * Returns if mouse head rotation is enabled/disabled (default: disabled).
 */
INLINE bool P3Listener::is_mouse_head_enabled() const
{
	return mMouseEnabledH;
}

/**
 * Enables/disables mouse pitch rotation (default: disabled).
 */
INLINE void P3Listener::enable_mouse_pitch(bool enable)
{
	RETURN_ON_COND(mMouseEnabledP == enable,)

	mMouseEnabledP = enable;
	// handle mouse if possible
	do_handle_mouse();
}

/**
 * Returns if mouse pitch rotation is enabled/disabled (default: disabled).
 */
INLINE bool P3Listener::is_mouse_pitch_enabled() const
{
	return mMouseEnabledP;
}

/**
 * Enables/disables mouse movement externally handled (default: disabled).
 */
INLINE void P3Listener::enable_mouse_move(bool enable)
{
	RETURN_ON_COND(mMouseMoveKey == enable,)

	mMouseMoveKey = enable;
	// handle mouse if possible
	do_handle_mouse();
}

/**
 * Returns if mouse movement externally handled is enabled/disabled (default:
 * disabled).
 */
INLINE bool P3Listener::is_mouse_move_enabled() const
{
	return mMouseMoveKey;
}

/**
 * Returns if pitch down rotation is enabled/disabled (default: enabled).
 */
INLINE bool P3Listener::is_pitch_down_enabled() const
{
	return mPitchDownKey;
}

/**
 * Activates/deactivates head left rotation.
 */
INLINE void P3Listener::set_rotate_head_left(bool activate)
{
	if (mHeadLeftKey)
	{
		mHeadLeft = activate;
	}
}

/**
 * Returns if head left rotation is activated/deactivated.
 */
INLINE bool P3Listener::get_rotate_head_left() const
{
	return mHeadLeft;
}

/**
 * Activates/deactivates head right rotation.
 */
INLINE void P3Listener::set_rotate_head_right(bool activate)
{
	if (mHeadRightKey)
	{
		mHeadRight = activate;
	}
}

/**
 * Returns if head right rotation is activated/deactivated.
 */
INLINE bool P3Listener::get_rotate_head_right() const
{
	return mHeadRight;
}

/**
 * Activates/deactivates pitch up rotation.
 */
INLINE void P3Listener::set_rotate_pitch_up(bool activate)
{
	if (mPitchUpKey)
	{
		mPitchUp = activate;
	}
}

/**
 * Returns if pitch up rotation is activated/deactivated.
 */
INLINE bool P3Listener::get_rotate_pitch_up() const
{
	return mPitchUp;
}

/**
 * Activates/deactivates pitch down rotation.
 */
INLINE void P3Listener::set_rotate_pitch_down(bool activate)
{
	if (mPitchDownKey)
	{
		mPitchDown = activate;
	}
}

/**
 * Returns if pitch down rotation is activated/deactivated.
 */
INLINE bool P3Listener::get_rotate_pitch_down() const
{
	return mPitchDown;
}

/**
 * Activates/deactivates hold look at.
 * When activated rotation through mouse movements is disabled.
 */
INLINE void P3Listener::set_hold_look_at(bool activate)
{
	mHoldLookAt = activate;
}

/**
 * Returns if hold look at is activated.
 */
INLINE bool P3Listener::get_hold_look_at() const
{
	return mHoldLookAt;
}

/**
 * Activates/deactivates backward chasing (default: activated).
 * When activated the chased object is in front of the P3Listener, otherwise it is
 * behind it.
 */
INLINE void P3Listener::set_backward(bool activate)
{
	RETURN_ON_COND(mBackward == activate,)

	//check if backward located
	float sign = (mBackward ? 1.0 : -1.0);
	//set listener position (wrt chased node)
	mListenerPosition = LPoint3f(0.0, -mAbsMaxDistance * sign,
			mAbsMinHeight * 1.5);
	//set "look at" position (wrt chased node)
	mLookAtPosition = LPoint3f(0.0, mAbsLookAtDistance * sign,
			mAbsLookAtHeight);
}

/**
 * Returns if backward chasing is activated (default: deactivated).
 */
INLINE bool P3Listener::get_backward() const
{
	return mBackward;
}

/**
 * Activates/deactivates backward chasing.
 * When activated
 */
INLINE void P3Listener::set_fixed_relative_position(bool activate)
{
	mFixedRelativePosition = activate;
}

INLINE bool P3Listener::get_fixed_relative_position()
{
	return mFixedRelativePosition;
}

/**
 * Activates/deactivates inverted rotation (default: deactivated).
 */
INLINE void P3Listener::set_inverted_rotation(bool activate)
{
	activate ? mSignOfMouse = -1: mSignOfMouse = 1;
}

/**
 * Returns if inverted rotation is activated/deactivated (default: deactivated).
 */
INLINE bool P3Listener::get_inverted_rotation() const
{
	return (mSignOfMouse == -1) ? true : false;
}

/**
 * Sets the max distance from chased object (>=0.0).
 */
INLINE void P3Listener::set_max_distance(float absMaxDistance)
{
	absMaxDistance >= 0.0 ? mAbsMaxDistance = absMaxDistance : mAbsMaxDistance =
									-absMaxDistance;
}

/**
 * Returns the max distance from chased object (>=0.0).
 */
INLINE float P3Listener::get_max_distance() const
{
	return mAbsMaxDistance;
}

/**
 * Sets the min distance from chased object (>=0.0).
 */
INLINE void P3Listener::set_min_distance(float absMinDistance)
{
	absMinDistance >= 0.0 ? mAbsMinDistance = absMinDistance : mAbsMinDistance =
									-absMinDistance;
}

/**
 * Returns the min distance from chased object (>=0.0).
 */
INLINE float P3Listener::get_min_distance() const
{
	return mAbsMinDistance;
}

/**
 * Sets the max height from ground (>=0.0).
 */
INLINE void P3Listener::set_max_height(float absMaxHeight)
{
	absMaxHeight >= 0.0 ? mAbsMaxHeight = absMaxHeight : mAbsMaxHeight =
									-absMaxHeight;
}

/**
 * Returns the max height from ground (>=0.0).
 */
INLINE float P3Listener::get_max_height() const
{
	return mAbsMaxHeight;
}

/**
 * Sets the min height from ground (>=0.0).
 */
INLINE void P3Listener::set_min_height(float absMinHeight)
{
	absMinHeight >= 0.0 ? mAbsMinHeight = absMinHeight : mAbsMinHeight =
									-absMinHeight;
}

/**
 * Returns the min height from ground (>=0.0).
 */
INLINE float P3Listener::get_min_height() const
{
	return mAbsMinHeight;
}

/**
 * Sets the look at distance (>=0.0).
 */
INLINE void P3Listener::set_look_at_distance(float absLookAtDistance)
{
	absLookAtDistance >= 0.0 ? mAbsLookAtDistance = absLookAtDistance : mAbsLookAtDistance =
									-absLookAtDistance;
}

/**
 * Returns the look at distance (>=0.0).
 */
INLINE float P3Listener::get_look_at_distance() const
{
	return mAbsLookAtDistance;
}

/**
 * Sets the look at height (>=0.0).
 */
INLINE void P3Listener::set_look_at_height(float absLookAtHeight)
{
	absLookAtHeight >= 0.0 ? mAbsLookAtHeight = absLookAtHeight : mAbsLookAtHeight =
									-absLookAtHeight;
}

/**
 * Returns the look at height (>=0.0).
 */
INLINE float P3Listener::get_look_at_height() const
{
	return mAbsLookAtHeight;
}

/**
 * Sets the friction (>=0.0).
 */
INLINE void P3Listener::set_friction(float friction)
{
	friction >= 0.0 ? mFriction = friction : mFriction =
									-friction;
}

/**
 * Returns the friction (>=0.0).
 */
INLINE float P3Listener::get_friction() const
{
	return mFriction;
}

/**
 * Sets mouse movement sensitivities along screen x,y axes (>=0.0).
 */
INLINE void P3Listener::set_sens(float sensX, float sensY)
{
	mSensX = abs(sensX);
	mSensY = abs(sensY);
}

/**
 * Returns mouse movement sensitivities along screen x,y axes (>=0.0).
 */
INLINE ValueList<float> P3Listener::get_sens() const
{
	ValueList<float> sens;
	sens.add_value(mSensX);
	sens.add_value(mSensY);
	return sens;
}

/**
 * Resets the P3Listener.
 * \note Internal use only.
 */
inline void P3Listener::do_reset()
{
	//
	mEnabled = false;
	mChasedNP.clear();
	mHeadLeft = mHeadRight = mPitchUp = mPitchDown = false;
	mHeadLeftKey = mHeadRightKey = mPitchUpKey = mPitchDownKey = mMouseMoveKey =
			false;
	mMouseEnabledH = mMouseEnabledP = mMouseHandled = false;
	mSignOfMouse = 1;
	mFixedLookAtNP.clear();
	mBackward = mFixedRelativePosition = mHoldLookAt = false;
	mFixedLookAt = false;
	mAbsLookAtDistance = mAbsLookAtHeight = mFriction = 0.0;
	mAbsMaxDistance = mAbsMaxHeight = FLT_MAX;
	mAbsMinDistance = mAbsMinHeight = FLT_MIN;
	mListenerPosition = mLookAtPosition = LPoint3f::zero();
	mSensX = mSensY = 0.0;
	mCentX = mCentY = 0.0;
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
