/**
 * \file btSoftBody.cxx
 *
 * \date 2016-10-09
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "btSoftBody.h"
#include "gamePhysicsManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include <py_panda.h>
extern Dtool_PyTypedObject Dtool_BTSoftBody;
#endif //PYTHON_BUILD

/**
 *
 */
BTSoftBody::BTSoftBody(const string& name) :
		PandaNode(name)
{
	do_reset();
}

/**
 *
 */
BTSoftBody::~BTSoftBody()
{
}

/**
 * Initializes the BTSoftBody with starting settings.
 * \note Internal use only.
 */
void BTSoftBody::do_initialize()
{
	WPT(GamePhysicsManager)mTmpl = GamePhysicsManager::get_global_ptr();
	//inverted setting (1/-1): not inverted -> 1, inverted -> -1
	mSignOfTranslation = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("inverted_translation"))
			== string("true") ? -1 : 1);
	mSignOfMouse = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("inverted_rotation"))
			== string("true") ? -1 : 1);
	//head limit: enabled@[limit]; limit >= 0.0
	pvector<string> paramValuesStr = parseCompoundString(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("head_limit")), '@');
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
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("pitch_limit")), '@');
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
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("mouse_head"))
			== string("enabled") ? true : false);
	mMouseEnabledP = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("mouse_pitch"))
			== string("enabled") ? true : false);
	//key events setting
	//forward key
	mForwardKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("forward"))
			== string("disabled") ? false : true);
	//backward key
	mBackwardKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("backward"))
			== string("disabled") ? false : true);
	//up key
	mUpKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("up")) == string("disabled") ?
			false : true);
	//down key
	mDownKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("down")) == string("disabled") ?
			false : true);
	//strafeLeft key
	mStrafeLeftKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("strafe_left"))
			== string("disabled") ? false : true);
	//strafeRight key
	mStrafeRightKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("strafe_right"))
			== string("disabled") ? false : true);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("head_left"))
			== string("disabled") ? false : true);
	//headRight key
	mHeadRightKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("head_right"))
			== string("disabled") ? false : true);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("pitch_up"))
			== string("disabled") ? false : true);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("pitch_down"))
			== string("disabled") ? false : true);
	//mouseMove key: enabled/disabled
	mMouseMoveKey = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("mouse_move"))
			== string("enabled") ? true : false);
	//
	float value, absValue;
	//max linear speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("max_linear_speed")).c_str(),
			NULL);
	absValue = (value >= 0.0 ? value : -value);
	mMaxSpeedXYZ = LVecBase3f(absValue, absValue, absValue);
	mMaxSpeedSquaredXYZ = LVector3f(mMaxSpeedXYZ.get_x() * mMaxSpeedXYZ.get_x(),
			mMaxSpeedXYZ.get_y() * mMaxSpeedXYZ.get_y(),
			mMaxSpeedXYZ.get_z() * mMaxSpeedXYZ.get_z());
	//max angular speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("max_angular_speed")).c_str(),
			NULL);
	mMaxSpeedHP = (value >= 0.0 ? value : -value);
	mMaxSpeedSquaredHP = mMaxSpeedHP * mMaxSpeedHP;
	//linear accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("linear_accel")).c_str(), NULL);
	absValue = (value >= 0.0 ? value : -value);
	mAccelXYZ = LVecBase3f(absValue, absValue, absValue);
	//angular accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("angular_accel")).c_str(),
			NULL);
	mAccelHP = (value >= 0.0 ? value : -value);
	//reset actual speeds
	mActualSpeedXYZ = LVector3f::zero();
	mActualSpeedH = 0.0;
	mActualSpeedP = 0.0;
	//linear friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("linear_friction")).c_str(),
			NULL);
	mFrictionXYZ = (value >= 0.0 ? value : -value);
	//angular friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("angular_friction")).c_str(),
			NULL);
	mFrictionHP = (value >= 0.0 ? value : -value);
	//stop threshold ([0.0, 1.0])
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("stop_threshold")).c_str(),
			NULL);
	mStopThreshold =
	(value >= 0.0 ? value - floor(value) : ceil(value) - value);
	//fast factor (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("fast_factor")).c_str(),
			NULL);
	mFastFactor = (value >= 0.0 ? value : -value);
	//sens x (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("sens_x")).c_str(),
			NULL);
	mSensX = (value >= 0.0 ? value : -value);
	//sens_y (>=0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("sens_y")).c_str(),
			NULL);
	mSensY = (value >= 0.0 ? value : -value);
	//
	mCentX = mWin->get_properties().get_x_size() / 2;
	mCentY = mWin->get_properties().get_y_size() / 2;
	//enabling setting
	if ((mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY,
			string("enabled")) == string("false") ? false : true))
	{
		do_enable();
	}
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_BTSoftBody, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

/**
 * On destruction cleanup.
 * Gives an BTSoftBody the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void BTSoftBody::do_finalize()
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
 * Enables the BTSoftBody to perform its task (default: enabled).
 */
bool BTSoftBody::enable()
{
	//if enabled return
	RETURN_ON_COND(mEnabled, false)

	//actual enabling
	do_enable();
	//
	return true;
}

/**
 * Enables actually the BTSoftBody.
 * \note Internal use only.
 */
void BTSoftBody::do_enable()
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
 * Disables the BTSoftBody to perform its task (default: enabled).
 */
bool BTSoftBody::disable()
{
	//if not enabled return
	RETURN_ON_COND(! mEnabled, false)

	//actual disabling
	do_disable();
	//
	return true;
}

/**
 * Disables actually the BTSoftBody.
 * \note Internal use only.
 */
void BTSoftBody::do_disable()
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
void BTSoftBody::do_handle_mouse()
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
 * Updates the BTSoftBody state.
 */
void BTSoftBody::update(float dt)
{
	RETURN_ON_COND(!mEnabled,)

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	NodePath thisNP = NodePath::any_path(this);
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
				thisNP.set_h(
						thisNP.get_h() - deltaX * mSensX * mSignOfMouse);
			}
			if (mMouseEnabledP && (deltaY != 0.0))
			{
				thisNP.set_p(
						thisNP.get_p() - deltaY * mSensY * mSignOfMouse);
			}
		}
	}
	//update position/orientation
	thisNP.set_y(thisNP,
			mActualSpeedXYZ.get_y() * dt * mSignOfTranslation);
	thisNP.set_x(thisNP,
			mActualSpeedXYZ.get_x() * dt * mSignOfTranslation);
	thisNP.set_z(thisNP, mActualSpeedXYZ.get_z() * dt);
	//head
	if (mHeadLimitEnabled)
	{
		float head = thisNP.get_h() + mActualSpeedH * dt * mSignOfMouse;
		if (head > mHLimit)
		{
			head = mHLimit;
		}
		else if (head < -mHLimit)
		{
			head = -mHLimit;
		}
		thisNP.set_h(head);
	}
	else
	{
		thisNP.set_h(
				thisNP.get_h() + mActualSpeedH * dt * mSignOfMouse);
	}
	//pitch
	if (mPitchLimitEnabled)
	{
		float pitch = thisNP.get_p() + mActualSpeedP * dt * mSignOfMouse;
		if (pitch > mPLimit)
		{
			pitch = mPLimit;
		}
		else if (pitch < -mPLimit)
		{
			pitch = -mPLimit;
		}
		thisNP.set_p(pitch);
	}
	else
	{
		thisNP.set_p(
				thisNP.get_p() + mActualSpeedP * dt * mSignOfMouse);
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
 * Writes a sensible description of the BTSoftBody to the indicated output
 * stream.
 */
void BTSoftBody::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this BTSoftBody as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void BTSoftBody::set_update_callback(PyObject *value)
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
 * Sets the update callback as a c++ function taking this BTSoftBody as
 * an argument, or NULL.
 * \note C++ only.
 */
void BTSoftBody::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD


//TypedWritable API
/**
 * Tells the BamReader how to create objects of type BTSoftBody.
 */
void BTSoftBody::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void BTSoftBody::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this BTSoftBody.
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
int BTSoftBody::complete_pointers(TypedWritable **p_list, BamReader *manager)
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
 * type BTSoftBody is encountered in the Bam file.  It should create the
 * BTSoftBody and extract its information from the file.
 */
TypedWritable *BTSoftBody::make_from_bam(const FactoryParams &params)
{
	// continue only if GamePhysicsManager exists
	CONTINUE_IF_ELSE_R(GamePhysicsManager::get_global_ptr(), NULL)

	// create a BTSoftBody with default parameters' values: they'll be restored later
	GamePhysicsManager::get_global_ptr()->set_parameters_defaults(
			GamePhysicsManager::SOFTBODY);
	BTSoftBody *node = DCAST(BTSoftBody,
			GamePhysicsManager::get_global_ptr()->create_soft_body(
					"Driver").node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new BTSoftBody.
 */
void BTSoftBody::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this BTSoftBody.
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
TypeHandle BTSoftBody::_type_handle;
