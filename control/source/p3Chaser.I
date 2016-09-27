/**
 * \file p3Chaser.I
 *
 * \date 2016-09-27
 * \author consultit
 */

#ifndef P3CHASER_I_
#define P3CHASER_I_


///P3Chaser inline definitions


inline bool P3Chaser::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mEnabled;
}

inline float P3Chaser::getAbsMaxDistance() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMaxDistance;
}

inline void P3Chaser::setAbsMaxDistance(float absMaxDistance)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMaxDistance >= 0.0 ? mAbsMaxDistance = absMaxDistance : mAbsMaxDistance =
									-absMaxDistance;
}

inline float P3Chaser::getAbsMinDistance() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMinDistance;
}

inline void P3Chaser::setAbsMinDistance(float absMinDistance)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMinDistance >= 0.0 ? mAbsMinDistance = absMinDistance : mAbsMinDistance =
									-absMinDistance;
}

inline float P3Chaser::getAbsMaxHeight() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMaxHeight;
}

inline void P3Chaser::setAbsMaxHeight(float absMaxHeight)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMaxHeight >= 0.0 ? mAbsMaxHeight = absMaxHeight : mAbsMaxHeight =
									-absMaxHeight;
}

inline float P3Chaser::getAbsMinHeight() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMinHeight;
}

inline void P3Chaser::setAbsMinHeight(float absMinHeight)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMinHeight >= 0.0 ? mAbsMinHeight = absMinHeight : mAbsMinHeight =
									-absMinHeight;
}

inline float P3Chaser::getAbsLookAtDistance() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsLookAtDistance;
}

inline void P3Chaser::setAbsLookAtDistance(float absLookAtDistance)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absLookAtDistance >= 0.0 ? mAbsLookAtDistance = absLookAtDistance : mAbsLookAtDistance =
									-absLookAtDistance;
}

inline float P3Chaser::getAbsLookAtHeight() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsLookAtHeight;
}

inline void P3Chaser::setAbsLookAtHeight(float absLookAtHeight)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absLookAtHeight >= 0.0 ? mAbsLookAtHeight = absLookAtHeight : mAbsLookAtHeight =
									-absLookAtHeight;
}

inline float P3Chaser::getFriction() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFriction;
}

inline void P3Chaser::setFriction(float friction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	friction >= 0.0 ? mFriction = friction : mFriction =
									-friction;
}

inline void P3Chaser::enableHeadLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadLeftKey)
	{
		mHeadLeft = enable;
	}
}

inline bool P3Chaser::isHeadLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadLeft;
}

inline void P3Chaser::enableHeadRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadRightKey)
	{
		mHeadRight = enable;
	}
}

inline bool P3Chaser::isHeadRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadRight;
}

inline void P3Chaser::enablePitchUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchUpKey)
	{
		mPitchUp = enable;
	}
}

inline bool P3Chaser::isPitchUpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchUp;
}

inline void P3Chaser::enablePitchDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchDownKey)
	{
		mPitchDown = enable;
	}
}

inline bool P3Chaser::isPitchDownEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchDown;
}

inline void P3Chaser::holdLookAt(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mHoldLookAt = enable;
}

inline void P3Chaser::enableFixedRelativePosition(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFixedRelativePosition = enable;
}

inline bool P3Chaser::isFixedRelativePosition()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFixedRelativePosition;
}

inline ObjectId P3Chaser::getChasedObject()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mChasedId;
}

inline void P3Chaser::setChasedObject(const ObjectId& objectId)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//set the (node path of) object chased by this component;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not, this component chases nothing.
	mChasedId = objectId;
	SMARTPTR(Object)chasedObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mChasedId);
	if (chasedObject != NULL)
	{
		mChasedNodePath = chasedObject->getNodePath();

		//set the (node path of) reference object;
		//that object is supposed to be already created,
		//set up and added to the created objects table;
		//if not, this will be the parent of the chased object.
		SMARTPTR(Object)referenceObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				mReferenceId);
		if (referenceObject != NULL)
		{
			mReferenceNodePath = referenceObject->getNodePath();
		}
		else
		{
			mReferenceNodePath = mChasedNodePath.get_parent();
		}
	}
}


inline void P3Chaser::reset()
{
	//
	mChasedNodePath = NodePath();
	mReferenceNodePath = NodePath();
	mFixedLookAtNodePath = NodePath();
	mStartEnabled = mEnabled = mFixedRelativePosition = mBackward =
			mHoldLookAt = false;
	mFixedLookAt = true;
#ifdef ELY_THREAD
	mDisabling = false;
#endif
	mChasedId = ObjectId();
	mReferenceId = ObjectId();
	mAbsLookAtDistance = mAbsLookAtHeight = mFriction = 0.0;
	mAbsMaxDistance = mAbsMaxHeight = FLT_MAX;
	mAbsMinDistance = mAbsMinHeight = FLT_MIN;
	mChaserPosition = mLookAtPosition = LPoint3f::zero();
	mHeadLeftKey = mHeadRightKey = mPitchUpKey = mPitchDownKey = false;
	mHeadLeft = mHeadRight = mPitchUp = mPitchDown = mMouseEnabledH =
			mMouseEnabledP = false;
	mSignOfMouse = 1;
	mSensX = mSensY = 0.0;
	mCentX = mCentY = 0.0;
}


/**
 * Resets the P3Chaser.
 * \note Internal use only.
 */
inline void P3Chaser::do_reset()
{
	//
	mEnabled = false;
	mForward = mBackward = mStrafeLeft = mStrafeRight = mUp = mDown =
			mHeadLeft = mHeadRight = mPitchUp = mPitchDown = false;
	mForwardKey = mBackwardKey = mStrafeLeftKey = mStrafeRightKey = mUpKey =
			mDownKey = mHeadLeftKey = mHeadRightKey = mPitchUpKey =
					mPitchDownKey = mMouseMoveKey = false;
	mMouseEnabledH = mMouseEnabledP = mMouseHandled = mHeadLimitEnabled =
			mPitchLimitEnabled = false;
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






/////////////////////////////////////////////////////////////////////////////
/**
 * Sets the reference NodePath of this P3Chaser.
 */
INLINE void P3Chaser::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns if the P3Chaser is enabled (default: enabled).
 */
INLINE bool P3Chaser::is_enabled() const
{
	return mEnabled;
}

/**
 * Enables/disables forward movement (default: enabled).
 */
INLINE void P3Chaser::enable_forward(bool enable)
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
INLINE bool P3Chaser::is_forward_enabled() const
{
	return mForwardKey;
}

/**
 * Enables/disables backward movement (default: enabled).
 */
INLINE void P3Chaser::enable_backward(bool enable)
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
INLINE bool P3Chaser::is_backward_enabled() const
{
	return mBackwardKey;
}

/**
 * Enables/disables strafe left movement (default: enabled).
 */
INLINE void P3Chaser::enable_strafe_left(bool enable)
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
INLINE bool P3Chaser::is_strafe_left_enabled() const
{
	return mStrafeLeftKey;
}

/**
 * Enables/disables strafe right movement (default: enabled).
 */
INLINE void P3Chaser::enable_strafe_right(bool enable)
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
INLINE bool P3Chaser::is_strafe_right_enabled() const
{
	return mStrafeRightKey;
}

/**
 * Enables/disables up movement (default: enabled).
 */
INLINE void P3Chaser::enable_up(bool enable)
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
INLINE bool P3Chaser::is_up_enabled() const
{
	return mUpKey;
}

/**
 * Enables/disables down movement (default: enabled).
 */
INLINE void P3Chaser::enable_down(bool enable)
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
INLINE bool P3Chaser::is_down_enabled() const
{
	return mDownKey;
}

/**
 * Enables/disables head left rotation (default: enabled).
 */
INLINE void P3Chaser::enable_head_left(bool enable)
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
INLINE bool P3Chaser::is_head_left_enabled() const
{
	return mHeadLeftKey;
}

/**
 * Enables/disables head right rotation (default: enabled).
 */
INLINE void P3Chaser::enable_head_right(bool enable)
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
INLINE bool P3Chaser::is_head_right_enabled() const
{
	return mHeadRightKey;
}

/**
 * Enables/disables pitch up rotation (default: enabled).
 */
INLINE void P3Chaser::enable_pitch_up(bool enable)
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
INLINE bool P3Chaser::is_pitch_up_enabled() const
{
	return mPitchUpKey;
}

/**
 * Enables/disables pitch down rotation (default: enabled).
 */
INLINE void P3Chaser::enable_pitch_down(bool enable)
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
INLINE bool P3Chaser::is_pitch_down_enabled() const
{
	return mPitchDownKey;
}

/**
 * Enables/disables mouse head rotation (default: disabled).
 */
INLINE void P3Chaser::enable_mouse_head(bool enable)
{
	RETURN_ON_COND(mMouseEnabledH == enable,)

	mMouseEnabledH = enable;
	// handle mouse if possible
	do_handle_mouse();
}

/**
 * Returns if mouse head rotation is enabled/disabled (default: disabled).
 */
INLINE bool P3Chaser::is_mouse_head_enabled() const
{
	return mMouseEnabledH;
}

/**
 * Enables/disables mouse pitch rotation (default: disabled).
 */
INLINE void P3Chaser::enable_mouse_pitch(bool enable)
{
	RETURN_ON_COND(mMouseEnabledP == enable,)

	mMouseEnabledP = enable;
	// handle mouse if possible
	do_handle_mouse();
}

/**
 * Returns if mouse pitch rotation is enabled/disabled (default: disabled).
 */
INLINE bool P3Chaser::is_mouse_pitch_enabled() const
{
	return mMouseEnabledP;
}

/**
 * Enables/disables mouse movement externally handled (default: disabled).
 */
INLINE void P3Chaser::enable_mouse_move(bool enable)
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
INLINE bool P3Chaser::is_mouse_move_enabled() const
{
	return mMouseMoveKey;
}

/**
 * Activates/deactivates forward movement.
 */
INLINE void P3Chaser::set_move_forward(bool activate)
{
	if (mForwardKey)
	{
		mForward = activate;
	}
}

/**
 * Returns if the forward movement is activated/deactivated.
 */
INLINE bool P3Chaser::get_move_forward() const
{
	return mForward;
}

/**
 * Activates/deactivates backward movement.
 */
INLINE void P3Chaser::set_move_backward(bool activate)
{
	if (mBackwardKey)
	{
		mBackward = activate;
	}
}

/**
 * Returns if backward movement is activated/deactivated.
 */
INLINE bool P3Chaser::get_move_backward() const
{
	return mBackward;
}

/**
 * Activates/deactivates strafe left movement.
 */
INLINE void P3Chaser::set_move_strafe_left(bool activate)
{
	if (mStrafeLeftKey)
	{
		mStrafeLeft = activate;
	}
}

/**
 * Returns if strafe left movement is activated/deactivated.
 */
INLINE bool P3Chaser::get_move_strafe_left() const
{

	return mStrafeLeft;
}

/**
 * Activates/deactivates strafe right movement.
 */
INLINE void P3Chaser::set_move_strafe_right(bool activate)
{
	if (mStrafeRightKey)
	{
		mStrafeRight = activate;
	}
}

/**
 * Returns if strafe right movement is activated/deactivated.
 */
INLINE bool P3Chaser::get_move_strafe_right() const
{
	return mStrafeRight;
}

/**
 * Activates/deactivates up movement.
 */
INLINE void P3Chaser::set_move_up(bool activate)
{
	if (mUpKey)
	{
		mUp = activate;
	}
}

/**
 * Returns if up movement is activated/deactivated.
 */
INLINE bool P3Chaser::get_move_up() const
{
	return mUp;
}

/**
 * Activates/deactivates down movement.
 */
INLINE void P3Chaser::set_move_down(bool activate)
{
	if (mDownKey)
	{
		mDown = activate;
	}
}

/**
 * Returns if down movement is activated/deactivated.
 */
INLINE bool P3Chaser::get_move_down() const
{
	return mDown;
}

/**
 * Activates/deactivates head left rotation.
 */
INLINE void P3Chaser::set_rotate_head_left(bool activate)
{
	if (mHeadLeftKey)
	{
		mHeadLeft = activate;
	}
}

/**
 * Returns if head left rotation is activated/deactivated.
 */
INLINE bool P3Chaser::get_rotate_head_left() const
{
	return mHeadLeft;
}

/**
 * Activates/deactivates head right rotation.
 */
INLINE void P3Chaser::set_rotate_head_right(bool activate)
{
	if (mHeadRightKey)
	{
		mHeadRight = activate;
	}
}

/**
 * Returns if head right rotation is activated/deactivated.
 */
INLINE bool P3Chaser::get_rotate_head_right() const
{
	return mHeadRight;
}

/**
 * Activates/deactivates pitch up rotation.
 */
INLINE void P3Chaser::set_rotate_pitch_up(bool activate)
{
	if (mPitchUpKey)
	{
		mPitchUp = activate;
	}
}

/**
 * Returns if pitch up rotation is activated/deactivated.
 */
INLINE bool P3Chaser::get_rotate_pitch_up() const
{
	return mPitchUp;
}

/**
 * Activates/deactivates pitch down rotation.
 */
INLINE void P3Chaser::set_rotate_pitch_down(bool activate)
{
	if (mPitchDownKey)
	{
		mPitchDown = activate;
	}
}

/**
 * Returns if pitch down rotation is activated/deactivated.
 */
INLINE bool P3Chaser::get_rotate_pitch_down() const
{
	return mPitchDown;
}

/**
 * Activates/deactivates inverted translation (default: deactivated).
 */
INLINE void P3Chaser::set_inverted_translation(bool enable)
{
	enable ? mSignOfTranslation = -1: mSignOfTranslation = 1;
}

/**
 * Returns if inverted translation is activated/deactivated (default:
 * deactivated).
 */
INLINE bool P3Chaser::get_inverted_translation() const
{
	return (mSignOfTranslation == -1) ? true : false;
}

/**
 * Activates/deactivates inverted rotation (default: deactivated).
 */
INLINE void P3Chaser::set_inverted_rotation(bool enable)
{
	enable ? mSignOfMouse = -1: mSignOfMouse = 1;
}

/**
 * Returns if inverted rotation is activated/deactivated (default: deactivated).
 */
INLINE bool P3Chaser::get_inverted_rotation() const
{
	return (mSignOfMouse == -1) ? true : false;
}

/**
 * Sets head rotation limit (>=0.0).
 */
INLINE void P3Chaser::set_head_limit(bool enabled, float hLimit)
{
	mHeadLimitEnabled = enabled;
	hLimit >= 0.0 ? mHLimit = hLimit : mHLimit = -hLimit;
}

/**
 * Returns head rotation limit (>=0.0).
 */
INLINE Pair<bool, float> P3Chaser::get_head_limit() const
{
	return Pair<bool, float>(mHeadLimitEnabled, mHLimit);
}

/**
 * Sets pitch rotation limit (>=0.0).
 */
INLINE void P3Chaser::set_pitch_limit(bool enabled, float pLimit)
{
	mPitchLimitEnabled = enabled;
	pLimit >= 0.0 ? mPLimit = pLimit : mPLimit = -pLimit;
}

/**
 * Returns pitch rotation limit (>=0.0).
 */
INLINE Pair<bool, float> P3Chaser::get_pitch_limit() const
{
	return Pair<bool, float>(mPitchLimitEnabled, mPLimit);
}

/**
 * Sets max linear speed along local x,y,z axes independently (>=0.0).
 */
INLINE void P3Chaser::set_max_linear_speed(const LVector3f& maxLinearSpeed)
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

/**
 * Sets max angular speed for head/pitch rotations (>=0.0).
 */
INLINE void P3Chaser::set_max_angular_speed(float maxAngularSpeed)
{
	mMaxSpeedHP = abs(maxAngularSpeed);
	mMaxSpeedSquaredHP = maxAngularSpeed * maxAngularSpeed;
}

/**
 * Returns linear/angular max speeds (>=0.0).
 */
INLINE Pair<LVector3f, float> P3Chaser::get_max_speeds() const
{
	return Pair<LVector3f, float>(mMaxSpeedXYZ, mMaxSpeedHP);
}

/**
 * Sets linear accelerations along local x,y,z axes independently (>=0.0).
 */
INLINE void P3Chaser::set_linear_accel(const LVector3f& linearAccel)
{
	mAccelXYZ = LVector3f(
			abs(linearAccel.get_x()),
			abs(linearAccel.get_y()),
			abs(linearAccel.get_z()));
}

/**
 * Sets max angular acceleration for head/pitch rotations (>=0.0).
 */
INLINE void P3Chaser::set_angular_accel(float angularAccel)
{
	mAccelHP = abs(angularAccel);
}

/**
 * Returns linear/angular accelerations (>=0.0).
 */
INLINE Pair<LVector3f, float> P3Chaser::get_accels() const
{
	return Pair<LVector3f, float>(mAccelXYZ, mAccelHP);
}

/**
 * Sets linear friction along local x,y,z axes (>=0.0).
 */
INLINE void P3Chaser::set_linear_friction(float linearFriction)
{
	mFrictionXYZ = abs(linearFriction);
}

/**
 * Sets angular friction for head/pitch rotations (>=0.0).
 */
INLINE void P3Chaser::set_angular_friction(float angularFriction)
{
	mFrictionHP = abs(angularFriction);
}

/**
 * Returns linear/angular frictions (>=0.0).
 */
INLINE ValueList<float> P3Chaser::get_frictions() const
{
	ValueList<float> frictions;
	frictions.add_value(mFrictionXYZ);
	frictions.add_value(mFrictionHP);
	return frictions;
}

/**
 * Sets movement stop threshold ([0.0, 1.0]).
 */
INLINE void P3Chaser::set_stop_threshold(float threshold)
{
	mStopThreshold = (
			threshold >= 0.0 ?
					threshold - floor(threshold) : ceil(threshold) - threshold);
}

/**
 * Returns movement stop threshold ([0.0, 1.0]).
 */
INLINE float P3Chaser::get_stop_threshold() const
{
	return mStopThreshold;
}

/**
 * Sets mouse movement sensitivities along screen x,y axes (>=0.0).
 */
INLINE void P3Chaser::set_sens(float sensX, float sensY)
{
	mSensX = abs(sensX);
	mSensY = abs(sensY);
}

/**
 * Returns mouse movement sensitivities along screen x,y axes (>=0.0).
 */
INLINE ValueList<float> P3Chaser::get_sens() const
{
	ValueList<float> sens;
	sens.add_value(mSensX);
	sens.add_value(mSensY);
	return sens;
}

/**
 * Sets movement fast factor (>=0.0).
 */
INLINE void P3Chaser::set_fast_factor(float factor)
{
	mFastFactor = abs(factor);
}

/**
 * Returns movement fast factor (>=0.0).
 */
INLINE float P3Chaser::get_fast_factor() const
{
	return mFastFactor;
}

/**
 * Returns current linear/angular "signed" speeds along local x,y,z axes and
 * for head/pitch rotations.
 */
INLINE Pair<LVector3f, ValueList<float> > P3Chaser::get_current_speeds() const
{
	ValueList<float> angularSpeeds;
	angularSpeeds.add_value(mActualSpeedH);
	angularSpeeds.add_value(mActualSpeedP);
	return Pair<LVector3f, ValueList<float> >(mActualSpeedXYZ, angularSpeeds);
}

INLINE ostream &operator <<(ostream &out, const P3Chaser& chaser)
{
	chaser.output(out);
	return out;
}

#endif /* P3CHASER_I_ */
