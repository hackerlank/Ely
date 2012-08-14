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

Driver::Driver(DriverTemplate* tmpl) :
		mTmpl(tmpl), mForward(this, false), mBackward(this, false), mStrafeLeft(
				this, false), mStrafeRight(this, false), mUp(this, false), mDown(
				this, false), mRollLeft(this, false), mRollRight(this, false), mTrue(
				true), mFalse(false), mIsEnabled(false)
{
	//initialized by template:
	//mInvertedKeyboard, mInvertedMouse, mMouseEnabledH, mMouseEnabledP, mEnabled

	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	mCentX = win->get_properties().get_x_size() / 2;
	mCentY = win->get_properties().get_y_size() / 2;
}

Driver::~Driver()
{
	disable();
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
	//Unregister the handlers
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mBackward);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mDown);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mForward);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mStrafeLeft);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mStrafeRight);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mRollLeft);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mRollRight);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mUp);
	//
	mIsEnabled = not mIsEnabled;
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

	//Register the handlers
	//helper variables
	std::string speedKey = mSpeedKey;
	std::string keyEvent, speedKeyEvent, upKeyEvent;
	//backward event keys (e.g. "s", "shift-s", "s-up")
	keyEvent = mBackwardKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "backward",
				&Driver::setControlTrue, (void*) &this->mBackward);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-backward",
				&Driver::setControlTrue, (void*) &this->mBackward);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "backward-up",
				&Driver::setControlFalse, (void*) &this->mBackward);
	}
	//down event keys (e.g. "f", "shift-f", "f-up")
	keyEvent = mDownKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "down",
				&Driver::setControlTrue, (void*) &this->mDown);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-down",
				&Driver::setControlTrue, (void*) &this->mDown);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "down-up",
				&Driver::setControlFalse, (void*) &this->mDown);
	}
	//forward event keys (e.g. "w", "shift-w", "w-up")
	keyEvent = mForwardKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "forward",
				&Driver::setControlTrue, (void*) &this->mForward);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-forward",
				&Driver::setControlTrue, (void*) &this->mForward);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "forward-up",
				&Driver::setControlFalse, (void*) &this->mForward);
	}
	//strafeLeft event keys (e.g. "q", "shift-q", "q-up")
	keyEvent = mStrafeLeftKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "strafeLeft",
				&Driver::setControlTrue, (void*) &this->mStrafeLeft);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeLeft",
				&Driver::setControlTrue, (void*) &this->mStrafeLeft);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeLeft-up",
				&Driver::setControlFalse, (void*) &this->mStrafeLeft);
	}
	//strafeRight event keys (e.g. "e", "shift-e", "e-up")
	keyEvent = mStrafeRightKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "strafeRight",
				&Driver::setControlTrue, (void*) &this->mStrafeRight);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeRight",
				&Driver::setControlTrue, (void*) &this->mStrafeRight);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeRight-up",
				&Driver::setControlFalse, (void*) &this->mStrafeRight);
	}
	//rollLeft event keys (e.g. "a", "shift-a", "a-up")
	keyEvent = mRollLeftKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "rollLeft",
				&Driver::setControlTrue, (void*) &this->mRollLeft);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollLeft",
				&Driver::setControlTrue, (void*) &this->mRollLeft);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "rollLeft-up",
				&Driver::setControlFalse, (void*) &this->mRollLeft);
	}
	//rollRight event keys (e.g. "d", "shift-d", "d-up")
	keyEvent = mRollRightKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "rollRight",
				&Driver::setControlTrue, (void*) &this->mRollRight);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollRight",
				&Driver::setControlTrue, (void*) &this->mRollRight);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "rollRight-up",
				&Driver::setControlFalse, (void*) &this->mRollRight);
	}
	//up event keys (e.g. "r", "shift-r", "r-up")
	keyEvent = mUpKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "up",
				&Driver::setControlTrue, (void*) &this->mUp);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-up",
				&Driver::setControlTrue, (void*) &this->mUp);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "up-up",
				&Driver::setControlFalse, (void*) &this->mUp);
	}
	//speedKey events (e.g. "shift", "shift-up")
	upKeyEvent = speedKey + "-up";
	mTmpl->pandaFramework()->define_key(speedKey, "speedKey",
			&Driver::setSpeedFast, (void*) this);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "speedKey-up",
			&Driver::setSpeed, (void*) this);
	//
	mIsEnabled = not mIsEnabled;
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
	mBackwardKey = mTmpl->parameter(std::string("backward"));
	//down key
	mDownKey = mTmpl->parameter(std::string("down"));
	//forward key
	mForwardKey = mTmpl->parameter(std::string("forward"));
	//strafeLeft key
	mStrafeLeftKey = mTmpl->parameter(std::string("strafe_left"));
	//strafeRight key
	mStrafeRightKey = mTmpl->parameter(std::string("strafe_right"));
	//rollLeft key
	mRollLeftKey = mTmpl->parameter(std::string("roll_left"));
	//rollRight key
	mRollRightKey = mTmpl->parameter(std::string("roll_right"));
	//up key
	mUpKey = mTmpl->parameter(std::string("up"));
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

void Driver::setControlTrue(const Event* event, void* data)
{
	ThisBool* thisBool = (ThisBool*) data;
	//lock (guard) the mutex
	HOLDMUTEX(thisBool->ptr->mMutex)

	thisBool->value = true;
}

void Driver::setControlFalse(const Event* event, void* data)
{
	ThisBool* thisBool = (ThisBool*) data;
	//lock (guard) the mutex
	HOLDMUTEX(thisBool->ptr->mMutex)

	thisBool->value = false;
}

void Driver::setSpeed(const Event* event, void* data)
{
	Driver* _this = (Driver*) data;
	//lock (guard) the mutex
	HOLDMUTEX(_this->mMutex)

	_this->mSpeedActual = _this->mSpeed;
}

void Driver::setSpeedFast(const Event* event, void* data)
{
	Driver* _this = (Driver*) data;
	//lock (guard) the mutex
	HOLDMUTEX(_this->mMutex)

	_this->mSpeedActual = _this->mSpeed * _this->mFastFactor;
}

bool Driver::isEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsEnabled;
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
	if (mMouseEnabledH or mMouseEnabledP)
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
		CPT(TransformState) newTransform = mActualTransform->compose(
				TransformState::make_identity()->set_pos(
						LVecBase3(newX, newY, newZ)))->set_hpr(
				LVecBase3(newH, newP, newR));
		ownerNodePath.set_transform(newTransform);
		mActualTransform = newTransform;
	}
#endif
}

//TypedObject semantics: hardcoded
TypeHandle Driver::_type_handle;

