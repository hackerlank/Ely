/**
 * \file p3Driver.cxx
 *
 * \date 2016-09-18
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "p3Driver.h"
#include "controlManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include <py_panda.h>
extern Dtool_PyTypedObject Dtool_P3Driver;
#endif //PYTHON_BUILD

/**
 *
 */
P3Driver::P3Driver(const string& name) :
		PandaNode(name)
{
	do_reset();
}

/**
 *
 */
P3Driver::~P3Driver()
{
}

/**
 * Initializes the P3Driver with starting settings.
 * \note Internal use only.
 */
void P3Driver::do_initialize()
{
	WPT(ControlManager)mTmpl = ControlManager::get_global_ptr();
	//inverted setting (1/-1): not inverted -> 1, inverted -> -1
	mSignOfTranslation = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("inverted_translation"))
					== string("true") ? -1 : 1);
	mSignOfMouse = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("inverted_rotation"))
					== string("true") ? -1 : 1);
	//head limit: enabled@[limit]; limit >= 0.0
	pvector<string> paramValuesStr = parseCompoundString(mTmpl->get_parameter_value(ControlManager::DRIVER, string("head_limit")), '@');
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
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("pitch_limit")), '@');
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
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("mouse_enabled_h"))
					== string("true") ? true : false);
	mMouseEnabledP = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("mouse_enabled_p"))
					== string("true") ? true : false);
	//key events setting
	//backward key
	mBackwardKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("backward"))
					== string("disabled") ? false : true);
	//down key
	mDownKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("down")) == string("disabled") ?
					false : true);
	//forward key
	mForwardKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("forward"))
					== string("disabled") ? false : true);
	//strafeLeft key
	mStrafeLeftKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("strafe_left"))
					== string("disabled") ? false : true);
	//strafeRight key
	mStrafeRightKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("strafe_right"))
					== string("disabled") ? false : true);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("head_left"))
					== string("disabled") ? false : true);
	//headRight key
	mHeadRightKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("head_right"))
					== string("disabled") ? false : true);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("pitch_up"))
					== string("disabled") ? false : true);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("pitch_down"))
					== string("disabled") ? false : true);
	//up key
	mUpKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("up")) == string("disabled") ?
					false : true);
	//mouseMove key: enabled/disabled
	mMouseMoveKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("mouse_move"))
					== string("enabled") ? true : false);
	//speedKey
	mSpeedKey = mTmpl->get_parameter_value(ControlManager::DRIVER, string("speed_key"));
	if ((mSpeedKey != string("control")	and (mSpeedKey != string("alt"))))
	{
		mSpeedKey = string("shift");
	}
	//
	float value, absValue;
	//max linear speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("max_linear_speed")).c_str(),
	NULL);
	absValue = (value >= 0.0 ? value : -value);
	mMaxSpeedXYZ = LVecBase3f(absValue, absValue, absValue);
	mMaxSpeedSquaredXYZ = LVector3f(mMaxSpeedXYZ.get_x() * mMaxSpeedXYZ.get_x(),
			mMaxSpeedXYZ.get_y() * mMaxSpeedXYZ.get_y(),
			mMaxSpeedXYZ.get_z() * mMaxSpeedXYZ.get_z());
	//max angular speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("max_angular_speed")).c_str(),
	NULL);
	mMaxSpeedHP = (value >= 0.0 ? value : -value);
	mMaxSpeedSquaredHP = mMaxSpeedHP * mMaxSpeedHP;
	//linear accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("linear_accel")).c_str(), NULL);
	absValue = (value >= 0.0 ? value : -value);
	mAccelXYZ = LVecBase3f(absValue, absValue, absValue);
	//angular accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("angular_accel")).c_str(),
	NULL);
	mAccelHP = (value >= 0.0 ? value : -value);
	//reset actual speeds
	mActualSpeedXYZ = LVector3f::zero();
	mActualSpeedH = 0.0;
	mActualSpeedP = 0.0;
	//linear friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("linear_friction")).c_str(),
			NULL);
	mFrictionXYZ = (value >= 0.0 ? value : -value);
	//angular friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("angular_friction")).c_str(),
			NULL);
	mFrictionHP = (value >= 0.0 ? value : -value);
	//stop threshold ([0.0, 1.0])
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("stop_threshold")).c_str(),
	NULL);
	mStopThreshold =
			(value >= 0.0 ? value - floor(value) : ceil(value) - value);
	//fast factor (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("fast_factor")).c_str(),
	NULL);
	mFastFactor = (value >= 0.0 ? value : -value);
	//sens x (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("sens_x")).c_str(),
	NULL);
	mSensX = (value >= 0.0 ? value : -value);
	//sens_y (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("sens_y")).c_str(),
	NULL);
	mSensY = (value >= 0.0 ? value : -value);
	//
	mCentX = mWin->get_properties().get_x_size() / 2;
	mCentY = mWin->get_properties().get_y_size() / 2;
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_P3Driver, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

/**
 * On destruction cleanup.
 * Gives an P3Driver the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void P3Driver::do_finalize()
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
 * Enables the P3Driver to perform its task (default: disabled).
 */
bool P3Driver::enable()
{
	//if enabled return
	RETURN_ON_COND(mEnabled, false)

	//actual enabling
	do_enable();
	//
	return true;
}

/**
 * Enables actually the P3Driver.
 * \note Internal use only.
 */
void P3Driver::do_enable()
{
	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we want control through mouse movements
		//hide mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(true);
		mWin->request_properties(props);
		//reset mouse to start position
		mWin->move_pointer(0, mCentX, mCentY);
	}
	//
	mEnabled = true;
}

/**
 * Disables the P3Driver to perform its task (default: disabled).
 */
bool P3Driver::disable()
{
	//if not enabled return
	RETURN_ON_COND(not mEnabled, false)

	//actual disabling
	do_disable();
	//
	return true;
}

/**
 * Disables actually the P3Driver.
 * \note Internal use only.
 */
void P3Driver::do_disable()
{
	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		mWin->request_properties(props);
	}
	//
	mEnabled = false;
}

/**
 * Updates the P3Driver state.
 */
void P3Driver::update(float dt)
{
	RETURN_ON_COND(!mEnabled,)

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//handle mouse
	if (mMouseMove and (mMouseEnabledH or mMouseEnabledP))
	{
		MouseData md = mWin->get_pointer(0);
		float deltaX = md.get_x() - mCentX;
		float deltaY = md.get_y() - mCentY;

		if (mWin->move_pointer(0, mCentX, mCentY))
		{
			if (mMouseEnabledH and (deltaX != 0.0))
			{
				mThisNP.set_h(
						mThisNP.get_h() - deltaX * mSensX * mSignOfMouse);
			}
			if (mMouseEnabledP and (deltaY != 0.0))
			{
				mThisNP.set_p(
						mThisNP.get_p() - deltaY * mSensY * mSignOfMouse);
			}
		}
		//if mMouseMoveKey is true we are controlling mouse movements
		//so we need to reset mMouseMove to false
		if (mMouseMoveKey)
		{
			mMouseMove = false;
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
	if (mForward and (not mBackward))
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
	else if (mBackward and (not mForward))
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
	if (mStrafeLeft and (not mStrafeRight))
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
	else if (mStrafeRight and (not mStrafeLeft))
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
	if (mUp and (not mDown))
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
	else if (mDown and (not mUp))
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
	if (mHeadLeft and (not mHeadRight))
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
	else if (mHeadRight and (not mHeadLeft))
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
	if (mPitchUp and (not mPitchDown))
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
	else if (mPitchDown and (not mPitchUp))
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
 * Writes a sensible description of the P3Driver to the indicated output
 * stream.
 */
void P3Driver::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this P3Driver as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void P3Driver::set_update_callback(PyObject *value)
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
 * Sets the update callback as a c++ function taking this P3Driver as
 * an argument, or NULL.
 * \note C++ only.
 */
void P3Driver::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD


//TypedWritable API
/**
 * Tells the BamReader how to create objects of type P3Driver.
 */
void P3Driver::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void P3Driver::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this P3Driver.
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
	dg.add_bool(mMouseMove);
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
	dg.add_string(mSpeedKey);
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
int P3Driver::complete_pointers(TypedWritable **p_list, BamReader *manager)
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
 * type P3Driver is encountered in the Bam file.  It should create the
 * P3Driver and extract its information from the file.
 */
TypedWritable *P3Driver::make_from_bam(const FactoryParams &params)
{
	// continue only if ControlManager exists
	CONTINUE_IF_ELSE_R(ControlManager::get_global_ptr(), NULL)

	// create a P3Driver with default parameters' values: they'll be restored later
	ControlManager::get_global_ptr()->set_parameters_defaults(
			ControlManager::DRIVER);
	P3Driver *node = DCAST(P3Driver,
			ControlManager::get_global_ptr()->create_driver(
					"Driver").node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new P3Driver.
 */
void P3Driver::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this P3Driver.
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
	mMouseMove = scan.get_bool();
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
	mSpeedKey = scan.get_string();
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
TypeHandle P3Driver::_type_handle;
