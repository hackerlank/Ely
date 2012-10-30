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
 * \author marco
 */

#include "ControlComponents/Driver.h"
#include "ControlComponents/DriverTemplate.h"

Driver::Driver()
{
	// TODO Auto-generated constructor stub
}

Driver::Driver(SMARTPTR(DriverTemplate) tmpl) :
		mTrue(true), mFalse(false), mIsEnabled(false)
{
	mTmpl = tmpl;
	//initialized by template:
	//mInvertedKeyboard, mInvertedMouse, mMouseEnabledH, mMouseEnabledP, mEnabled
	mForward = false;
	mBackward = false;
	mStrafeLeft = false;
	mStrafeRight = false;
	mUp = false;
	mDown = false;
	mRollLeft = false;
	mRollRight = false;
	//by default we consider mouse moved on every update, because
	//we want mouse poll by default; this can be changed by calling
	//the enabler (for example by an handler responding to mouse-move
	//event if it is possible. See: http://www.panda3d.org/forums/viewtopic.php?t=9326
	//	http://www.panda3d.org/forums/viewtopic.php?t=6049)
	mMouseMove = true;
	//
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	mCentX = win->get_properties().get_x_size() / 2;
	mCentY = win->get_properties().get_y_size() / 2;
}

Driver::~Driver()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	disable();
}

const ComponentFamilyType Driver::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Driver::componentType() const
{
	return mTmpl->componentType();
}

void Driver::enable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mIsEnabled or (not mOwnerObject))
	{
		return;
	}

#ifdef ELY_THREAD
	//initialize the actual transform
	mActualTransform = mOwnerObject->getNodePath().get_transform();
#endif

	//hide mouse cursor
	WindowProperties props;
	props.set_cursor_hidden(true);
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	win->request_properties(props);
	//reset mouse to start position
	win->move_pointer(0, mCentX, mCentY);

	//Add to the control manager update
	//first check if game audio manager exists
	if (GameControlManager::GetSingletonPtr())
	{
		GameControlManager::GetSingletonPtr()->addToControlUpdate(this);
	}
	//
	mIsEnabled = not mIsEnabled;
	//register event callbacks if any
	registerEventCallbacks();
}

void Driver::disable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if ((not mIsEnabled) or (not mOwnerObject))
	{
		return;
	}
	//show mouse cursor
	WindowProperties props;
	props.set_cursor_hidden(false);
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	win->request_properties(props);

	//Remove from the control manager update
	//first check if game audio manager exists
	if (GameControlManager::GetSingletonPtr())
	{
		GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);
	}
	//
	mIsEnabled = not mIsEnabled;
	//unregister event callbacks if any
	unregisterEventCallbacks();
}

bool Driver::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//get settings from template
	//enabling setting
	mEnabled =
			(mTmpl->parameter(std::string("enabled")) == std::string("true") ? true :
					false);
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
	mDownKey =
			(mTmpl->parameter(std::string("down")) == std::string("enabled") ? true :
					false);
	//forward key
	mForwardKey =
			(mTmpl->parameter(std::string("forward"))
					== std::string("enabled") ? true : false);
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
	mUpKey =
			(mTmpl->parameter(std::string("up")) == std::string("enabled") ? true :
					false);
	//mouseMove key
	mMouseMoveKey = (
			mTmpl->parameter(std::string("mouse_move"))
					== std::string("enabled") ? true : false);
	//speedKey
	mSpeedKey = mTmpl->parameter(std::string("speed_key"));
	if (not (mSpeedKey == "control" or mSpeedKey == "alt"
			or mSpeedKey == "shift"))
	{
		mSpeedKey = "shift";
	}

	//set sensitivity parameters
	mSpeed = (float) atof(mTmpl->parameter(std::string("speed")).c_str());
	mFastFactor = (float) atof(
			mTmpl->parameter(std::string("fast_factor")).c_str());
	mSpeedActual = mSpeed;
	mMovSens = (float) atof(mTmpl->parameter(std::string("mov_sens")).c_str());
	mRollSens = (float) atof(
			mTmpl->parameter(std::string("roll_sens")).c_str());
	mSensX = (float) atof(mTmpl->parameter(std::string("sens_x")).c_str());
	mSensY = (float) atof(mTmpl->parameter(std::string("sens_y")).c_str());
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void Driver::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//enable the component
	if (mEnabled)
	{
		enable();
	}
}

bool Driver::isEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsEnabled;
}

void Driver::enableForward(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mForwardKey)
	{
		mForward = enable;
	}
}

void Driver::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

void Driver::enableStrafeLeft(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

void Driver::enableStrafeRight(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

void Driver::enableUp(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mUpKey)
	{
		mUp = enable;
	}
}

void Driver::enableDown(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mDownKey)
	{
		mDown = enable;
	}
}

void Driver::enableRollLeft(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mRollLeftKey)
	{
		mRollLeft = enable;
	}
}

void Driver::enableRollRight(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mRollRightKey)
	{
		mRollRight = enable;
	}
}

void Driver::enableMouseMove(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mMouseMoveKey)
	{
		mMouseMove = enable;
	}
}

void Driver::setSpeed()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mSpeedActual = mSpeed;
}

void Driver::setSpeedFast()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mSpeedActual = mSpeed * mFastFactor;
}

void Driver::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
		newY -= mMovSens * mSpeedActual * dt * signOfKeyboard;
#else
		ownerNodePath.set_y(ownerNodePath,
				-mMovSens * mSpeedActual * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mBackward)
	{
#ifdef ELY_THREAD
		newY += mMovSens * mSpeedActual * dt * signOfKeyboard;
#else
		ownerNodePath.set_y(ownerNodePath,
				+mMovSens * mSpeedActual * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mStrafeLeft)
	{
#ifdef ELY_THREAD
		newX += mMovSens * mSpeedActual * dt * signOfKeyboard;
#else
		ownerNodePath.set_x(ownerNodePath,
				+mMovSens * mSpeedActual * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mStrafeRight)
	{
#ifdef ELY_THREAD
		newX -= mMovSens * mSpeedActual * dt * signOfKeyboard;
#else
		ownerNodePath.set_x(ownerNodePath,
				-mMovSens * mSpeedActual * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mUp)
	{
#ifdef ELY_THREAD
		newZ += mMovSens * mSpeedActual * dt;
#else
		ownerNodePath.set_z(ownerNodePath, +mMovSens * mSpeedActual * dt);
#endif
		modified = true;
	}
	if (mDown)
	{
#ifdef ELY_THREAD
		newZ -= mMovSens * mSpeedActual * dt;
#else
		ownerNodePath.set_z(ownerNodePath, -mMovSens * mSpeedActual * dt);
#endif
		modified = true;
	}
	if (mRollLeft)
	{
#ifdef ELY_THREAD
		newH += mRollSens * mSpeedActual * dt * signOfKeyboard;
#else
		ownerNodePath.set_h(
				ownerNodePath.get_h()
				+ mRollSens * mSpeedActual * dt * signOfKeyboard);
#endif
		modified = true;
	}
	if (mRollRight)
	{
#ifdef ELY_THREAD
		newH -= mRollSens * mSpeedActual * dt * signOfKeyboard;
#else
		ownerNodePath.set_h(
				ownerNodePath.get_h()
				- mRollSens * mSpeedActual * dt * signOfKeyboard);
#endif
		modified = true;
	}
#ifdef ELY_THREAD
	if (modified)
	{
		newH += mActualTransform->get_hpr().get_x();
		newP += mActualTransform->get_hpr().get_y();
		newR += mActualTransform->get_hpr().get_z();
		CSMARTPTR(TransformState) newTransform = mActualTransform->compose(
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

