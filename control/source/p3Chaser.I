/**
 * \file p3Chaser.I
 *
 * \date 2016-09-27
 * \author consultit
 */

#ifndef P3CHASER_I_
#define P3CHASER_I_


///P3Chaser inline definitions

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

INLINE void P3Chaser::set_hold_look_at(bool enable)
{
	mHoldLookAt = enable;
}

INLINE bool P3Chaser::get_hold_look_at() const
{
	return mHoldLookAt;
}

INLINE void P3Chaser::set_fixed_relative_position(bool enable)
{
	mFixedRelativePosition = enable;
}

INLINE bool P3Chaser::get_fixed_relative_position()
{
	return mFixedRelativePosition;
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

INLINE void P3Chaser::set_max_distance(float absMaxDistance)
{
	absMaxDistance >= 0.0 ? mAbsMaxDistance = absMaxDistance : mAbsMaxDistance =
									-absMaxDistance;
}

INLINE float P3Chaser::get_max_distance() const
{
	return mAbsMaxDistance;
}

INLINE void P3Chaser::set_min_distance(float absMinDistance)
{
	absMinDistance >= 0.0 ? mAbsMinDistance = absMinDistance : mAbsMinDistance =
									-absMinDistance;
}

INLINE float P3Chaser::get_min_distance() const
{
	return mAbsMinDistance;
}

INLINE void P3Chaser::set_max_height(float absMaxHeight)
{
	absMaxHeight >= 0.0 ? mAbsMaxHeight = absMaxHeight : mAbsMaxHeight =
									-absMaxHeight;
}

INLINE float P3Chaser::get_max_height() const
{
	return mAbsMaxHeight;
}

INLINE void P3Chaser::set_min_height(float absMinHeight)
{
	absMinHeight >= 0.0 ? mAbsMinHeight = absMinHeight : mAbsMinHeight =
									-absMinHeight;
}

INLINE float P3Chaser::get_min_height() const
{
	return mAbsMinHeight;
}

INLINE void P3Chaser::set_look_at_distance(float absLookAtDistance)
{
	absLookAtDistance >= 0.0 ? mAbsLookAtDistance = absLookAtDistance : mAbsLookAtDistance =
									-absLookAtDistance;
}

INLINE float P3Chaser::get_look_at_distance() const
{
	return mAbsLookAtDistance;
}

INLINE void P3Chaser::set_look_at_height(float absLookAtHeight)
{
	absLookAtHeight >= 0.0 ? mAbsLookAtHeight = absLookAtHeight : mAbsLookAtHeight =
									-absLookAtHeight;
}

INLINE float P3Chaser::get_look_at_height() const
{
	return mAbsLookAtHeight;
}

INLINE void P3Chaser::set_friction(float friction)
{
	friction >= 0.0 ? mFriction = friction : mFriction =
									-friction;
}

INLINE float P3Chaser::get_friction() const
{
	return mFriction;
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

//INLINE ObjectId P3Chaser::getChasedObject() xxx
//{
//	//lock (guard) the mutex
//	HOLD_REMUTEX(mMutex)
//
//	return mChasedId;
//}

//INLINE void P3Chaser::setChasedObject(const ObjectId& objectId) xxx
//{
//	//lock (guard) the mutex
//	HOLD_REMUTEX(mMutex)
//
//	//set the (node path of) object chased by this component;
//	//that object is supposed to be already created,
//	//set up and added to the created objects table;
//	//if not, this component chases nothing.
//	mChasedId = objectId;
//	SMARTPTR(Object)chasedObject =
//	ObjectTemplateManager::GetSingleton().getCreatedObject(
//			mChasedId);
//	if (chasedObject != NULL)
//	{
//		mChasedNodePath = chasedObject->getNodePath();
//
//		//set the (node path of) reference object;
//		//that object is supposed to be already created,
//		//set up and added to the created objects table;
//		//if not, this will be the parent of the chased object.
//		SMARTPTR(Object)referenceObject =
//		ObjectTemplateManager::GetSingleton().getCreatedObject(
//				mReferenceId);
//		if (referenceObject != NULL)
//		{
//			mReferenceNodePath = referenceObject->getNodePath();
//		}
//		else
//		{
//			mReferenceNodePath = mChasedNodePath.get_parent();
//		}
//	}
//}


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

INLINE ostream &operator <<(ostream &out, const P3Chaser& chaser)
{
	chaser.output(out);
	return out;
}

#endif /* P3CHASER_I_ */
