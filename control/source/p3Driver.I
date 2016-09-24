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
 * Returns if the P3Driver is enabled (default: disabled).
 */
INLINE bool P3Driver::is_enabled() const
{
	return mEnabled;
}

/**
 * Enables/disables the P3Driver for forward movement (default: enabled).
 */
INLINE void P3Driver::enable_forward(bool enable)
{
	if (mForwardKey)
	{
		mForward = enable;
	}
}

/**
 * Returns if the P3Driver's forward movement is enabled.
 */
INLINE bool P3Driver::is_forward_enabled() const
{
	return mForward;
}

/**
 * Enables/disables the P3Driver for backward movement.
 */
INLINE void P3Driver::enable_backward(bool enable)
{
	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

/**
 * Returns if the P3Driver's backward movement is enabled.
 */
INLINE bool P3Driver::is_backward_enabled() const
{
	return mBackward;
}

/**
 * Enables/disables the P3Driver for strafe left movement.
 */
INLINE void P3Driver::enable_strafe_left(bool enable)
{
	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

/**
 * Returns if the P3Driver's strafe left movement is enabled.
 */
INLINE bool P3Driver::is_strafe_left_enabled() const
{

	return mStrafeLeft;
}

/**
 * Enables/disables the P3Driver for strafe right movement.
 */
INLINE void P3Driver::enable_strafe_right(bool enable)
{
	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

/**
 * Returns if the P3Driver's strafe right movement is enabled.
 */
INLINE bool P3Driver::is_strafe_right_enabled() const
{
	return mStrafeRight;
}

/**
 * Enables/disables the P3Driver for up movement.
 */
INLINE void P3Driver::enable_up(bool enable)
{
	if (mUpKey)
	{
		mUp = enable;
	}
}

/**
 * Returns if the P3Driver's up movement is enabled.
 */
INLINE bool P3Driver::is_up_enabled() const
{
	return mUp;
}

/**
 * Enables/disables the P3Driver for down movement.
 */
INLINE void P3Driver::enable_down(bool enable)
{
	if (mDownKey)
	{
		mDown = enable;
	}
}

/**
 * Returns if the P3Driver's down movement is enabled.
 */
INLINE bool P3Driver::is_down_enabled() const
{
	return mDown;
}

/**
 * Enables/disables the P3Driver for head left rotation.
 */
INLINE void P3Driver::enable_head_left(bool enable)
{
	if (mHeadLeftKey)
	{
		mHeadLeft = enable;
	}
}

/**
 * Returns if the P3Driver's head left rotation is enabled.
 */
INLINE bool P3Driver::is_head_left_enabled() const
{
	return mHeadLeft;
}

/**
 * Enables/disables the P3Driver for head right rotation.
 */
INLINE void P3Driver::enable_head_right(bool enable)
{
	if (mHeadRightKey)
	{
		mHeadRight = enable;
	}
}

/**
 * Returns if the P3Driver's head right rotation is enabled.
 */
INLINE bool P3Driver::is_head_right_enabled() const
{
	return mHeadRight;
}

/**
 * Enables/disables the P3Driver for pitch up rotation.
 */
INLINE void P3Driver::enable_pitch_up(bool enable)
{
	if (mPitchUpKey)
	{
		mPitchUp = enable;
	}
}

/**
 * Returns if the P3Driver's pitch up rotation is enabled.
 */
INLINE bool P3Driver::is_pitch_up_enabled() const
{
	return mPitchUp;
}

/**
 * Enables/disables the P3Driver for pitch down rotation.
 */
INLINE void P3Driver::enable_pitch_down(bool enable)
{
	if (mPitchDownKey)
	{
		mPitchDown = enable;
	}
}

/**
 * Returns if the P3Driver's pitch down rotation is enabled.
 */
INLINE bool P3Driver::is_pitch_down_enabled() const
{
	return mPitchDown;
}




//by default we consider mouse moved on every update, because
//we want mouse poll by default; this can be changed by calling
//the enabler (for example by an handler responding to mouse-move
//event if it is possible. See: http://www.panda3d.org/forums/viewtopic.php?t=9326
// http://www.panda3d.org/forums/viewtopic.php?t=6049)

INLINE void P3Driver::enable_mouse_move(bool enable)
{
	if (mMouseMoveKey)
	{
		mMouseMove = enable;
	}
}

INLINE bool P3Driver::is_mouse_move_enabled() const
{
	return mMouseMove;
}

INLINE void P3Driver::set_head_limit(bool enabled, float hLimit)
{
	mHeadLimitEnabled = enabled;
	hLimit >= 0.0 ? mHLimit = hLimit : mHLimit = -hLimit;
}

INLINE void P3Driver::set_pitch_limit(bool enabled, float pLimit)
{
	mPitchLimitEnabled = enabled;
	pLimit >= 0.0 ? mPLimit = pLimit : mPLimit = -pLimit;
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
	mEnabled = false;
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
	mSpeedKey = std::string("shift");
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
	//serialization
	mSerializedDataTmpPtr = NULL;
}

INLINE ostream &operator <<(ostream &out, const P3Driver& driver)
{
	driver.output(out);
	return out;
}

#endif /* P3DRIVER_I_ */
