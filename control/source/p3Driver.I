/**
 * \file p3Driver.I
 *
 * \date 2016-09-18
 * \author consultit
 */

#ifndef P3DRIVER_I_
#define P3DRIVER_I_


///P3Driver inline definitions

/**
 * Returns if the P3Driver is enabled to perform its task(default: enabled).
 */
INLINE bool P3Driver::is_enabled() const
{
	return mEnabled;
}

/**
 * Enables/disables forward movement (default: enabled).
 */
INLINE void P3Driver::enable_forward(bool enable)
{
	mForwardKey = enable;
	if (!enable)
	{
		mForward = false;
	}
}

/**
 * Returns if forward movement is enabled/disabled (default: enabled).
 */
INLINE bool P3Driver::is_forward_enabled() const
{
	return mForwardKey;
}

/**
 * Enables/disables backward movement (default: enabled).
 */
INLINE void P3Driver::enable_backward(bool enable)
{
	mBackwardKey = enable;
	if (!enable)
	{
		mBackward = false;
	}
}

/**
 * Returns if backward movement is enabled/disabled (default: enabled).
 */
INLINE bool P3Driver::is_backward_enabled() const
{
	return mBackwardKey;
}

/**
 * Enables/disables strafe left movement (default: enabled).
 */
INLINE void P3Driver::enable_strafe_left(bool enable)
{
	mStrafeLeftKey = enable;
	if (!enable)
	{
		mStrafeLeft = false;
	}
}

/**
 * Returns if strafe left movement is enabled/disabled (default: enabled).
 */
INLINE bool P3Driver::is_strafe_left_enabled() const
{
	return mStrafeLeftKey;
}

/**
 * Enables/disables strafe right movement (default: enabled).
 */
INLINE void P3Driver::enable_strafe_right(bool enable)
{
	mStrafeRightKey = enable;
	if (!enable)
	{
		mStrafeRight = false;
	}
}

/**
 * Returns if strafe right movement is enabled/disabled (default: enabled).
 */
INLINE bool P3Driver::is_strafe_right_enabled() const
{
	return mStrafeRightKey;
}

/**
 * Enables/disables up movement (default: enabled).
 */
INLINE void P3Driver::enable_up(bool enable)
{
	mUpKey = enable;
	if (!enable)
	{
		mUp = false;
	}
}

/**
 * Returns if up movement is enabled/disabled (default: enabled).
 */
INLINE bool P3Driver::is_up_enabled() const
{
	return mUpKey;
}

/**
 * Enables/disables down movement (default: enabled).
 */
INLINE void P3Driver::enable_down(bool enable)
{
	mDownKey = enable;
	if (!enable)
	{
		mDown = false;
	}
}

/**
 * Returns if down movement is enabled/disabled (default: enabled).
 */
INLINE bool P3Driver::is_down_enabled() const
{
	return mDownKey;
}

/**
 * Enables/disables head left rotation (default: enabled).
 */
INLINE void P3Driver::enable_head_left(bool enable)
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
INLINE bool P3Driver::is_head_left_enabled() const
{
	return mHeadLeftKey;
}

/**
 * Enables/disables head right rotation (default: enabled).
 */
INLINE void P3Driver::enable_head_right(bool enable)
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
INLINE bool P3Driver::is_head_right_enabled() const
{
	return mHeadRightKey;
}

/**
 * Enables/disables pitch up rotation (default: enabled).
 */
INLINE void P3Driver::enable_pitch_up(bool enable)
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
INLINE bool P3Driver::is_pitch_up_enabled() const
{
	return mPitchUpKey;
}

/**
 * Enables/disables pitch down rotation (default: enabled).
 */
INLINE void P3Driver::enable_pitch_down(bool enable)
{
	mPitchDownKey = enable;
	if (!enable)
	{
		mPitchDown = false;
	}
}

/**
 * Returns if pitch down rotation is enabled/disabled (default: enabled).
 */
INLINE bool P3Driver::is_pitch_down_enabled() const
{
	return mPitchDownKey;
}

/**
 * Enables/disables mouse movement (default: disabled).
 */
INLINE void P3Driver::enable_mouse_move(bool enable)
{
	mMouseMoveKey = enable;
	if (!enable)
	{
		mMouseMove = false;
	}
}

/**
 * Returns if mouse movement is enabled/disabled (default: disabled).
 */
INLINE bool P3Driver::is_mouse_move_enabled() const
{
	return mMouseMoveKey;
}

/**
 * Activates/deactivates the P3Driver for forward movement.
 */
INLINE void P3Driver::set_move_forward(bool activate)
{
	if (mForwardKey)
	{
		mForward = activate;
	}
}

/**
 * Returns if the P3Driver's forward movement is activated.
 */
INLINE bool P3Driver::get_move_forward() const
{
	return mForward;
}

/**
 * Activates/deactivates the P3Driver for backward movement.
 */
INLINE void P3Driver::set_move_backward(bool activate)
{
	if (mBackwardKey)
	{
		mBackward = activate;
	}
}

/**
 * Returns if the P3Driver's backward movement is activated.
 */
INLINE bool P3Driver::get_move_backward() const
{
	return mBackward;
}

/**
 * Activates/deactivates the P3Driver for strafe left movement.
 */
INLINE void P3Driver::set_move_strafe_left(bool activate)
{
	if (mStrafeLeftKey)
	{
		mStrafeLeft = activate;
	}
}

/**
 * Returns if the P3Driver's strafe left movement is activated.
 */
INLINE bool P3Driver::get_move_strafe_left() const
{

	return mStrafeLeft;
}

/**
 * Activates/deactivates the P3Driver for strafe right movement.
 */
INLINE void P3Driver::set_move_strafe_right(bool activate)
{
	if (mStrafeRightKey)
	{
		mStrafeRight = activate;
	}
}

/**
 * Returns if the P3Driver's strafe right movement is activated.
 */
INLINE bool P3Driver::get_move_strafe_right() const
{
	return mStrafeRight;
}

/**
 * Activates/deactivates the P3Driver for up movement.
 */
INLINE void P3Driver::set_move_up(bool activate)
{
	if (mUpKey)
	{
		mUp = activate;
	}
}

/**
 * Returns if the P3Driver's up movement is activated.
 */
INLINE bool P3Driver::get_move_up() const
{
	return mUp;
}

/**
 * Activates/deactivates the P3Driver for down movement.
 */
INLINE void P3Driver::set_move_down(bool activate)
{
	if (mDownKey)
	{
		mDown = activate;
	}
}

/**
 * Returns if the P3Driver's down movement is activated.
 */
INLINE bool P3Driver::get_move_down() const
{
	return mDown;
}

/**
 * Activates/deactivates the P3Driver for head left rotation.
 */
INLINE void P3Driver::set_rotate_head_left(bool activate)
{
	if (mHeadLeftKey)
	{
		mHeadLeft = activate;
	}
}

/**
 * Returns if the P3Driver's head left rotation is activated.
 */
INLINE bool P3Driver::get_rotate_head_left() const
{
	return mHeadLeft;
}

/**
 * Activates/deactivates the P3Driver for head right rotation.
 */
INLINE void P3Driver::set_rotate_head_right(bool activate)
{
	if (mHeadRightKey)
	{
		mHeadRight = activate;
	}
}

/**
 * Returns if the P3Driver's head right rotation is activated.
 */
INLINE bool P3Driver::get_rotate_head_right() const
{
	return mHeadRight;
}

/**
 * Activates/deactivates the P3Driver for pitch up rotation.
 */
INLINE void P3Driver::set_rotate_pitch_up(bool activate)
{
	if (mPitchUpKey)
	{
		mPitchUp = activate;
	}
}

/**
 * Returns if the P3Driver's pitch up rotation is activated.
 */
INLINE bool P3Driver::get_rotate_pitch_up() const
{
	return mPitchUp;
}

/**
 * Activates/deactivates the P3Driver for pitch down rotation.
 */
INLINE void P3Driver::set_rotate_pitch_down(bool activate)
{
	if (mPitchDownKey)
	{
		mPitchDown = activate;
	}
}

/**
 * Returns if the P3Driver's pitch down rotation is activated.
 */
INLINE bool P3Driver::get_rotate_pitch_down() const
{
	return mPitchDown;
}

/**
 * Activates/deactivates the P3Driver for mouse movement.
 * \note By default we consider mouse moved on every update, because we want
 * mouse poll by default; this can be changed by calling the enabler (for
 * example by an handler responding to mouse-move event if it is possible.
 * \see http://www.panda3d.org/forums/viewtopic.php?t=9326
 * \see http://www.panda3d.org/forums/viewtopic.php?t=6049)
 */
INLINE void P3Driver::set_mouse_move(bool activate)
{
	if (mMouseMoveKey)
	{
		mMouseMove = activate;
	}
}

/**
 * Returns if the P3Driver's mouse movement is activated.
 * \note By default we consider mouse moved on every update, because we want
 * mouse poll by default; this can be changed by calling the enabler (for
 * example by an handler responding to mouse-move event if it is possible.
 * \see http://www.panda3d.org/forums/viewtopic.php?t=9326
 * \see http://www.panda3d.org/forums/viewtopic.php?t=6049)
 */
INLINE bool P3Driver::get_mouse_move() const
{
	return mMouseMove;
}

INLINE void P3Driver::set_inverted_translation(bool enable)
{
	enable ? mSignOfTranslation = -1: mSignOfTranslation = 1;
}

INLINE bool P3Driver::get_inverted_translation() const
{
	return (mSignOfTranslation == -1) ? true : false;
}

INLINE void P3Driver::set_inverted_rotation(bool enable)
{
	enable ? mSignOfMouse = -1: mSignOfMouse = 1;
}

INLINE bool P3Driver::get_inverted_rotation() const
{
	return (mSignOfMouse == -1) ? true : false;
}

INLINE void P3Driver::set_head_limit(bool enabled, float hLimit)
{
	mHeadLimitEnabled = enabled;
	hLimit >= 0.0 ? mHLimit = hLimit : mHLimit = -hLimit;
}

INLINE Pair<bool, float> P3Driver::get_head_limit() const
{
	return Pair<bool, float>(mHeadLimitEnabled, mHLimit);
}

INLINE void P3Driver::set_pitch_limit(bool enabled, float pLimit)
{
	mPitchLimitEnabled = enabled;
	pLimit >= 0.0 ? mPLimit = pLimit : mPLimit = -pLimit;
}

INLINE Pair<bool, float> P3Driver::get_pitch_limit() const
{
	return Pair<bool, float>(mPitchLimitEnabled, mPLimit);
}

INLINE void P3Driver::set_max_linear_speed(const LVector3f& maxLinearSpeed)
{
	mMaxSpeedXYZ = LVector3f(
			abs(maxLinearSpeed.get_x()),
			abs(maxLinearSpeed.get_y()),
			abs(maxLinearSpeed.get_z()));
	mMaxSpeedSquaredXYZ = LVector3f(
			maxLinearSpeed.get_x() * maxLinearSpeed.get_x(),
			maxLinearSpeed.get_y() * maxLinearSpeed.get_y(),
			maxLinearSpeed.get_z() * maxLinearSpeed.get_z());
}

INLINE void P3Driver::set_max_angular_speed(float maxAngularSpeed)
{
	mMaxSpeedHP = abs(maxAngularSpeed);
	mMaxSpeedSquaredHP = maxAngularSpeed * maxAngularSpeed;
}

INLINE Pair<LVector3f, float> P3Driver::get_max_speeds() const
{
	return Pair<LVector3f, float>(mMaxSpeedXYZ, mMaxSpeedHP);
}

INLINE void P3Driver::set_linear_accel(const LVector3f& linearAccel)
{
	mAccelXYZ = LVector3f(
			abs(linearAccel.get_x()),
			abs(linearAccel.get_y()),
			abs(linearAccel.get_z()));
}

INLINE void P3Driver::set_angular_accel(float angularAccel)
{
	mAccelHP = abs(angularAccel);
}

INLINE Pair<LVector3f, float> P3Driver::get_accels() const
{
	return Pair<LVector3f, float>(mAccelXYZ, mAccelHP);
}

INLINE void P3Driver::set_linear_friction(float linearFriction)
{
	mFrictionXYZ = abs(linearFriction);
}

INLINE void P3Driver::set_angular_friction(float angularFriction)
{
	mFrictionHP = abs(angularFriction);
}

INLINE ValueList<float> P3Driver::get_frictions() const
{
	ValueList<float> frictions;
	frictions.add_value(mFrictionXYZ);
	frictions.add_value(mFrictionHP);
	return frictions;
}

INLINE void P3Driver::set_stop_threshold(float threshold)
{
	mStopThreshold = (
			threshold >= 0.0 ?
					threshold - floor(threshold) : ceil(threshold) - threshold);
}

INLINE float P3Driver::get_stop_threshold() const
{
	return mStopThreshold;
}

INLINE void P3Driver::set_sens(float sensX, float sensY)
{
	mSensX = abs(sensX);
	mSensY = abs(sensY);
}

INLINE ValueList<float> P3Driver::get_sens() const
{
	ValueList<float> sens;
	sens.add_value(mSensX);
	sens.add_value(mSensY);
	return sens;
}

INLINE void P3Driver::set_fast_factor(float factor)
{
	mFastFactor = abs(factor);
}

INLINE float P3Driver::get_fast_factor() const
{
	return mFastFactor;
}

INLINE void P3Driver::set_mouse_enable_head(bool enable)
{
	mMouseEnabledH = enable;
}

INLINE bool P3Driver::get_mouse_enable_head() const
{
	return mMouseEnabledH;
}

INLINE void P3Driver::set_mouse_enable_pitch(bool enable)
{
	mMouseEnabledP = enable;
}

INLINE bool P3Driver::get_mouse_enable_pitch() const
{
	return mMouseEnabledP;
}

INLINE Pair<LVector3f, ValueList<float> > P3Driver::get_current_speeds() const
{
	ValueList<float> angularSpeeds;
	angularSpeeds.add_value(mActualSpeedH);
	angularSpeeds.add_value(mActualSpeedP);
	return Pair<LVector3f, ValueList<float> >(mActualSpeedXYZ, angularSpeeds);
}

/**
 * Resets the P3Driver.
 * \note Internal use only.
 */
inline void P3Driver::do_reset()
{
	//
	mEnabled = true;
	mForward = mBackward = mStrafeLeft = mStrafeRight = mUp = mDown =
			mHeadLeft = mHeadRight = mPitchUp = mPitchDown = false;
	//by default we consider mouse moved on every update, because
	//we want mouse poll by default; this can be changed by calling
	//the enabler (for example by an handler responding to mouse-move
	//event if it is possible. See: http://www.panda3d.org/forums/viewtopic.php?t=9326
	// http://www.panda3d.org/forums/viewtopic.php?t=6049)
	mMouseMove = true;
	mForwardKey = mBackwardKey = mStrafeLeftKey = mStrafeRightKey = mUpKey = mDownKey =
			mHeadLeftKey = mHeadRightKey = mPitchUpKey = mPitchDownKey =
					mMouseMoveKey = false;
	mMouseEnabledH = mMouseEnabledP = mHeadLimitEnabled = mPitchLimitEnabled =
			false;
	mHLimit = mPLimit = 0.0;
	mSignOfTranslation = mSignOfMouse = 1;
	mFastFactor = 0.0;
	mActualSpeedXYZ = mMaxSpeedXYZ = mMaxSpeedSquaredXYZ = LVecBase3f::zero();
	mActualSpeedH = mActualSpeedP = mMaxSpeedHP = mMaxSpeedSquaredHP = 0.0;
	mAccelXYZ = LVecBase3f::zero();
	mAccelHP = 0.0;
	mFrictionXYZ = mFrictionHP = 0.0;
	mStopThreshold = 0.0;
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

INLINE ostream &operator <<(ostream &out, const P3Driver& driver)
{
	driver.output(out);
	return out;
}

#endif /* P3DRIVER_I_ */
