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

using namespace ely;

Driver::Driver()
{
	// TODO Auto-generated constructor stub
}

Driver::Driver(SMARTPTR(DriverTemplate)tmpl) :
mTrue(true), mFalse(false), mIsEnabled(false)
{
	CHECKEXISTENCE(GameControlManager::GetSingletonPtr(),
			"Driver::Driver: invalid GameControlManager")

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
	// http://www.panda3d.org/forums/viewtopic.php?t=6049)
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

	//enable only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

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
		GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
		//reset mouse to start position
		win->move_pointer(0, mCentX, mCentY);
	}

	//add to the control manager update
	GameControlManager::GetSingletonPtr()->addToControlUpdate(this);
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

	//disable only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}

	//check if control manager exists
	if (GameControlManager::GetSingletonPtr())
	{
		//remove from control manager update
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
	mEnabled = (
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
	if (not (mSpeedKey == "control" or mSpeedKey == "alt"
			or mSpeedKey == "shift"))
	{
		mSpeedKey = "shift";
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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//setup event callbacks if any
	setupEvents();
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

bool Driver::isForwardEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mForward;
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

bool Driver::isBackwardEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mBackward;
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

bool Driver::isStrafeLeftEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mStrafeLeft;
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

bool Driver::isStrafeRightEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mStrafeRight;
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

bool Driver::isUpEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mUp;
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

bool Driver::isDownEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mDown;
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

bool Driver::isRollLeftEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mRollLeft;
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

bool Driver::isRollRightEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mRollRight;
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

bool Driver::isMouseMoveEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mMouseMove;
}

void Driver::setLinearSpeed(LVector3f linearSpeed)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mSpeedActualXYZ = linearSpeed;
}

LVector3f Driver::getLinearSpeed()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mSpeedActualXYZ;
}

void Driver::setAngularSpeed(float angularSpeed)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mSpeedActualH = angularSpeed;
}

float Driver::getAngularSpeed()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mSpeedActualH;
}

void Driver::setFastFactor(float factor)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mFastFactor = factor;
}

float Driver::getFastFactor()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mFastFactor;
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

