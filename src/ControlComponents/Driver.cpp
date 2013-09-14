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
#include "ControlComponents/DriverTemplate.h"
#include "ObjectModel/Object.h"
#include "Game/GameControlManager.h"

namespace ely
{

Driver::Driver()
{
	// TODO Auto-generated constructor stub
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
			mTmpl->parameter(std::string("enabled")) == std::string("true") ?
					true : false);
	//inverted setting
	mSignOfTranslation = (
			mTmpl->parameter(std::string("inverted_translation"))
					== std::string("true") ? -1 : 1);
	mSignOfMouse = (
			mTmpl->parameter(std::string("inverted_rotation"))
					== std::string("true") ? -1 : 1);
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
					== std::string("enabled") ? true : false);
	//down key
	mDownKey = (
			mTmpl->parameter(std::string("down")) == std::string("enabled") ?
					true : false);
	//forward key
	mForwardKey = (
			mTmpl->parameter(std::string("forward")) == std::string("enabled") ?
					true : false);
	//strafeLeft key
	mStrafeLeftKey = (
			mTmpl->parameter(std::string("strafe_left"))
					== std::string("enabled") ? true : false);
	//strafeRight key
	mStrafeRightKey = (
			mTmpl->parameter(std::string("strafe_right"))
					== std::string("enabled") ? true : false);
	//rollLeft key
	mRollLeftKey = (
			mTmpl->parameter(std::string("roll_left"))
					== std::string("enabled") ? true : false);
	//rollRight key
	mRollRightKey = (
			mTmpl->parameter(std::string("roll_right"))
					== std::string("enabled") ? true : false);
	//up key
	mUpKey = (
			mTmpl->parameter(std::string("up")) == std::string("enabled") ?
					true : false);
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

	//set sensitivity parameters
	float speed = (float) strtof(
			mTmpl->parameter(std::string("max_linear_speed")).c_str(), NULL);
	mMaxSpeedXYZ = LVecBase3f(speed, speed, speed);
	mMaxSpeedSquaredXYZ = LVector3f(mMaxSpeedXYZ.get_x() * mMaxSpeedXYZ.get_x(),
			mMaxSpeedXYZ.get_y() * mMaxSpeedXYZ.get_y(),
			mMaxSpeedXYZ.get_z() * mMaxSpeedXYZ.get_z());
	mMaxSpeedH = (float) strtof(
			mTmpl->parameter(std::string("max_angular_speed")).c_str(), NULL);
	mMaxSpeedSquaredH = mMaxSpeedH * mMaxSpeedH;
	//
	float accel = (float) strtof(
			mTmpl->parameter(std::string("linear_accel")).c_str(), NULL);
	mAccelXYZ = LVecBase3f(accel, accel, accel);
	mAccelH = (float) strtof(
			mTmpl->parameter(std::string("angular_accel")).c_str(), NULL);
	//reset speeds
	mActualSpeedXYZ = LVector3f::zero();
	mActualSpeedH = 0.0;
	//
	float linearFriction = 1
			- (float) strtof(
					mTmpl->parameter(std::string("linear_friction")).c_str(),
					NULL);
	mFrictionXYZ = LVector3f(linearFriction, linearFriction, linearFriction);
	if ((mFrictionXYZ.get_x() < 0.0) or (mFrictionXYZ.get_x() > 1.0))
	{
		mFrictionXYZ.set_x(0.1);
	}
	if ((mFrictionXYZ.get_y() < 0.0) or (mFrictionXYZ.get_y() > 1.0))
	{
		mFrictionXYZ.set_y(0.1);
	}
	if ((mFrictionXYZ.get_z() < 0.0) or (mFrictionXYZ.get_z() > 1.0))
	{
		mFrictionXYZ.set_z(0.1);
	}
	mFrictionH = 1
			- (float) strtof(
					mTmpl->parameter(std::string("angular_friction")).c_str(),
					NULL);
	if ((mFrictionH < 0.0) or (mFrictionH > 1.0))
	{
		mFrictionH = 0.1;
	}
	mStopThreshold = (float) strtof(
			mTmpl->parameter(std::string("stop_threshold")).c_str(),
			NULL);
	if ((mStopThreshold < 0.0) or (mStopThreshold > 1.0))
	{
		mStopThreshold = 0.01;
	}

	mFastFactor = (float) strtof(
			mTmpl->parameter(std::string("fast_factor")).c_str(), NULL);
	mSensX = (float) strtof(mTmpl->parameter(std::string("sens_x")).c_str(),
	NULL);
	mSensY = (float) strtof(mTmpl->parameter(std::string("sens_y")).c_str(),
	NULL);
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
	HOLD_MUTEX(mMutex)

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
		HOLD_MUTEX(mMutex)

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
	HOLD_MUTEX(mMutex)

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
	HOLD_MUTEX(mMutex)

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
	ownerNodePath.set_h(ownerNodePath.get_h()
			+ mActualSpeedH * dt * mSignOfMouse);

	//update speeds
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
					mActualSpeedXYZ.get_y() * mFrictionXYZ.get_y());
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
					mActualSpeedXYZ.get_x() * mFrictionXYZ.get_x());
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
					mActualSpeedXYZ.get_z() * mFrictionXYZ.get_z());
		}
	}
	//rotation
	if (mRollLeft and (not mRollRight))
	{
		if(mAccelH != 0)
		{
			//accelerate
			mActualSpeedH += mAccelH * dt;
			if (mActualSpeedH > mMaxSpeedH)
			{
				//limit speed
				mActualSpeedH = mMaxSpeedH;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = mMaxSpeedH;
		}
	}
	else if (mRollRight and (not mRollLeft))
	{
		if(mAccelH != 0)
		{
			//accelerate
			mActualSpeedH -= mAccelH * dt;
			if (mActualSpeedH < -mMaxSpeedH)
			{
				//limit speed
				mActualSpeedH = -mMaxSpeedH;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = -mMaxSpeedH;
		}
	}
	else if (mActualSpeedH != 0.0)
	{
		if (mActualSpeedH * mActualSpeedH <
				mMaxSpeedSquaredH * mStopThreshold)
		{
			//stop
			mActualSpeedH = 0.0;
		}
		else
		{
			//decelerate
			mActualSpeedH *= mFrictionH;
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle Driver::_type_handle;

} // namespace ely
