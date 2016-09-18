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
 * Returns the P3Driver type.
 */
/**
 * Returns the added OSSteerVehicle given its index, or NULL on error.
 */
/**
 * Returns the number of OSSteerVehicles added to this P3Driver.
 */
/**
 * Returns the OSSteerVehicle given its index.
 */
/**
 * Returns the number of OSSteerVehicles added to this P3Driver.
 */
/**
 * Returns the pathway points.
 */
/**
 * Returns the pathway points.
 */
/**
 * Returns the pathway points.
 */
/**
 * Returns the pathway points.
 */
/**
 * Adds an obstacle (associated to an empty NodePath) that is only seen by the
 * OpenSteer library.
 * All parameters should be specified.\n
 * Returns the obstacle's unique reference (>0), or a negative value on error.
 */
/**
 * Returns the obstacle's unique reference (>0) given its index into the list of
 * obstacles added by this P3Driver, or a negative value on error.
 */
/**
 * Returns the number of obstacles added by this P3Driver.
 */
/**
 * Returns a reference to underlying OpenSteer::AbstractPlugIn.
 */
/**
 * OpenSteer::AbstractPlugIn reference conversion function.
 */

INLINE bool P3Driver::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mEnabled;
}

INLINE void P3Driver::enableForward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mForwardKey)
	{
		mForward = enable;
	}
}

INLINE bool P3Driver::isForwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mForward;
}

INLINE void P3Driver::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

INLINE bool P3Driver::isBackwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mBackward;
}

INLINE void P3Driver::enableStrafeLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

INLINE bool P3Driver::isStrafeLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStrafeLeft;
}

INLINE void P3Driver::enableStrafeRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

INLINE bool P3Driver::isStrafeRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStrafeRight;
}

INLINE void P3Driver::enableUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mUpKey)
	{
		mUp = enable;
	}
}

INLINE bool P3Driver::isUpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mUp;
}

INLINE void P3Driver::enableDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mDownKey)
	{
		mDown = enable;
	}
}

INLINE bool P3Driver::isDownEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mDown;
}

INLINE void P3Driver::enableHeadLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadLeftKey)
	{
		mHeadLeft = enable;
	}
}

INLINE bool P3Driver::isHeadLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadLeft;
}

INLINE void P3Driver::enableHeadRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadRightKey)
	{
		mHeadRight = enable;
	}
}

INLINE bool P3Driver::isHeadRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadRight;
}

INLINE void P3Driver::enablePitchUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchUpKey)
	{
		mPitchUp = enable;
	}
}

INLINE bool P3Driver::isPitchUpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchUp;
}

INLINE void P3Driver::enablePitchDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchDownKey)
	{
		mPitchDown = enable;
	}
}

INLINE bool P3Driver::isPitchDownEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchDown;
}


INLINE void P3Driver::enableMouseMove(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mMouseMoveKey)
	{
		mMouseMove = enable;
	}
}

INLINE bool P3Driver::isMouseMoveEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMouseMove;
}

INLINE void P3Driver::setHeadLimit(bool enabled, float hLimit)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mHeadLimitEnabled = enabled;
	hLimit >= 0.0 ? mHLimit = hLimit: mHLimit = -hLimit;
}

INLINE void P3Driver::setPitchLimit(bool enabled, float pLimit)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mPitchLimitEnabled = enabled;
	pLimit >= 0.0 ? mPLimit = pLimit: mPLimit = -pLimit;
}

INLINE void P3Driver::setMaxLinearSpeed(const LVector3f& maxLinearSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mMaxSpeedXYZ = maxLinearSpeed;
	mMaxSpeedSquaredXYZ = LVector3f(
			maxLinearSpeed.get_x() * maxLinearSpeed.get_x(),
			maxLinearSpeed.get_y() * maxLinearSpeed.get_y(),
			maxLinearSpeed.get_z() * maxLinearSpeed.get_z());
}

INLINE void P3Driver::setMaxAngularSpeed(float maxAngularSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mMaxSpeedHP = maxAngularSpeed;
	mMaxSpeedSquaredHP = maxAngularSpeed * maxAngularSpeed;
}

INLINE LVector3f P3Driver::getMaxSpeeds(float& maxAngularSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	maxAngularSpeed = mMaxSpeedHP;
	return mMaxSpeedXYZ;
}

INLINE void P3Driver::setLinearAccel(const LVector3f& linearAccel)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mAccelXYZ = linearAccel;
}

INLINE void P3Driver::setAngularAccel(float angularAccel)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mAccelHP = angularAccel;
}

INLINE LVector3f P3Driver::getAccels(float& angularAccel)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	angularAccel = mAccelHP;
	return mAccelXYZ;
}

INLINE void P3Driver::setLinearFriction(float linearFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFrictionXYZ = linearFriction;
}

INLINE void P3Driver::setAngularFriction(float angularFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFrictionHP = angularFriction;
	if ((mFrictionHP < 0.0) or (mFrictionHP > 1.0))
	{
		mFrictionHP = 0.1;
	}
}

INLINE void P3Driver::getFrictions(float& linearFriction, float& angularFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	linearFriction	= mFrictionXYZ;
	angularFriction	= mFrictionHP;
}

INLINE void P3Driver::setSens(float sensX, float sensY)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mSensX = sensX;
	mSensY = sensY;
}

INLINE void P3Driver::getSens(float& sensX, float& sensY)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	sensX = mSensX;
	sensY = mSensY;
}

INLINE void P3Driver::setFastFactor(float factor)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFastFactor = factor;
}

INLINE float P3Driver::getFastFactor()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFastFactor;
}

INLINE LVector3f P3Driver::getCurrentSpeeds(float& angularSpeedH, float& angularSpeedP)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	angularSpeedH = mActualSpeedH;
	angularSpeedP = mActualSpeedP;
	return mActualSpeedXYZ;
}

inline bool P3Driver::is_enabled() const
{
	return mEnabled;
}

inline void P3Driver::enable_forward(bool enable)
{
	if (mForwardKey)
	{
		mForward = enable;
	}
}

inline bool P3Driver::is_forward_enabled() const
{
	return mForward;
}

inline void P3Driver::enable_backward(bool enable)
{
	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

inline bool P3Driver::is_backward_enabled() const
{
	return mBackward;
}

inline void P3Driver::enable_strafe_left(bool enable)
{
	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

inline bool P3Driver::is_strafe_left_enabled() const
{

	return mStrafeLeft;
}

inline void P3Driver::enable_strafe_right(bool enable)
{
	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

inline bool P3Driver::is_strafe_right_enabled() const
{
	return mStrafeRight;
}

inline void P3Driver::enable_up(bool enable)
{
	if (mUpKey)
	{
		mUp = enable;
	}
}

inline bool P3Driver::is_up_enabled() const
{
	return mUp;
}

inline void P3Driver::enable_down(bool enable)
{
	if (mDownKey)
	{
		mDown = enable;
	}
}

inline bool P3Driver::is_down_enabled() const
{
	return mDown;
}

inline void P3Driver::enable_head_left(bool enable)
{
	if (mHeadLeftKey)
	{
		mHeadLeft = enable;
	}
}

inline bool P3Driver::is_head_left_enabled() const
{
	return mHeadLeft;
}

inline void P3Driver::enable_head_right(bool enable)
{
	if (mHeadRightKey)
	{
		mHeadRight = enable;
	}
}

inline bool P3Driver::is_head_right_enabled() const
{
	return mHeadRight;
}

inline void P3Driver::enable_pitch_up(bool enable)
{
	if (mPitchUpKey)
	{
		mPitchUp = enable;
	}
}

inline bool P3Driver::is_pitch_up_enabled() const
{
	return mPitchUp;
}

inline void P3Driver::enable_pitch_down(bool enable)
{
	if (mPitchDownKey)
	{
		mPitchDown = enable;
	}
}

inline bool P3Driver::is_pitch_down_enabled() const
{
	return mPitchDown;
}

inline void P3Driver::enable_mouse_move(bool enable)
{
	if (mMouseMoveKey)
	{
		mMouseMove = enable;
	}
}

inline bool P3Driver::is_mouse_move_enabled() const
{
	return mMouseMove;
}

inline void P3Driver::set_head_limit(bool enabled, float hLimit)
{
	mHeadLimitEnabled = enabled;
	hLimit >= 0.0 ? mHLimit = hLimit : mHLimit = -hLimit;
}

inline void P3Driver::set_pitch_limit(bool enabled, float pLimit)
{
	mPitchLimitEnabled = enabled;
	pLimit >= 0.0 ? mPLimit = pLimit : mPLimit = -pLimit;
}

inline void P3Driver::set_max_linear_speed(const LVector3f& maxLinearSpeed)
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

inline void P3Driver::set_max_angular_speed(float maxAngularSpeed)
{
	mMaxSpeedHP = abs(maxAngularSpeed);
	mMaxSpeedSquaredHP = maxAngularSpeed * maxAngularSpeed;
}

inline Pair<LVector3f, float> P3Driver::get_max_speeds() const
{
	return Pair<LVector3f, float>(mMaxSpeedXYZ, mMaxSpeedHP);
}

inline void P3Driver::set_linear_accel(const LVector3f& linearAccel)
{
	mAccelXYZ = LVector3f(
			abs(linearAccel.get_x()),
			abs(linearAccel.get_y()),
			abs(linearAccel.get_z()));
}

inline void P3Driver::set_angular_accel(float angularAccel)
{
	mAccelHP = abs(angularAccel);
}

inline Pair<LVector3f, float> P3Driver::get_accels() const
{
	return Pair<LVector3f, float>(mAccelXYZ, mAccelHP);
}

inline void P3Driver::set_linear_friction(float linearFriction)
{
	mFrictionXYZ = abs(linearFriction);
}

inline void P3Driver::set_angular_friction(float angularFriction)
{
	mFrictionHP = abs(angularFriction);
}

inline ValueList<float> P3Driver::get_frictions() const
{
	ValueList<float> frictions;
	frictions.add_value(mFrictionXYZ);
	frictions.add_value(mFrictionHP);
	return frictions;
}

inline void P3Driver::set_sens(float sensX, float sensY)
{
	mSensX = abs(sensX);
	mSensY = abs(sensY);
}

inline ValueList<float> P3Driver::get_sens() const
{
	ValueList<float> sens;
	sens.add_value(mSensX);
	sens.add_value(mSensY);
	return sens;
}

inline void P3Driver::set_fast_factor(float factor)
{
	mFastFactor = abs(factor);
}

inline float P3Driver::get_fast_factor() const
{
	return mFastFactor;
}

inline Pair<LVector3f, ValueList<float> > P3Driver::get_current_speeds() const
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
	mPlugIn = NULL;
	mPlugInType = ONE_TURNING;
	mReferenceNP.clear();
	mReferenceDebugNP.clear();
	mReferenceDebug2DNP.clear();
	mCurrentTime = 0.0;
	mLocalObstacles.first().clear();
	mLocalObstacles.second().clear();
	mSteerVehicles.clear();
	mPathwayPoints.clear();
	mPathwayRadii.clear();
	mPathwaySingleRadius = mPathwayClosedCycle = false;
#ifdef PYTHON_BUILD
	mSelf = NULL;
	mUpdateCallback = NULL;
	mUpdateArgList = NULL;
#else
	mUpdateCallback = NULL;
#endif //PYTHON_BUILD
	//serialization
	mSerializedDataTmpPtr = NULL;
#ifdef ELY_DEBUG
	mDrawer3dNP.clear();
	mDrawer3dStaticNP.clear();
	mDrawer2dNP.clear();
	mDebugCamera.clear();
	mDrawer3d = mDrawer3dStatic = mDrawer2d = NULL;
	mEnableDebugDrawUpdate = false;
#endif
}
inline void P3Driver::reset()
{
	//
	mStartEnabled = mEnabled = false;
	mForward = mBackward = mStrafeLeft = mStrafeRight = mUp = mDown =
			mHeadLeft = mHeadRight = mPitchUp = mPitchDown = false;
	//by default we consider mouse moved on every update, because
	//we want mouse poll by default; this can be changed by calling
	//the enabler (for example by an handler responding to mouse-move
	//event if it is possible. See: http://www.panda3d.org/forums/viewtopic.php?t=9326
	// http://www.panda3d.org/forums/viewtopic.php?t=6049)
	mMouseMove = true;
	mForwardKey = mBackwardKey = mStrafeLeftKey = mStrafeRightKey = mUpKey =
	mDownKey = mHeadLeftKey = mHeadRightKey = mPitchUpKey = mPitchDownKey = mMouseMoveKey = false;
	mSpeedKey = std::string("shift");
	mMouseEnabledH = mMouseEnabledP = mHeadLimitEnabled = mPitchLimitEnabled = false;
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
#ifdef ELY_THREAD
	mDisabling = false;
#endif
}
void P3Driver::do_reset()
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
	mUpdateData.clear();
	mUpdateTask.clear();
}


INLINE ostream &operator <<(ostream &out, const P3Driver& plugIn)
{
	plugIn.output(out);
	return out;
}

#endif /* P3DRIVER_I_ */
