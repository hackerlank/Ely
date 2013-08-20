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
	CHECK_EXISTENCE(GameControlManager::GetSingletonPtr(),
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
	mInvertedKeyboard = (
			mTmpl->parameter(std::string("inverted_keyboard"))
					== std::string("true") ? true : false);
	mInvertedMouse = (
			mTmpl->parameter(std::string("inverted_mouse"))
					== std::string("true") ? true : false);
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
	if (not (mSpeedKey == std::string("control") or mSpeedKey == std::string("alt")
			or mSpeedKey == std::string("shift")))
	{
		mSpeedKey = std::string("shift");
	}

	//set sensitivity parameters
	float speed = (float) strtof(
			mTmpl->parameter(std::string("linear_speed")).c_str(), NULL);
	mSpeedActualXYZ = LVecBase3f(speed, speed, speed);
	mSpeedActualH = (float) strtof(
			mTmpl->parameter(std::string("angular_speed")).c_str(), NULL);
	mFastFactor = (float) strtof(
			mTmpl->parameter(std::string("fast_factor")).c_str(), NULL);
	mMovSens = (float) strtof(mTmpl->parameter(std::string("mov_sens")).c_str(), NULL);
	mRollSens = (float) strtof(
			mTmpl->parameter(std::string("roll_sens")).c_str(), NULL);
	mSensX = (float) strtof(mTmpl->parameter(std::string("sens_x")).c_str(), NULL);
	mSensY = (float) strtof(mTmpl->parameter(std::string("sens_y")).c_str(), NULL);
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
	if (mEnabled)
	{
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
	}
	//
	reset();
}

void Driver::onAddToSceneSetup()
{
	//enable the component (if requested)
	if (mStartEnabled)
	{
		enable();
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

void Driver::enable()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	//if enabled return
	RETURN_ON_COND(mEnabled,)

#ifdef ELY_THREAD
	//initialize the current transform
	mActualTransform = mOwnerObject->getNodePath().get_transform();
#endif

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

void Driver::disable()
{
	{
		//lock (guard) the mutex
		HOLD_MUTEX(mMutex)

		//if disabling return
		RETURN_ON_ASYNC_COND(mDisabling,)

		//if not enabled return
		RETURN_ON_COND(not mEnabled,)

#ifdef ELY_THREAD
		mDisabling = true;
#endif
	}

	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);

	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	//unregister event callbacks if any
	unregisterEventCallbacks();

	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		SMARTPTR(GraphicsWindow) win =
				mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}
	//
#ifdef ELY_THREAD
	mDisabling = false;
#endif
	mEnabled = false;
}

void Driver::update(void* data)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));
	bool modified = false;

	NodePath ownerNodePath = mOwnerObject->getNodePath();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	int signOfKeyboard = (mInvertedKeyboard ? -1 : 1);

#ifdef ELY_THREAD
	float newX = 0, newY = 0, newZ = 0;
	float newH = 0, newP = 0, newR = 0;
#endif
	//handle mouse
	if (mMouseMove and (mMouseEnabledH or mMouseEnabledP))
	{
		GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
		MouseData md = win->get_pointer(0);
		int signOfMouse = (mInvertedMouse ? -1 : 1);
		float x = md.get_x();
		float y = md.get_y();

		if (win->move_pointer(0, mCentX, mCentY))
		{
			if (mMouseEnabledH)
			{
#ifdef ELY_THREAD
				newH -= (x - mCentX) * mSensX * signOfMouse;
#else
				ownerNodePath.set_h(
						ownerNodePath.get_h()
						- (x - mCentX) * mSensX * signOfMouse);
#endif
				modified = true;
			}
			if (mMouseEnabledP)
			{
#ifdef ELY_THREAD
				newP -= (y - mCentY) * mSensY * signOfMouse;
#else
				ownerNodePath.set_p(
						ownerNodePath.get_p()
						- (y - mCentY) * mSensY * signOfMouse);
#endif
				modified = true;
			}
		}
		//if mMouseMoveKey is true we are controlling mouse movements
		//so we need to reset mMouseMove to false
		if (mMouseMoveKey)
		{
			mMouseMove = false;
		}
	}
	//handle keys:
	if (mForward)
	{
#ifdef ELY_THREAD
		newY -= mMovSens * mSpeedActualXYZ.get_y() * dt * signOfKeyboard;
#else
		ownerNodePath.set_y(ownerNodePath,
				-mMovSens * mSpeedActualXYZ.get_y() * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mBackward)
	{
#ifdef ELY_THREAD
		newY += mMovSens * mSpeedActualXYZ.get_y() * dt * signOfKeyboard;
#else
		ownerNodePath.set_y(ownerNodePath,
				+mMovSens * mSpeedActualXYZ.get_y() * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mStrafeLeft)
	{
#ifdef ELY_THREAD
		newX += mMovSens * mSpeedActualXYZ.get_x() * dt * signOfKeyboard;
#else
		ownerNodePath.set_x(ownerNodePath,
				+mMovSens * mSpeedActualXYZ.get_x() * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mStrafeRight)
	{
#ifdef ELY_THREAD
		newX -= mMovSens * mSpeedActualXYZ.get_x() * dt * signOfKeyboard;
#else
		ownerNodePath.set_x(ownerNodePath,
				-mMovSens * mSpeedActualXYZ.get_x() * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mUp)
	{
#ifdef ELY_THREAD
		newZ += mMovSens * mSpeedActualXYZ.get_z() * dt;
#else
		ownerNodePath.set_z(ownerNodePath, +mMovSens * mSpeedActualXYZ.get_z() * dt);
#endif
		modified = true;
	}
	if (mDown)
	{
#ifdef ELY_THREAD
		newZ -= mMovSens * mSpeedActualXYZ.get_z() * dt;
#else
		ownerNodePath.set_z(ownerNodePath, -mMovSens * mSpeedActualXYZ.get_z() * dt);
#endif
		modified = true;
	}
	if (mRollLeft)
	{
#ifdef ELY_THREAD
		newH += mRollSens * mSpeedActualH * dt * signOfKeyboard;
#else
		ownerNodePath.set_h(
				ownerNodePath.get_h()
				+ mRollSens * mSpeedActualH * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mRollRight)
	{
#ifdef ELY_THREAD
		newH -= mRollSens * mSpeedActualH * dt * signOfKeyboard;
#else
		ownerNodePath.set_h(
				ownerNodePath.get_h()
				- mRollSens * mSpeedActualH * dt * signOfKeyboard);
#endif
		modified = true;
	}
#ifdef ELY_THREAD
	if (modified)
	{
		newH += mActualTransform->get_hpr().get_x();
		newP += mActualTransform->get_hpr().get_y();
		newR += mActualTransform->get_hpr().get_z();
		CSMARTPTR(TransformState)newTransform = mActualTransform->compose(
				TransformState::make_identity()->set_pos(
						LVecBase3(newX, newY, newZ)))->set_hpr(
				LVecBase3(newH, newP, newR)).p();
		ownerNodePath.set_transform(newTransform);
		mActualTransform = newTransform;
	}
#endif
}

//TypedObject semantics: hardcoded
TypeHandle Driver::_type_handle;

} // namespace ely
