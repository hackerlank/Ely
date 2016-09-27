/**
 * \file p3Chaser.cxx
 *
 * \date 2016-09-27
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "p3Chaser.h"
#include "controlManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include <py_panda.h>
extern Dtool_PyTypedObject Dtool_P3Chaser;
#endif //PYTHON_BUILD


Chaser::Chaser(SMARTPTR(ChaserTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameControlManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GameControlManager")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GamePhysicsManager")

	mTmpl = tmpl;
	reset();
}

Chaser::~Chaser()
{
}

bool Chaser::initialize()
{
	bool result = true;
	//get settings from template
	//enabling setting
	mStartEnabled = (
			mTmpl->parameter(std::string("enabled")) == std::string("true") ?
					true : false);
	//backward setting
	mBackward = (
			mTmpl->parameter(std::string("backward")) == std::string("true") ?
					true : false);
	//fixed relative position setting
	mFixedRelativePosition = (
			mTmpl->parameter(std::string("fixed_relative_position"))
					== std::string("false") ? false : true);
	//
	float value;
	//abs max distance
	value = strtof(mTmpl->parameter(std::string("abs_max_distance")).c_str(),
			NULL);
	mAbsMaxDistance = (value >= 0.0 ? value : -value);
	//abs min distance
	value = strtof(mTmpl->parameter(std::string("abs_min_distance")).c_str(),
			NULL);
	mAbsMinDistance = (value >= 0.0 ? value : -value);
	//abs max height
	value = strtof(mTmpl->parameter(std::string("abs_max_height")).c_str(),
			NULL);
	mAbsMaxHeight = (value >= 0.0 ? value : -value);
	//abs min height
	value = strtof(mTmpl->parameter(std::string("abs_min_height")).c_str(),
			NULL);
	mAbsMinHeight = (value >= 0.0 ? value : -value);
	//friction
	value = strtof(mTmpl->parameter(std::string("friction")).c_str(), NULL);
	mFriction = (value >= 0.0 ? value : -value);
	//chased object id
	mChasedId = ObjectId(mTmpl->parameter(std::string("chased_object")));
	//reference object id
	mReferenceId = ObjectId(mTmpl->parameter(std::string("reference_object")));
	//fixed look at
	mFixedLookAt = (
			mTmpl->parameter(std::string("fixed_lookat"))
					== std::string("false") ? false : true);
	//abs lookat distance
	value = strtof(mTmpl->parameter(std::string("abs_lookat_distance")).c_str(),
			NULL);
	mAbsLookAtDistance = (value >= 0.0 ? value : -value);
	//abs lookat height
	value = strtof(mTmpl->parameter(std::string("abs_lookat_height")).c_str(),
			NULL);
	mAbsLookAtHeight = (value >= 0.0 ? value : -value);
	//mouse movement setting
	mMouseEnabledH = (
			mTmpl->parameter(std::string("mouse_enabled_h"))
					== std::string("true") ? true : false);
	mMouseEnabledP = (
			mTmpl->parameter(std::string("mouse_enabled_p"))
					== std::string("true") ? true : false);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->parameter(std::string("head_left"))
					== std::string("enabled") ? true : false);
	//headRight key
	mHeadRightKey = (
			mTmpl->parameter(std::string("head_right"))
					== std::string("enabled") ? true : false);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->parameter(std::string("pitch_up"))
					== std::string("enabled") ? true : false);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->parameter(std::string("pitch_down"))
					== std::string("enabled") ? true : false);
	//inverted rotation
	mSignOfMouse = (
			mTmpl->parameter(std::string("inverted_rotation"))
					== std::string("true") ? -1 : 1);
	//sens x
	value = strtof(mTmpl->parameter(std::string("sens_x")).c_str(),
	NULL);
	mSensX = (value >= 0.0 ? value : -value);
	mHeadSensX = mSensX * 375.0;
	//sens y
	value = strtof(mTmpl->parameter(std::string("sens_y")).c_str(),
	NULL);
	mSensY = (value >= 0.0 ? value : -value);
	mHeadSensY = mSensY * 375.0;
	//
	return result;
}

void Chaser::onAddToObjectSetup()
{
	//set the (node path of) object chased by this component;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not, this component chases nothing.
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
	//
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	mCentX = win->get_properties().get_x_size() / 2;
	mCentY = win->get_properties().get_y_size() / 2;
}

void Chaser::onRemoveFromObjectCleanup()
{
	//see disable
	if (mEnabled and (not mFixedLookAt) and (mMouseEnabledH or mMouseEnabledP))
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}
	//
	reset();
}

void Chaser::onAddToSceneSetup()
{
	//if chased node path is empty return
	RETURN_ON_COND(mChasedNodePath.is_empty(),)

	//enable the component (if requested)
	if (mStartEnabled)
	{
		doEnable();
		//when enabled set chaser initial position/orientation
		mOwnerObject->getNodePath().set_pos(mChasedNodePath, mChaserPosition);
		mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
				LVector3f::up());
	}
	else
	{
		unregisterEventCallbacks();
	}
}

void Chaser::onRemoveFromSceneCleanup()
{
	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);
}

Chaser::Result Chaser::enable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//if enabled return
	RETURN_ON_COND(mEnabled, Result::ERROR)

	//if chased node path is empty return
	RETURN_ON_COND(mChasedNodePath.is_empty(), Result::ERROR)

	//actual ebnabling
	doEnable();
	//
	return Result::OK;
}

void Chaser::doEnable()
{
	//check if backward located
	float sign = (mBackward ? 1.0 : -1.0);
	//set chaser position (wrt chased node)
	mChaserPosition = LPoint3f(0.0, -mAbsMaxDistance * sign,
			mAbsMinHeight * 1.5);
	//set "look at" position (wrt chased node)
	mLookAtPosition = LPoint3f(0.0, mAbsLookAtDistance * sign,
			mAbsLookAtHeight);
	//
	if ((not mFixedLookAt) and (mMouseEnabledH or mMouseEnabledP))
	{
		//we want control through mouse movements
		//hide mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(true);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
		//reset mouse to start position
		win->move_pointer(0, mCentX, mCentY);
	}
	//
	mFixedLookAtNodePath = mOwnerObject->getNodePath().get_parent().attach_new_node(
			"fixedLookAtNP");
	//
	mEnabled = true;
	//register event callbacks if any
	registerEventCallbacks();

	//add to the control manager update
	GameControlManager::GetSingletonPtr()->addToControlUpdate(this);
}

Chaser::Result Chaser::disable()
{
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//if disabling return
		RETURN_ON_ASYNC_COND(mDisabling, Result::CHASER_DISABLING)

		//if not enabled return
		RETURN_ON_COND(not mEnabled, Result::ERROR)

		//if chased node path is empty return
		RETURN_ON_COND(mChasedNodePath.is_empty(), Result::ERROR)

#ifdef ELY_THREAD
		mDisabling = true;
#endif
	}

	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);

	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//actual disabling
	doDisable();
	//
	return Result::OK;
}

void Chaser::doDisable()
{
	//unregister event callbacks if any
	unregisterEventCallbacks();

	//
	mFixedLookAtNodePath.remove_node();
	//
	if ((not mFixedLookAt) and (mMouseEnabledH or mMouseEnabledP))
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}
	//
#ifdef ELY_THREAD
	mDisabling = false;
#endif
	//
	mEnabled = false;
}

void Chaser::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//update chaser position and orientation (see OgreBulletDemos)
	//position
	LPoint3f currentChaserPos = mReferenceNodePath.get_relative_point(
			mOwnerObject->getNodePath(), LPoint3f::zero());
	LPoint3f newPos;
	LPoint3f desiredChaserPos;
	if (mFixedRelativePosition)
	{
		//follow chased node from fixed position wrt it
		desiredChaserPos = mReferenceNodePath.get_relative_point(
				mChasedNodePath, mChaserPosition);
		newPos = doGetChaserPos(desiredChaserPos, currentChaserPos, dt);
		LPoint3f currentChasedPos = mReferenceNodePath.get_relative_point(
				mChasedNodePath, LPoint3f::zero());
		doCorrectChaserHeight(newPos, currentChasedPos.get_z());
	}
	else
	{
		//correct position only if distance < min distance or distance > max distance
		LPoint3f currentChasedPos = mReferenceNodePath.get_relative_point(
				mChasedNodePath, LPoint3f::zero());
		LVector3f distanceDir = currentChaserPos - currentChasedPos;
		float distance = distanceDir.length();
		if (distance < mAbsMinDistance)
		{
			distanceDir.normalize();
			desiredChaserPos = currentChasedPos + distanceDir * mAbsMinDistance;
			newPos = doGetChaserPos(desiredChaserPos, currentChaserPos, dt);
			doCorrectChaserHeight(newPos, currentChasedPos.get_z());
		}
		else if (distance > mAbsMaxDistance)
		{
			distanceDir.normalize();
			desiredChaserPos = currentChasedPos + distanceDir * mAbsMaxDistance;
			newPos = doGetChaserPos(desiredChaserPos, currentChaserPos, dt);
			doCorrectChaserHeight(newPos, currentChasedPos.get_z());
		}
		else
		{
			newPos = currentChaserPos;
		}
	}
	//
	mOwnerObject->getNodePath().set_pos(mReferenceNodePath, newPos);
	//orientation
	if (mFixedLookAt)
	{
		//look at fixed location
		mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
				LVector3f::up());
	}
	else
	{
		//adjust look at with mouse and/or key events
		//handle mouse
		float deltaH = 0.0, deltaP = 0.0, deltaR = 0.0;
		bool wantRotate = false;
		if (mMouseEnabledH or mMouseEnabledP)
		{
			GraphicsWindow* win =
					mTmpl->windowFramework()->get_graphics_window();
			MouseData md = win->get_pointer(0);
			float deltaX = md.get_x() - mCentX;
			float deltaY = md.get_y() - mCentY;

			if (win->move_pointer(0, mCentX, mCentY))
			{
				if (mMouseEnabledH and (deltaX != 0.0))
				{
					deltaH -= deltaX * mSensX * mSignOfMouse;
					wantRotate = true;
				}
				if (mMouseEnabledP and (deltaY != 0.0))
				{
					deltaP -= deltaY * mSensY * mSignOfMouse;
					wantRotate = true;
				}
			}
		}

		//handle keys:
		if (mHeadLeft and (not mHeadRight))
		{
			deltaH += mHeadSensX * dt * mSignOfMouse;
			wantRotate = true;
		}
		else if (mHeadRight and (not mHeadLeft))
		{
			deltaH -= mHeadSensX * dt * mSignOfMouse;
			wantRotate = true;
		}
		if (mPitchUp and (not mPitchDown))
		{
			deltaP += mHeadSensY * dt * mSignOfMouse;
			wantRotate = true;
		}
		else if (mPitchDown and (not mPitchUp))
		{
			deltaP -= mHeadSensY * dt * mSignOfMouse;
			wantRotate = true;
		}

		//update look at
		if (wantRotate)
		{
			//want rotate: update to desired look up
			mOwnerObject->getNodePath().set_hpr(
					mOwnerObject->getNodePath().get_hpr()
							+ LVecBase3f(deltaH, deltaP, deltaR));
		}
		else if (not mHoldLookAt)
		{
			//don't want rotate: return to look up to fixed location
			mFixedLookAtNodePath.set_pos(mOwnerObject->getNodePath().get_pos());
			mFixedLookAtNodePath.look_at(mChasedNodePath, mLookAtPosition,
					LVector3f::up());
			LVecBase3f deltaHPR = mOwnerObject->getNodePath().get_hpr(mFixedLookAtNodePath);
			float kReductFactor = mFriction * dt;
			if (kReductFactor > 1.0)
			{
				kReductFactor = 1.0;
			}
			if (deltaHPR.length_squared() > 0.0)
			{
				deltaHPR -= deltaHPR * kReductFactor;
			}
			mOwnerObject->getNodePath().set_hpr(mFixedLookAtNodePath, deltaHPR);
		}
	}
}

LPoint3f Chaser::doGetChaserPos(LPoint3f desiredChaserPos,
		LPoint3f currentChaserPos, float deltaTime)
{
	float kReductFactor = mFriction * deltaTime;
	if (kReductFactor > 1.0)
	{
		kReductFactor = 1.0;
	}
	//calculate difference between desiredChaserPos and currentChaserPos
	LVector3f deltaPos = currentChaserPos - desiredChaserPos;
	//converge deltaPos.lenght toward zero: proportionally to deltaPos.lenght
	if (deltaPos.length_squared() > 0.0)
	{
		deltaPos -= deltaPos * kReductFactor;
	}
	//calculate new position
	LPoint3f newPos = desiredChaserPos + deltaPos;
	//correct min distance
	LPoint3f chasedPos = mReferenceNodePath.get_relative_point(mChasedNodePath,
			LPoint3f::zero());
	LVector3f newTargetDir = newPos - chasedPos;
	if (newTargetDir.length() < mAbsMinDistance)
	{
		newTargetDir.normalize();
		newPos = chasedPos + newTargetDir * mAbsMinDistance;
	}
	//
	return newPos;
}

void Chaser::doCorrectChaserHeight(LPoint3f& newPos, float baseHeight)
{
	//correct chaser height (not in OgreBulletDemos)
	LPoint3f downTo = LPoint3f(newPos.get_x(), newPos.get_y(), -1000000.0);
	BulletClosestHitRayResult result =
			GamePhysicsManager::GetSingleton().bulletWorld()->ray_test_closest(
					newPos, downTo);
	float hitPosZ = max(result.get_hit_pos().get_z(), baseHeight);
	if (newPos.get_z() < hitPosZ + mAbsMinHeight)
	{
		newPos.set_z(hitPosZ + mAbsMinHeight);
	}
	else if (newPos.get_z() > hitPosZ + mAbsMaxHeight)
	{
		newPos.set_z(hitPosZ + mAbsMaxHeight);
	}
}












///////////////////////////////////////////////////////////

/**
 *
 */
P3Chaser::P3Chaser(const string& name) :
		PandaNode(name)
{
	do_reset();
}

/**
 *
 */
P3Chaser::~P3Chaser()
{
}

/**
 * Initializes the P3Chaser with starting settings.
 * \note Internal use only.
 */
void P3Chaser::do_initialize()
{
	WPT(ControlManager)mTmpl = ControlManager::get_global_ptr();
	//inverted setting (1/-1): not inverted -> 1, inverted -> -1
	mSignOfTranslation = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("inverted_translation"))
			== string("true") ? -1 : 1);
	mSignOfMouse = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("inverted_rotation"))
			== string("true") ? -1 : 1);
	//head limit: enabled@[limit]; limit >= 0.0
	pvector<string> paramValuesStr = parseCompoundString(mTmpl->get_parameter_value(ControlManager::CHASER, string("head_limit")), '@');
	if (paramValuesStr.size() >= 2)
	{
		//enabled
		mHeadLimitEnabled = (
				paramValuesStr[0] == string("true") ? true : false);
		float value;
		//limit
		value = STRTOF(paramValuesStr[1].c_str(), NULL);
		value >= 0.0 ? mHLimit = value : mHLimit = -value;
	}
	//pitch limit: enabled@[limit]; limit >= 0.0
	paramValuesStr = parseCompoundString(
			mTmpl->get_parameter_value(ControlManager::CHASER, string("pitch_limit")), '@');
	if (paramValuesStr.size() >= 2)
	{
		//enabled
		mPitchLimitEnabled = (
				paramValuesStr[0] == string("true") ? true : false);
		float value;
		//limit
		value = STRTOF(paramValuesStr[1].c_str(), NULL);
		value >= 0.0 ? mPLimit = value : mPLimit = -value;
	}
	//mouse movement setting
	mMouseEnabledH = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("mouse_head"))
			== string("enabled") ? true : false);
	mMouseEnabledP = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("mouse_pitch"))
			== string("enabled") ? true : false);
	//key events setting
	//backward key
	mBackwardKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("backward"))
			== string("disabled") ? false : true);
	//down key
	mDownKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("down")) == string("disabled") ?
			false : true);
	//forward key
	mForwardKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("forward"))
			== string("disabled") ? false : true);
	//strafeLeft key
	mStrafeLeftKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("strafe_left"))
			== string("disabled") ? false : true);
	//strafeRight key
	mStrafeRightKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("strafe_right"))
			== string("disabled") ? false : true);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("head_left"))
			== string("disabled") ? false : true);
	//headRight key
	mHeadRightKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("head_right"))
			== string("disabled") ? false : true);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("pitch_up"))
			== string("disabled") ? false : true);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("pitch_down"))
			== string("disabled") ? false : true);
	//up key
	mUpKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("up")) == string("disabled") ?
			false : true);
	//mouseMove key: enabled/disabled
	mMouseMoveKey = (
			mTmpl->get_parameter_value(ControlManager::CHASER, string("mouse_move"))
			== string("enabled") ? true : false);
	//
	float value, absValue;
	//max linear speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("max_linear_speed")).c_str(),
			NULL);
	absValue = (value >= 0.0 ? value : -value);
	mMaxSpeedXYZ = LVecBase3f(absValue, absValue, absValue);
	mMaxSpeedSquaredXYZ = LVector3f(mMaxSpeedXYZ.get_x() * mMaxSpeedXYZ.get_x(),
			mMaxSpeedXYZ.get_y() * mMaxSpeedXYZ.get_y(),
			mMaxSpeedXYZ.get_z() * mMaxSpeedXYZ.get_z());
	//max angular speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("max_angular_speed")).c_str(),
			NULL);
	mMaxSpeedHP = (value >= 0.0 ? value : -value);
	mMaxSpeedSquaredHP = mMaxSpeedHP * mMaxSpeedHP;
	//linear accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("linear_accel")).c_str(), NULL);
	absValue = (value >= 0.0 ? value : -value);
	mAccelXYZ = LVecBase3f(absValue, absValue, absValue);
	//angular accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("angular_accel")).c_str(),
			NULL);
	mAccelHP = (value >= 0.0 ? value : -value);
	//reset actual speeds
	mActualSpeedXYZ = LVector3f::zero();
	mActualSpeedH = 0.0;
	mActualSpeedP = 0.0;
	//linear friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("linear_friction")).c_str(),
			NULL);
	mFrictionXYZ = (value >= 0.0 ? value : -value);
	//angular friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("angular_friction")).c_str(),
			NULL);
	mFrictionHP = (value >= 0.0 ? value : -value);
	//stop threshold ([0.0, 1.0])
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("stop_threshold")).c_str(),
			NULL);
	mStopThreshold =
	(value >= 0.0 ? value - floor(value) : ceil(value) - value);
	//fast factor (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("fast_factor")).c_str(),
			NULL);
	mFastFactor = (value >= 0.0 ? value : -value);
	//sens x (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("sens_x")).c_str(),
			NULL);
	mSensX = (value >= 0.0 ? value : -value);
	//sens_y (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::CHASER, string("sens_y")).c_str(),
			NULL);
	mSensY = (value >= 0.0 ? value : -value);
	//
	mCentX = mWin->get_properties().get_x_size() / 2;
	mCentY = mWin->get_properties().get_y_size() / 2;
	//enabling setting
	if ((mTmpl->get_parameter_value(ControlManager::CHASER,
			string("enabled")) == string("false") ? false : true))
	{
		do_enable();
	}
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_P3Chaser, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

/**
 * On destruction cleanup.
 * Gives an P3Chaser the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void P3Chaser::do_finalize()
{
	//if enabled: disable
	if (mEnabled)
	{
		//actual disabling
		do_disable();
	}
#ifdef PYTHON_BUILD
	//Python callback
	Py_DECREF(mSelf);
	Py_XDECREF(mUpdateCallback);
	Py_XDECREF(mUpdateArgList);
#endif //PYTHON_BUILD
	do_reset();
	//
	return;
}

/**
 * Enables the P3Chaser to perform its task (default: enabled).
 */
bool P3Chaser::enable()
{
	//if enabled return
	RETURN_ON_COND(mEnabled, false)

	//actual enabling
	do_enable();
	//
	return true;
}

/**
 * Enables actually the P3Chaser.
 * \note Internal use only.
 */
void P3Chaser::do_enable()
{
	mEnabled = true;
	// handle mouse if possible
	if ((mMouseEnabledH || mMouseEnabledP) && (!mMouseMoveKey))
	{
		//we want control through mouse movements
		//hide mouse cursor
		WindowProperties props = mWin->get_properties();
		props.set_cursor_hidden(true);
		mWin->request_properties(props);
		//reset mouse to start position
		mWin->move_pointer(0, mCentX, mCentY);
		// start handle mouse
		mMouseHandled = true;
	}
}

/**
 * Disables the P3Chaser to perform its task (default: enabled).
 */
bool P3Chaser::disable()
{
	//if not enabled return
	RETURN_ON_COND(! mEnabled, false)

	//actual disabling
	do_disable();
	//
	return true;
}

/**
 * Disables actually the P3Chaser.
 * \note Internal use only.
 */
void P3Chaser::do_disable()
{
	mEnabled = false;
	// don't handle mouse
	WindowProperties props = mWin->get_properties();
	//show mouse cursor if hidden
	props.set_cursor_hidden(false);
	mWin->request_properties(props);
	// stop handle mouse
	mMouseHandled = false;
}

/**
 * Make mouse handled if possible.
 * \note Internal use only.
 */
void P3Chaser::do_handle_mouse()
{
	// handle mouse if possible
	if (mEnabled && (mMouseEnabledH || mMouseEnabledP) && (!mMouseMoveKey))
	{
		//we want control through mouse movements
		//hide mouse cursor
		WindowProperties props = mWin->get_properties();
		props.set_cursor_hidden(true);
		mWin->request_properties(props);
		//reset mouse to start position
		mWin->move_pointer(0, mCentX, mCentY);
		// start handle mouse
		mMouseHandled = true;
	}
	else
	{
		// don't handle mouse
		WindowProperties props = mWin->get_properties();
		//show mouse cursor if hidden
		props.set_cursor_hidden(false);
		mWin->request_properties(props);
		// stop handle mouse
		mMouseHandled = false;
	}
}

/**
 * Updates the P3Chaser state.
 */
void P3Chaser::update(float dt)
{
	RETURN_ON_COND(!mEnabled,)

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//handle mouse
	if (mMouseHandled)
	{
		MouseData md = mWin->get_pointer(0);
		float deltaX = md.get_x() - mCentX;
		float deltaY = md.get_y() - mCentY;

		if (mWin->move_pointer(0, mCentX, mCentY))
		{
			if (mMouseEnabledH && (deltaX != 0.0))
			{
				mThisNP.set_h(
						mThisNP.get_h() - deltaX * mSensX * mSignOfMouse);
			}
			if (mMouseEnabledP && (deltaY != 0.0))
			{
				mThisNP.set_p(
						mThisNP.get_p() - deltaY * mSensY * mSignOfMouse);
			}
		}
	}
	//update position/orientation
	mThisNP.set_y(mThisNP,
			mActualSpeedXYZ.get_y() * dt * mSignOfTranslation);
	mThisNP.set_x(mThisNP,
			mActualSpeedXYZ.get_x() * dt * mSignOfTranslation);
	mThisNP.set_z(mThisNP, mActualSpeedXYZ.get_z() * dt);
	//head
	if (mHeadLimitEnabled)
	{
		float head = mThisNP.get_h() + mActualSpeedH * dt * mSignOfMouse;
		if (head > mHLimit)
		{
			head = mHLimit;
		}
		else if (head < -mHLimit)
		{
			head = -mHLimit;
		}
		mThisNP.set_h(head);
	}
	else
	{
		mThisNP.set_h(
				mThisNP.get_h() + mActualSpeedH * dt * mSignOfMouse);
	}
	//pitch
	if (mPitchLimitEnabled)
	{
		float pitch = mThisNP.get_p() + mActualSpeedP * dt * mSignOfMouse;
		if (pitch > mPLimit)
		{
			pitch = mPLimit;
		}
		else if (pitch < -mPLimit)
		{
			pitch = -mPLimit;
		}
		mThisNP.set_p(pitch);
	}
	else
	{
		mThisNP.set_p(
				mThisNP.get_p() + mActualSpeedP * dt * mSignOfMouse);
	}

	//update speeds
	//y axis
	if (mForward && (! mBackward))
	{
		if (mAccelXYZ.get_y() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() - mAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() < -mMaxSpeedXYZ.get_y())
			{
				//limit speed
				mActualSpeedXYZ.set_y(-mMaxSpeedXYZ.get_y());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_y(-mMaxSpeedXYZ.get_y());
		}
	}
	else if (mBackward && (! mForward))
	{
		if (mAccelXYZ.get_y() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() + mAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() > mMaxSpeedXYZ.get_y())
			{
				//limit speed
				mActualSpeedXYZ.set_y(mMaxSpeedXYZ.get_y());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_y(mMaxSpeedXYZ.get_y());
		}
	}
	else if (mActualSpeedXYZ.get_y() != 0.0)
	{
		if (mActualSpeedXYZ.get_y() * mActualSpeedXYZ.get_y()
				< mMaxSpeedSquaredXYZ.get_y() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_y(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() * (1.0 - min(mFrictionXYZ * dt, 1.0f)));
		}
	}
	//x axis
	if (mStrafeLeft && (! mStrafeRight))
	{
		if (mAccelXYZ.get_x() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() + mAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() > mMaxSpeedXYZ.get_x())
			{
				//limit speed
				mActualSpeedXYZ.set_x(mMaxSpeedXYZ.get_x());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_x(mMaxSpeedXYZ.get_x());
		}
	}
	else if (mStrafeRight && (! mStrafeLeft))
	{
		if (mAccelXYZ.get_x() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() - mAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() < -mMaxSpeedXYZ.get_x())
			{
				//limit speed
				mActualSpeedXYZ.set_x(-mMaxSpeedXYZ.get_x());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_x(-mMaxSpeedXYZ.get_y());
		}
	}
	else if (mActualSpeedXYZ.get_x() != 0.0)
	{
		if (mActualSpeedXYZ.get_x() * mActualSpeedXYZ.get_x()
				< mMaxSpeedSquaredXYZ.get_x() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_x(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() * (1.0 - min(mFrictionXYZ * dt, 1.0f)));
		}
	}
	//z axis
	if (mUp && (! mDown))
	{
		if (mAccelXYZ.get_z() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() + mAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() > mMaxSpeedXYZ.get_z())
			{
				//limit speed
				mActualSpeedXYZ.set_z(mMaxSpeedXYZ.get_z());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_z(mMaxSpeedXYZ.get_z());
		}
	}
	else if (mDown && (! mUp))
	{
		if (mAccelXYZ.get_z() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() - mAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() < -mMaxSpeedXYZ.get_z())
			{
				//limit speed
				mActualSpeedXYZ.set_z(-mMaxSpeedXYZ.get_z());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_z(-mMaxSpeedXYZ.get_z());
		}
	}
	else if (mActualSpeedXYZ.get_z() != 0.0)
	{
		if (mActualSpeedXYZ.get_z() * mActualSpeedXYZ.get_z()
				< mMaxSpeedSquaredXYZ.get_z() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_z(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() * (1.0 - min(mFrictionXYZ * dt, 1.0f)));
		}
	}
	//rotation h
	if (mHeadLeft && (! mHeadRight))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedH += mAccelHP * dt;
			if (mActualSpeedH > mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedH = mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = mMaxSpeedHP;
		}
	}
	else if (mHeadRight && (! mHeadLeft))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedH -= mAccelHP * dt;
			if (mActualSpeedH < -mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedH = -mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = -mMaxSpeedHP;
		}
	}
	else if (mActualSpeedH != 0.0)
	{
		if (mActualSpeedH * mActualSpeedH < mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedH = 0.0;
		}
		else
		{
			//decelerate
            mActualSpeedH = mActualSpeedH * (1.0 - min(mFrictionHP * dt, 1.0f));
		}
	}
	//rotation p
	if (mPitchUp && (! mPitchDown))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedP += mAccelHP * dt;
			if (mActualSpeedP > mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedP = mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedP = mMaxSpeedHP;
		}
	}
	else if (mPitchDown && (! mPitchUp))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedP -= mAccelHP * dt;
			if (mActualSpeedP < -mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedP = -mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedP = -mMaxSpeedHP;
		}
	}
	else if (mActualSpeedP != 0.0)
	{
		if (mActualSpeedP * mActualSpeedP < mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedP = 0.0;
		}
		else
		{
			//decelerate
			mActualSpeedP = mActualSpeedP * (1.0 - min(mFrictionHP * dt, 1.0f));
		}
	}
	//
#ifdef PYTHON_BUILD
	// execute python callback (if any)
	if (mUpdateCallback && (mUpdateCallback != Py_None))
	{
		PyObject *result;
		result = PyObject_CallObject(mUpdateCallback, mUpdateArgList);
		if (result == NULL)
		{
			string errStr = get_name() +
					string(": Error calling callback function");
			PyErr_SetString(PyExc_TypeError, errStr.c_str());
			return;
		}
		Py_DECREF(result);
	}
#else
	// execute c++ callback (if any)
	if (mUpdateCallback)
	{
		mUpdateCallback(this);
	}
#endif //PYTHON_BUILD
}

/**
 * Writes a sensible description of the P3Chaser to the indicated output
 * stream.
 */
void P3Chaser::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this P3Chaser as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void P3Chaser::set_update_callback(PyObject *value)
{
	if ((!PyCallable_Check(value)) && (value != Py_None))
	{
		PyErr_SetString(PyExc_TypeError,
				"Error: the argument must be callable or None");
		return;
	}

	if (mUpdateArgList == NULL)
	{
		mUpdateArgList = Py_BuildValue("(O)", mSelf);
		if (mUpdateArgList == NULL)
		{
			return;
		}
	}
	Py_DECREF(mSelf);

	Py_XDECREF(mUpdateCallback);
	Py_INCREF(value);
	mUpdateCallback = value;
}
#else
/**
 * Sets the update callback as a c++ function taking this P3Chaser as
 * an argument, or NULL.
 * \note C++ only.
 */
void P3Chaser::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD


//TypedWritable API
/**
 * Tells the BamReader how to create objects of type P3Chaser.
 */
void P3Chaser::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void P3Chaser::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this P3Chaser.
	dg.add_string(get_name());

	///Enable/disable flag.
	dg.add_bool(mEnabled);

	///Key controls and effective keys.
	///@{
	dg.add_bool(mForward);
	dg.add_bool(mBackward);
	dg.add_bool(mStrafeLeft);
	dg.add_bool(mStrafeRight);
	dg.add_bool(mUp);
	dg.add_bool(mDown);
	dg.add_bool(mHeadLeft);
	dg.add_bool(mHeadRight);
	dg.add_bool(mPitchUp);
	dg.add_bool(mPitchDown);
//	dg.add_bool(mMouseMove);
	dg.add_bool(mForwardKey);
	dg.add_bool(mBackwardKey);
	dg.add_bool(mStrafeLeftKey);
	dg.add_bool(mStrafeRightKey);
	dg.add_bool(mUpKey);
	dg.add_bool(mDownKey);
	dg.add_bool(mHeadLeftKey);
	dg.add_bool(mHeadRightKey);
	dg.add_bool(mPitchUpKey);
	dg.add_bool(mPitchDownKey);
	dg.add_bool(mMouseMoveKey);
	///@}

	///Key control values.
	///@{
	dg.add_bool(mMouseEnabledH);
	dg.add_bool(mMouseEnabledP);
	dg.add_bool(mHeadLimitEnabled);
	dg.add_bool(mPitchLimitEnabled);
	dg.add_stdfloat(mHLimit);
	dg.add_stdfloat(mPLimit);
	dg.add_int8(mSignOfTranslation);
	dg.add_int8(mSignOfMouse);
	///@}

	/// Sensitivity settings.
	///@{
	dg.add_stdfloat(mFastFactor);
	mActualSpeedXYZ.write_datagram(dg);
	mMaxSpeedXYZ.write_datagram(dg);
	mMaxSpeedSquaredXYZ.write_datagram(dg);
	dg.add_stdfloat(mActualSpeedH);
	dg.add_stdfloat(mActualSpeedP);
	dg.add_stdfloat(mMaxSpeedHP);
	dg.add_stdfloat(mMaxSpeedSquaredHP);
	mAccelXYZ.write_datagram(dg);
	dg.add_stdfloat(mAccelHP);
	dg.add_stdfloat(mFrictionXYZ);
	dg.add_stdfloat(mFrictionHP);
	dg.add_stdfloat(mStopThreshold);
	dg.add_stdfloat(mSensX);
	dg.add_stdfloat(mSensY);
	///@}

	///The reference node path.
	manager->write_pointer(dg, mReferenceNP.node());
}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int P3Chaser::complete_pointers(TypedWritable **p_list, BamReader *manager)
{
	int pi = PandaNode::complete_pointers(p_list, manager);

	/// Pointers
	///The reference node path.
	PT(PandaNode)referenceNPPandaNode = DCAST(PandaNode, p_list[pi++]);
	mReferenceNP = NodePath::any_path(referenceNPPandaNode);

	return pi;
}

/**
 * This function is called by the BamReader's factory when a new object of
 * type P3Chaser is encountered in the Bam file.  It should create the
 * P3Chaser and extract its information from the file.
 */
TypedWritable *P3Chaser::make_from_bam(const FactoryParams &params)
{
	// continue only if ControlManager exists
	CONTINUE_IF_ELSE_R(ControlManager::get_global_ptr(), NULL)

	// create a P3Chaser with default parameters' values: they'll be restored later
	ControlManager::get_global_ptr()->set_parameters_defaults(
			ControlManager::CHASER);
	P3Chaser *node = DCAST(P3Chaser,
			ControlManager::get_global_ptr()->create_chaser(
					"Chaser").node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new P3Chaser.
 */
void P3Chaser::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this P3Chaser.
	set_name(scan.get_string());

	///Enable/disable flag.
	mEnabled = scan.get_bool();

	///Key controls and effective keys.
	///@{
	mForward = scan.get_bool();
	mBackward = scan.get_bool();
	mStrafeLeft = scan.get_bool();
	mStrafeRight = scan.get_bool();
	mUp = scan.get_bool();
	mDown = scan.get_bool();
	mHeadLeft = scan.get_bool();
	mHeadRight = scan.get_bool();
	mPitchUp = scan.get_bool();
	mPitchDown = scan.get_bool();
//	mMouseMove = scan.get_bool();
	mForwardKey = scan.get_bool();
	mBackwardKey = scan.get_bool();
	mStrafeLeftKey = scan.get_bool();
	mStrafeRightKey = scan.get_bool();
	mUpKey = scan.get_bool();
	mDownKey = scan.get_bool();
	mHeadLeftKey = scan.get_bool();
	mHeadRightKey = scan.get_bool();
	mPitchUpKey = scan.get_bool();
	mPitchDownKey = scan.get_bool();
	mMouseMoveKey = scan.get_bool();
	///@}

	///Key control values.
	///@{
	mMouseEnabledH = scan.get_bool();
	mMouseEnabledP = scan.get_bool();
	mHeadLimitEnabled = scan.get_bool();
	mPitchLimitEnabled = scan.get_bool();
	mHLimit = scan.get_stdfloat();
	mPLimit = scan.get_stdfloat();
	mSignOfTranslation = scan.get_int8();
	mSignOfMouse = scan.get_int8();
	///@}

	/// Sensitivity settings.
	///@{
	mFastFactor = scan.get_stdfloat();
	mActualSpeedXYZ.read_datagram(scan);
	mMaxSpeedXYZ.read_datagram(scan);
	mMaxSpeedSquaredXYZ.read_datagram(scan);
	mActualSpeedH = scan.get_stdfloat();
	mActualSpeedP = scan.get_stdfloat();
	mMaxSpeedHP = scan.get_stdfloat();
	mMaxSpeedSquaredHP = scan.get_stdfloat();
	mAccelXYZ.read_datagram(scan);
	mAccelHP = scan.get_stdfloat();
	mFrictionXYZ = scan.get_stdfloat();
	mFrictionHP = scan.get_stdfloat();
	mStopThreshold = scan.get_stdfloat();
	mSensX = scan.get_stdfloat();
	mSensY = scan.get_stdfloat();
	///@}

	///The reference node path.
	manager->read_pointer(scan);
}

//TypedObject semantics: hardcoded
TypeHandle P3Chaser::_type_handle;
