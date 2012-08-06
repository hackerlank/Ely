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
 * \file /Ely/src/InputComponents/ControlByEvent.cpp
 *
 * \date 31/mag/2012 (16:42:14)
 * \author marco
 */

#include "InputComponents/ControlByEvent.h"
#include "InputComponents/ControlByEventTemplate.h"

ControlByEvent::ControlByEvent()
{
	// TODO Auto-generated constructor stub
}

ControlByEvent::ControlByEvent(ControlByEventTemplate* tmpl) :
		mTmpl(tmpl), mForward(false), mBackward(false), mStrafeLeft(false), mStrafeRight(
				false), mUp(false), mDown(false), mRollLeft(false), mRollRight(
				false), mTrue(true), mFalse(false), mIsEnabled(false)
{
	//initialized by template:
	//mInvertedKeyboard, mInvertedMouse, mMouseEnabledH, mMouseEnabledP, mEnabled

	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	mCentX = win->get_properties().get_x_size() / 2;
	mCentY = win->get_properties().get_y_size() / 2;
}

ControlByEvent::~ControlByEvent()
{
	disable();
}

void ControlByEvent::disable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not mIsEnabled)
	{
		return;
	}
	//show mouse cursor
	WindowProperties props;
	props.set_cursor_hidden(false);
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	win->request_properties(props);

	//Remove from the input manager update
	//first check if game audio manager exists
	if (GameInputManager::GetSingletonPtr())
	{
		GameInputManager::GetSingletonPtr()->removeFromInputUpdate(this);
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

const ComponentFamilyType ControlByEvent::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType ControlByEvent::componentType() const
{
	return mTmpl->componentType();
}

void ControlByEvent::enable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mIsEnabled)
	{
		return;
	}

	//hide mouse cursor
	WindowProperties props;
	props.set_cursor_hidden(true);
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	win->request_properties(props);
	//reset mouse to start position
	win->move_pointer(0, mCentX, mCentY);

	//Add to the input manager update
	//first check if game audio manager exists
	if (GameInputManager::GetSingletonPtr())
	{
		GameInputManager::GetSingletonPtr()->addToInputUpdate(this);
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
				&ControlByEvent::setControlTrue, (void*) &this->mBackward);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-backward",
				&ControlByEvent::setControlTrue, (void*) &this->mBackward);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "backward-up",
				&ControlByEvent::setControlFalse, (void*) &this->mBackward);
	}
	//down event keys (e.g. "f", "shift-f", "f-up")
	keyEvent = mDownKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "down",
				&ControlByEvent::setControlTrue, (void*) &this->mDown);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-down",
				&ControlByEvent::setControlTrue, (void*) &this->mDown);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "down-up",
				&ControlByEvent::setControlFalse, (void*) &this->mDown);
	}
	//forward event keys (e.g. "w", "shift-w", "w-up")
	keyEvent = mForwardKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "forward",
				&ControlByEvent::setControlTrue, (void*) &this->mForward);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-forward",
				&ControlByEvent::setControlTrue, (void*) &this->mForward);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "forward-up",
				&ControlByEvent::setControlFalse, (void*) &this->mForward);
	}
	//strafeLeft event keys (e.g. "q", "shift-q", "q-up")
	keyEvent = mStrafeLeftKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "strafeLeft",
				&ControlByEvent::setControlTrue, (void*) &this->mStrafeLeft);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeLeft",
				&ControlByEvent::setControlTrue, (void*) &this->mStrafeLeft);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeLeft-up",
				&ControlByEvent::setControlFalse, (void*) &this->mStrafeLeft);
	}
	//strafeRight event keys (e.g. "e", "shift-e", "e-up")
	keyEvent = mStrafeRightKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "strafeRight",
				&ControlByEvent::setControlTrue, (void*) &this->mStrafeRight);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeRight",
				&ControlByEvent::setControlTrue, (void*) &this->mStrafeRight);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeRight-up",
				&ControlByEvent::setControlFalse, (void*) &this->mStrafeRight);
	}
	//rollLeft event keys (e.g. "a", "shift-a", "a-up")
	keyEvent = mRollLeftKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "rollLeft",
				&ControlByEvent::setControlTrue, (void*) &this->mRollLeft);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollLeft",
				&ControlByEvent::setControlTrue, (void*) &this->mRollLeft);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "rollLeft-up",
				&ControlByEvent::setControlFalse, (void*) &this->mRollLeft);
	}
	//rollRight event keys (e.g. "d", "shift-d", "d-up")
	keyEvent = mRollRightKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "rollRight",
				&ControlByEvent::setControlTrue, (void*) &this->mRollRight);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollRight",
				&ControlByEvent::setControlTrue, (void*) &this->mRollRight);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "rollRight-up",
				&ControlByEvent::setControlFalse, (void*) &this->mRollRight);
	}
	//up event keys (e.g. "r", "shift-r", "r-up")
	keyEvent = mUpKey;
	if (keyEvent != "")
	{
		speedKeyEvent = speedKey + "-" + keyEvent;
		upKeyEvent = keyEvent + "-up";
		mTmpl->pandaFramework()->define_key(keyEvent, "up",
				&ControlByEvent::setControlTrue, (void*) &this->mUp);
		mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-up",
				&ControlByEvent::setControlTrue, (void*) &this->mUp);
		mTmpl->pandaFramework()->define_key(upKeyEvent, "up-up",
				&ControlByEvent::setControlFalse, (void*) &this->mUp);
	}
	//speedKey events (e.g. "shift", "shift-up")
	upKeyEvent = speedKey + "-up";
	mTmpl->pandaFramework()->define_key(speedKey, "speedKey",
			&ControlByEvent::setSpeedFast, (void*) this);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "speedKey-up",
			&ControlByEvent::setSpeed, (void*) this);
	//
	mIsEnabled = not mIsEnabled;
}

bool ControlByEvent::initialize()
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

void ControlByEvent::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//enable the component
	if (mEnabled)
	{
		enable();
	}
}

void ControlByEvent::setControlTrue(const Event* event, void* data)
{
	bool* boolPtr = (bool*) data;
	*boolPtr = true;
}

void ControlByEvent::setControlFalse(const Event* event, void* data)
{
	bool* boolPtr = (bool*) data;
	*boolPtr = false;
}

void ControlByEvent::setSpeed(const Event* event, void* data)
{
	ControlByEvent* _this = (ControlByEvent*) data;
	_this->mSpeedActual = _this->mSpeed;
}

void ControlByEvent::setSpeedFast(const Event* event, void* data)
{
	ControlByEvent* _this = (ControlByEvent*) data;
	_this->mSpeedActual = _this->mSpeed * _this->mFastFactor;
}

bool ControlByEvent::isEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsEnabled;
}

void ControlByEvent::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

	NodePath ownerNodePath = mOwnerObject->getNodePath();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	int signOfKeyboard = (mInvertedKeyboard ? -1 : 1);

#ifdef ELY_THREAD
	CPT(TransformState) ownerTransform = ownerNodePath.get_transform();
	LPoint3 ownerPos = ownerTransform->get_pos();
	LVecBase3 ownerHpr = ownerTransform->get_hpr();
	float newX = ownerPos.get_X();
	float newY = ownerPos.get_Y();
	float newZ = ownerPos.get_Z();
	float newH = ownerHpr.get_X();
	float newP = ownerHpr.get_Y();








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
				ownerNodePath.set_h(
						ownerNodePath.get_h()
								- (x - mCentX) * mSensX * signOfMouse);
			}
			if (mMouseEnabledP)
			{
				ownerNodePath.set_p(
						ownerNodePath.get_p()
								- (y - mCentY) * mSensY * signOfMouse);
			}
		}
	}

	//handle keys:
	if (mForward)
	{
		ownerNodePath.set_y(ownerNodePath,
				ownerNodePath.get_y(ownerNodePath)
						- mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mBackward)
	{
		ownerNodePath.set_y(ownerNodePath,
				ownerNodePath.get_y(ownerNodePath)
						+ mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mStrafeLeft)
	{
		ownerNodePath.set_x(ownerNodePath,
				ownerNodePath.get_x(ownerNodePath)
						+ mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mStrafeRight)
	{
		ownerNodePath.set_x(ownerNodePath,
				ownerNodePath.get_x(ownerNodePath)
						- mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mUp)
	{
		ownerNodePath.set_z(ownerNodePath,
				ownerNodePath.get_z(ownerNodePath)
						+ mMovSens * mSpeedActual * dt);
	}
	if (mDown)
	{
		ownerNodePath.set_z(ownerNodePath,
				ownerNodePath.get_z(ownerNodePath)
						- mMovSens * mSpeedActual * dt);
	}
	if (mRollLeft)
	{
		ownerNodePath.set_h(ownerNodePath,
				ownerNodePath.get_h(ownerNodePath)
						+ mRollSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mRollRight)
	{
		ownerNodePath.set_h(ownerNodePath,
				ownerNodePath.get_h(ownerNodePath)
						- mRollSens * mSpeedActual * dt * signOfKeyboard);
	}
#else
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
				ownerNodePath.set_h(
						ownerNodePath.get_h()
						- (x - mCentX) * mSensX * signOfMouse);
			}
			if (mMouseEnabledP)
			{
				ownerNodePath.set_p(
						ownerNodePath.get_p()
						- (y - mCentY) * mSensY * signOfMouse);
			}
		}
	}

	//handle keys:
	if (mForward)
	{
		ownerNodePath.set_y(ownerNodePath,
				ownerNodePath.get_y(ownerNodePath)
				- mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mBackward)
	{
		ownerNodePath.set_y(ownerNodePath,
				ownerNodePath.get_y(ownerNodePath)
				+ mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mStrafeLeft)
	{
		ownerNodePath.set_x(ownerNodePath,
				ownerNodePath.get_x(ownerNodePath)
				+ mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mStrafeRight)
	{
		ownerNodePath.set_x(ownerNodePath,
				ownerNodePath.get_x(ownerNodePath)
				- mMovSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mUp)
	{
		ownerNodePath.set_z(ownerNodePath,
				ownerNodePath.get_z(ownerNodePath)
				+ mMovSens * mSpeedActual * dt);
	}
	if (mDown)
	{
		ownerNodePath.set_z(ownerNodePath,
				ownerNodePath.get_z(ownerNodePath)
				- mMovSens * mSpeedActual * dt);
	}
	if (mRollLeft)
	{
		ownerNodePath.set_h(ownerNodePath,
				ownerNodePath.get_h(ownerNodePath)
				+ mRollSens * mSpeedActual * dt * signOfKeyboard);
	}
	if (mRollRight)
	{
		ownerNodePath.set_h(ownerNodePath,
				ownerNodePath.get_h(ownerNodePath)
				- mRollSens * mSpeedActual * dt * signOfKeyboard);
	}
#endif
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEvent::_type_handle;

