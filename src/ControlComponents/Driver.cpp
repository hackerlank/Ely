/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/src/ControlComponents/Driver.cpp
 *
 * \date 31/mag/2012 (16:42:14)
 * \author consultit
 */

#include "ControlComponents/Driver.h"
#include "ObjectModel/Object.h"
#include "Game/GameControlManager.h"
#include <cmath>

namespace ely
{

Driver::Driver()
{
	reset();
}

Driver::Driver(SMARTPTR(DriverTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameControlManager::GetSingletonPtr(),
	"Driver::Driver: invalid GameControlManager")

	mTmpl = tmpl;
	reset();
}

Driver::~Driver()
{
}

ComponentFamilyType Driver::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Driver::componentType() const
{
	return mTmpl->componentType();
}

bool Driver::initialize()
{
	bool result = true;
	//get settings from template
	//enabling setting
	mStartEnabled = (
			mTmpl->parameter(std::string("enabled")) == std::string("false") ?
					false : true);
	//inverted setting
	mSignOfTranslation = (
			mTmpl->parameter(std::string("inverted_translation"))
					== std::string("true") ? -1 : 1);
	mSignOfMouse = (
			mTmpl->parameter(std::string("inverted_rotation"))
					== std::string("true") ? -1 : 1);
	//head limit: enabled@[limit]
	std::vector<std::string> paramValuesStr = parseCompoundString(
			mTmpl->parameter(std::string("head_limit")), '@');
	if (paramValuesStr.size() >= 2)
	{
		//enabled
		mHeadLimitEnabled = (
				paramValuesStr[0] == std::string("true") ? true : false);
		float value;
		//limit
		value = strtof(paramValuesStr[1].c_str(), NULL);
		value >= 0.0 ? mHLimit = value : mHLimit = -value;
	}
	//pitch limit: enabled@[limit]
	paramValuesStr = parseCompoundString(
			mTmpl->parameter(std::string("pitch_limit")), '@');
	if (paramValuesStr.size() >= 2)
	{
		//enabled
		mPitchLimitEnabled = (
				paramValuesStr[0] == std::string("true") ? true : false);
		float value;
		//limit
		value = strtof(paramValuesStr[1].c_str(), NULL);
		value >= 0.0 ? mPLimit = value : mPLimit = -value;
	}
	//mouse movement setting
	mMouseEnabledH = (
			mTmpl->parameter(std::string("mouse_enabled_h"))
					== std::string("true") ? true : false);
	mMouseEnabledP = (
			mTmpl->parameter(std::string("mouse_enabled_p"))
					== std::string("true") ? true : false);
	//key events setting
	//backward key
	mBackwardKey = (
			mTmpl->parameter(std::string("backward"))
					== std::string("disabled") ? false : true);
	//down key
	mDownKey = (
			mTmpl->parameter(std::string("down")) == std::string("disabled") ?
					false : true);
	//forward key
	mForwardKey = (
			mTmpl->parameter(std::string("forward"))
					== std::string("disabled") ? false : true);
	//strafeLeft key
	mStrafeLeftKey = (
			mTmpl->parameter(std::string("strafe_left"))
					== std::string("disabled") ? false : true);
	//strafeRight key
	mStrafeRightKey = (
			mTmpl->parameter(std::string("strafe_right"))
					== std::string("disabled") ? false : true);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->parameter(std::string("head_left"))
					== std::string("disabled") ? false : true);
	//headRight key
	mHeadRightKey = (
			mTmpl->parameter(std::string("head_right"))
					== std::string("disabled") ? false : true);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->parameter(std::string("pitch_up"))
					== std::string("disabled") ? false : true);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->parameter(std::string("pitch_down"))
					== std::string("disabled") ? false : true);
	//up key
	mUpKey = (
			mTmpl->parameter(std::string("up")) == std::string("disabled") ?
					false : true);
	//mouseMove key
	mMouseMoveKey = (
			mTmpl->parameter(std::string("mouse_move"))
					== std::string("enabled") ? true : false);
	//speedKey
	mSpeedKey = mTmpl->parameter(std::string("speed_key"));
	if (not (mSpeedKey == std::string("control")
			or mSpeedKey == std::string("alt")
			or mSpeedKey == std::string("shift")))
	{
		mSpeedKey = std::string("shift");
	}

	//
	float value, absValue;
	//max linear speed
	value = strtof(mTmpl->parameter(std::string("max_linear_speed")).c_str(),
	NULL);
	absValue = (value >= 0.0 ? value : -value);
	mMaxSpeedXYZ = LVecBase3f(absValue, absValue, absValue);
	mMaxSpeedSquaredXYZ = LVector3f(mMaxSpeedXYZ.get_x() * mMaxSpeedXYZ.get_x(),
			mMaxSpeedXYZ.get_y() * mMaxSpeedXYZ.get_y(),
			mMaxSpeedXYZ.get_z() * mMaxSpeedXYZ.get_z());
	//max angular speed
	value = strtof(mTmpl->parameter(std::string("max_angular_speed")).c_str(),
	NULL);
	mMaxSpeedHP = (value >= 0.0 ? value : -value);
	mMaxSpeedSquaredHP = mMaxSpeedHP * mMaxSpeedHP;
	//linear accel
	value = strtof(mTmpl->parameter(std::string("linear_accel")).c_str(), NULL);
	absValue = (value >= 0.0 ? value : -value);
	mAccelXYZ = LVecBase3f(absValue, absValue, absValue);
	//angular accel
	value = strtof(mTmpl->parameter(std::string("angular_accel")).c_str(),
	NULL);
	mAccelHP = (value >= 0.0 ? value : -value);
	//reset actual speeds
	mActualSpeedXYZ = LVector3f::zero();
	mActualSpeedH = 0.0;
	mActualSpeedP = 0.0;
	//linear friction
	value = strtof(mTmpl->parameter(std::string("linear_friction")).c_str(),
			NULL);
	mFrictionXYZ = (value >= 0.0 ? value : -value);
	//angular friction
	value = strtof(mTmpl->parameter(std::string("angular_friction")).c_str(),
			NULL);
	mFrictionHP = (value >= 0.0 ? value : -value);
	//stop threshold [0.0, 1.0]
	value = strtof(mTmpl->parameter(std::string("stop_threshold")).c_str(),
	NULL);
	mStopThreshold =
			(value >= 0.0 ? value - floor(value) : ceil(value) - value);
	//fast factor
	value = strtof(mTmpl->parameter(std::string("fast_factor")).c_str(),
	NULL);
	mFastFactor = (value >= 0.0 ? value : -value);
	//sens x
	value = strtof(mTmpl->parameter(std::string("sens_x")).c_str(),
	NULL);
	mSensX = (value >= 0.0 ? value : -value);
	//sens_y
	value = strtof(mTmpl->parameter(std::string("sens_y")).c_str(),
	NULL);
	mSensY = (value >= 0.0 ? value : -value);
	//
	return result;
}

void Driver::onAddToObjectSetup()
{
	//
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	mCentX = win->get_properties().get_x_size() / 2;
	mCentY = win->get_properties().get_y_size() / 2;
}

void Driver::onRemoveFromObjectCleanup()
{
	//see disable
	if (mEnabled and (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey))
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

void Driver::onAddToSceneSetup()
{
	//enable the component (if requested)
	if (mStartEnabled)
	{
		doEnable();
	}
	else
	{
		unregisterEventCallbacks();
	}
}

void Driver::onRemoveFromSceneCleanup()
{
	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);
}

Driver::Result Driver::enable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//if enabled return
	RETURN_ON_COND(mEnabled, Result::ERROR)

	//actual ebnabling
	doEnable();
	//
	return Result::OK;
}

void Driver::doEnable()
{
	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
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
	mEnabled = true;
	//register event callbacks if any
	registerEventCallbacks();

	//add to the control manager update
	GameControlManager::GetSingletonPtr()->addToControlUpdate(this);
}

Driver::Result Driver::disable()
{
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//if disabling return
		RETURN_ON_ASYNC_COND(mDisabling, Result::DRIVER_DISABLING)

		//if not enabled return
		RETURN_ON_COND(not mEnabled, Result::ERROR)

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

void Driver::doDisable()
{
	//unregister event callbacks if any
	unregisterEventCallbacks();

	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
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

void Driver::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

	NodePath ownerNodePath = mOwnerObject->getNodePath();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//handle mouse
	if (mMouseMove and (mMouseEnabledH or mMouseEnabledP))
	{
		GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
		MouseData md = win->get_pointer(0);
		float deltaX = md.get_x() - mCentX;
		float deltaY = md.get_y() - mCentY;

		if (win->move_pointer(0, mCentX, mCentY))
		{
			if (mMouseEnabledH and (deltaX != 0.0))
			{
				ownerNodePath.set_h(
						ownerNodePath.get_h()
						- deltaX * mSensX * mSignOfMouse);
			}
			if (mMouseEnabledP and (deltaY != 0.0))
			{
				ownerNodePath.set_p(
						ownerNodePath.get_p()
						- deltaY * mSensY * mSignOfMouse);
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
	ownerNodePath.set_y(ownerNodePath,
			mActualSpeedXYZ.get_y() * dt * mSignOfTranslation);
	ownerNodePath.set_x(ownerNodePath,
			mActualSpeedXYZ.get_x() * dt * mSignOfTranslation);
	ownerNodePath.set_z(ownerNodePath,
			mActualSpeedXYZ.get_z() * dt);
	//head
	if (mHeadLimitEnabled)
	{
		float head = ownerNodePath.get_h()
				+ mActualSpeedH * dt * mSignOfMouse;
		if (head > mHLimit)
		{
			head = mHLimit;
		}
		else if(head < -mHLimit)
		{
			head = -mHLimit;
		}
		ownerNodePath.set_h(head);
	}
	else
	{
		ownerNodePath.set_h(ownerNodePath.get_h()
				+ mActualSpeedH * dt * mSignOfMouse);
	}
	//pitch
	if (mPitchLimitEnabled)
	{
		float pitch = ownerNodePath.get_p()
						+ mActualSpeedP * dt * mSignOfMouse;
		if (pitch > mPLimit)
		{
			pitch = mPLimit;
		}
		else if(pitch < -mPLimit)
		{
			pitch = -mPLimit;
		}
		ownerNodePath.set_p(pitch);
	}
	else
	{
		ownerNodePath.set_p(ownerNodePath.get_p()
				+ mActualSpeedP * dt * mSignOfMouse);
	}

	//update speeds
	float kLinearReductFactor = mFrictionXYZ * dt;
	if (kLinearReductFactor > 1.0)
	{
		kLinearReductFactor = 1.0;
	}
	//y axis
	if (mForward and (not mBackward))
	{
		if(mAccelXYZ.get_y() != 0)
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
		if(mAccelXYZ.get_y() != 0)
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
		if (mActualSpeedXYZ.get_y() * mActualSpeedXYZ.get_y() <
				mMaxSpeedSquaredXYZ.get_y() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_y(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() * (1 - kLinearReductFactor));
		}
	}
	//x axis
	if (mStrafeLeft and (not mStrafeRight))
	{
		if(mAccelXYZ.get_x() != 0)
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
		if(mAccelXYZ.get_x() != 0)
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
		if (mActualSpeedXYZ.get_x() * mActualSpeedXYZ.get_x() <
				mMaxSpeedSquaredXYZ.get_x() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_x(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() * (1 - kLinearReductFactor));
		}
	}
	//z axis
	if (mUp and (not mDown))
	{
		if(mAccelXYZ.get_z() != 0)
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
		if(mAccelXYZ.get_z() != 0)
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
		if (mActualSpeedXYZ.get_z() * mActualSpeedXYZ.get_z() <
				mMaxSpeedSquaredXYZ.get_z() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_z(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() * (1 - kLinearReductFactor));
		}
	}
	//rotation h
	if (mHeadLeft and (not mHeadRight))
	{
		if(mAccelHP != 0)
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
		if(mAccelHP != 0)
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
		if (mActualSpeedH * mActualSpeedH <
				mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedH = 0.0;
		}
		else
		{
			//decelerate
			float kAngularReductFactor = mFrictionHP * dt;
			if (kAngularReductFactor > 1.0)
			{
				kAngularReductFactor = 1.0;
			}
			mActualSpeedH = mActualSpeedH * (1 - kAngularReductFactor);
		}
	}
	//rotation p
	if (mPitchUp and (not mPitchDown))
	{
		if(mAccelHP != 0)
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
		if(mAccelHP != 0)
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
		if (mActualSpeedP * mActualSpeedP <
				mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedP = 0.0;
		}
		else
		{
			//decelerate
			float kAngularReductFactor = mFrictionHP * dt;
			if (kAngularReductFactor > 1.0)
			{
				kAngularReductFactor = 1.0;
			}
			mActualSpeedP = mActualSpeedP * (1 - kAngularReductFactor);
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle Driver::_type_handle;

///Template

DriverTemplate::DriverTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"DriverTemplate::DriverTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"DriverTemplate::DriverTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameControlManager::GetSingletonPtr(),
			"DriverTemplate::DriverTemplate: invalid GameControlManager")
	//
	setParametersDefaults();
}

DriverTemplate::~DriverTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType DriverTemplate::componentType() const
{
	return ComponentType(Driver::get_class_type().get_name());
}

ComponentFamilyType DriverTemplate::familyType() const
{
	return ComponentFamilyType("Control");
}

SMARTPTR(Component)DriverTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Driver) newDriver = new Driver(this);
	newDriver->setComponentId(compId);
	if (not newDriver->initialize())
	{
		return NULL;
	}
	return newDriver.p();
}

void DriverTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("forward", "enabled"));
	mParameterTable.insert(ParameterNameValue("backward", "enabled"));
	mParameterTable.insert(ParameterNameValue("head_limit", "false@0.0"));
	mParameterTable.insert(ParameterNameValue("head_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("head_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("pitch_limit", "false@0.0"));
	mParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
	mParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
	mParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("up", "enabled"));
	mParameterTable.insert(ParameterNameValue("down", "enabled"));
	mParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_h", "false"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_p", "false"));
	mParameterTable.insert(ParameterNameValue("speed_key", "shift"));
	mParameterTable.insert(ParameterNameValue("inverted_translation", "false"));
	mParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
	mParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
	mParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
	mParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
	mParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
	mParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
	mParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
	mParameterTable.insert(ParameterNameValue("stop_threshold", "0.01"));
	mParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
	mParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
	mParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
}

//TypedObject semantics: hardcoded
TypeHandle DriverTemplate::_type_handle;

} // namespace ely
