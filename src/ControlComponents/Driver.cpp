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
	mSignOfKeyboard = (
			mTmpl->parameter(std::string("inverted_keyboard"))
					== std::string("true") ? -1 : 1);
	mSignOfMouse = (
			mTmpl->parameter(std::string("inverted_mouse"))
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
	mActualSpeedXYZ = LVecBase3f(speed, speed, speed);
	mActualSpeedH = (float) strtof(
			mTmpl->parameter(std::string("max_angular_speed")).c_str(), NULL);
	mMaxSpeedXYZ = speed;
	mMaxSpeedSquaredXYZ = mMaxSpeedXYZ * mMaxSpeedXYZ;
	mMaxSpeedH = mActualSpeedH;
	mMaxSpeedSquaredH = mMaxSpeedH * mMaxSpeedH;
	float accel = (float) strtof(
			mTmpl->parameter(std::string("linear_accel")).c_str(), NULL);
	mActualAccelXYZ = LVecBase3f(accel, accel, accel);
	mActualAccelH = (float) strtof(
			mTmpl->parameter(std::string("angular_accel")).c_str(), NULL);
	mFrictionXYZ = 1 - (float) strtof(
			mTmpl->parameter(std::string("linear_friction")).c_str(), NULL);
	mFrictionH = 1 - (float) strtof(
			mTmpl->parameter(std::string("angular_friction")).c_str(), NULL);
	if ((mFrictionXYZ < 0.0) or (mFrictionXYZ > 1.0))
	{
		mFrictionXYZ = 0.1;
	}
	if ((mFrictionH < 0.0) or (mFrictionH > 1.0))
	{
		mFrictionH = 0.1;
	}
	//get first movement type...
	std::string movementType = mTmpl->parameter(std::string("movement_type"));
	if (movementType == "dynamic")
	{
		mMovementType = DYNAMIC;
	}
	else
	{
		mMovementType = KINEMATIC;
	}
	//...tweak if not kinematic
	if (mMovementType == DYNAMIC)
	{
		//reset speeds
		mActualSpeedXYZ = LVector3f::zero();
		mActualSpeedH = 0.0;
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
	mEnabled = false;
}

//void Driver::update(void* data)
//{
//	//lock (guard) the mutex
//	HOLD_MUTEX(mMutex)
//
//	float dt = *(reinterpret_cast<float*>(data));
//	bool modified = false;
//
//	NodePath ownerNodePath = mOwnerObject->getNodePath();
//
//#ifdef TESTING
//	dt = 0.016666667; //60 fps
//#endif
//
//	int signOfKeyboard = (mInvertedKeyboard ? -1 : 1);
//
//#ifdef ELY_THREAD
//	float newX = 0, newY = 0, newZ = 0;
//	float newH = 0, newP = 0, newR = 0;
//#endif
//	//handle mouse
//	if (mMouseMove and (mMouseEnabledH or mMouseEnabledP))
//	{
//		GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
//		MouseData md = win->get_pointer(0);
//		int signOfMouse = (mInvertedMouse ? -1 : 1);
//		float x = md.get_x();
//		float y = md.get_y();
//
//		if (win->move_pointer(0, mCentX, mCentY))
//		{
//			if (mMouseEnabledH)
//			{
//#ifdef ELY_THREAD
//				newH -= (x - mCentX) * mSensX * signOfMouse;
//#else
//				ownerNodePath.set_h(
//						ownerNodePath.get_h()
//						- (x - mCentX) * mSensX * signOfMouse);
//#endif
//				modified = true;
//			}
//			if (mMouseEnabledP)
//			{
//#ifdef ELY_THREAD
//				newP -= (y - mCentY) * mSensY * signOfMouse;
//#else
//				ownerNodePath.set_p(
//						ownerNodePath.get_p()
//						- (y - mCentY) * mSensY * signOfMouse);
//#endif
//				modified = true;
//			}
//		}
//		//if mMouseMoveKey is true we are controlling mouse movements
//		//so we need to reset mMouseMove to false
//		if (mMouseMoveKey)
//		{
//			mMouseMove = false;
//		}
//	}
//	//handle keys:
//	if (mForward)
//	{
//#ifdef ELY_THREAD
//		newY -= mMovSens * mActualSpeedXYZ.get_y() * dt * signOfKeyboard;
//#else
//		ownerNodePath.set_y(ownerNodePath,
//				-mMovSens * mActualSpeedXYZ.get_y() * dt * signOfKeyboard);
//#endif
//		modified = true;
//	}
//	if (mBackward)
//	{
//#ifdef ELY_THREAD
//		newY += mMovSens * mActualSpeedXYZ.get_y() * dt * signOfKeyboard;
//#else
//		ownerNodePath.set_y(ownerNodePath,
//				+mMovSens * mActualSpeedXYZ.get_y() * dt * signOfKeyboard);
//#endif
//		modified = true;
//	}
//	if (mStrafeLeft)
//	{
//#ifdef ELY_THREAD
//		newX += mMovSens * mActualSpeedXYZ.get_x() * dt * signOfKeyboard;
//#else
//		ownerNodePath.set_x(ownerNodePath,
//				+mMovSens * mActualSpeedXYZ.get_x() * dt * signOfKeyboard);
//#endif
//		modified = true;
//	}
//	if (mStrafeRight)
//	{
//#ifdef ELY_THREAD
//		newX -= mMovSens * mActualSpeedXYZ.get_x() * dt * signOfKeyboard;
//#else
//		ownerNodePath.set_x(ownerNodePath,
//				-mMovSens * mActualSpeedXYZ.get_x() * dt * signOfKeyboard);
//#endif
//		modified = true;
//	}
//	if (mUp)
//	{
//#ifdef ELY_THREAD
//		newZ += mMovSens * mActualSpeedXYZ.get_z() * dt;
//#else
//		ownerNodePath.set_z(ownerNodePath, +mMovSens * mActualSpeedXYZ.get_z() * dt);
//#endif
//		modified = true;
//	}
//	if (mDown)
//	{
//#ifdef ELY_THREAD
//		newZ -= mMovSens * mActualSpeedXYZ.get_z() * dt;
//#else
//		ownerNodePath.set_z(ownerNodePath, -mMovSens * mActualSpeedXYZ.get_z() * dt);
//#endif
//		modified = true;
//	}
//	if (mRollLeft)
//	{
//#ifdef ELY_THREAD
//		newH += mRollSens * mActualSpeedH * dt * signOfKeyboard;
//#else
//		ownerNodePath.set_h(
//				ownerNodePath.get_h()
//				+ mRollSens * mActualSpeedH * dt * signOfKeyboard);
//#endif
//		modified = true;
//	}
//	if (mRollRight)
//	{
//#ifdef ELY_THREAD
//		newH -= mRollSens * mActualSpeedH * dt * signOfKeyboard;
//#else
//		ownerNodePath.set_h(
//				ownerNodePath.get_h()
//				- mRollSens * mActualSpeedH * dt * signOfKeyboard);
//#endif
//		modified = true;
//	}
//#ifdef ELY_THREAD
//	if (modified)
//	{
//		newH += mActualTransform->get_hpr().get_x();
//		newP += mActualTransform->get_hpr().get_y();
//		newR += mActualTransform->get_hpr().get_z();
//		CSMARTPTR(TransformState)newTransform = mActualTransform->compose(
//				TransformState::make_identity()->set_pos(
//						LVecBase3(newX, newY, newZ)))->set_hpr(
//				LVecBase3(newH, newP, newR)).p();
//		ownerNodePath.set_transform(newTransform);
//		mActualTransform = newTransform;
//	}
//#endif
//}

void Driver::update(void* data)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

	NodePath ownerNodePath = mOwnerObject->getNodePath();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

#ifdef ELY_THREAD
	float newX = 0, newY = 0, newZ = 0;
	float newH = 0, newP = 0, newR = 0;
#endif
	//handle mouse
	if (mMouseMove and (mMouseEnabledH or mMouseEnabledP))
	{
		GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
		MouseData md = win->get_pointer(0);
		float x = md.get_x();
		float y = md.get_y();

		if (win->move_pointer(0, mCentX, mCentY))
		{
			if (mMouseEnabledH)
			{
#ifdef ELY_THREAD
				newH -= (x - mCentX) * mSensX * mSignOfMouse;
#else
				ownerNodePath.set_h(
						ownerNodePath.get_h()
						- (x - mCentX) * mSensX * mSignOfMouse);
#endif
			}
			if (mMouseEnabledP)
			{
#ifdef ELY_THREAD
				newP -= (y - mCentY) * mSensY * mSignOfMouse;
#else
				ownerNodePath.set_p(
						ownerNodePath.get_p()
						- (y - mCentY) * mSensY * mSignOfMouse);
#endif
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
#ifdef ELY_THREAD
	newY += mActualSpeedXYZ.get_y() * dt * mSignOfKeyboard;
	newX += mActualSpeedXYZ.get_x() * dt * mSignOfKeyboard;
	newZ += mActualSpeedXYZ.get_z() * dt;
	newH += mActualSpeedH * dt * mSignOfKeyboard;
	newH += mActualTransform->get_hpr().get_x();
	newP += mActualTransform->get_hpr().get_y();
	newR += mActualTransform->get_hpr().get_z();
	//make transform
	CSMARTPTR(TransformState)newTransform = mActualTransform->compose(
			TransformState::make_identity()->set_pos(
					LVecBase3(newX, newY, newZ)))->set_hpr(
			LVecBase3(newH, newP, newR)).p();
	ownerNodePath.set_transform(newTransform);
	mActualTransform = newTransform;
#else
	ownerNodePath.set_y(ownerNodePath,
			mActualSpeedXYZ.get_y() * dt * mSignOfKeyboard);
	ownerNodePath.set_x(ownerNodePath,
			mActualSpeedXYZ.get_x() * dt * mSignOfKeyboard);
	ownerNodePath.set_z(ownerNodePath,
			mActualSpeedXYZ.get_z() * dt);
	ownerNodePath.set_h(ownerNodePath.get_h()
			+ mActualSpeedH * dt * mSignOfKeyboard);
#endif

	//update speeds
	bool moveOn = false;
	LVector3f signedAccelXYZ = LVector3f::zero();
	float signedAccelH = 0.0;
	///TODO optimize if flow
	//y axis
	if (mForward or mBackward)
	{
		if (mForward)
		{
			mActualSpeedXYZ.set_y(
				mActualSpeedXYZ.get_y() - mActualAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() < -mMaxSpeedXYZ)
			{
				mActualSpeedXYZ.set_y(-mMaxSpeedXYZ);
			}
		}
		if (mBackward)
		{
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() + mActualAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() > mMaxSpeedXYZ)
			{
				mActualSpeedXYZ.set_y(mMaxSpeedXYZ);
			}
		}
	}
	else if (mActualSpeedXYZ.get_y() != 0.0)
	{
		if (mActualSpeedXYZ.get_y() * mActualSpeedXYZ.get_y() <
				mMaxSpeedSquaredXYZ * 0.01)
		{
			mActualSpeedXYZ.set_y(0.0);
		}
		else
		{
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() * mFrictionXYZ);
		}
	}
	//x axis
	if (mStrafeLeft or mStrafeRight)
	{
		if (mStrafeLeft)
		{
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() + mActualAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() > mMaxSpeedXYZ)
			{
				mActualSpeedXYZ.set_x(mMaxSpeedXYZ);
			}
		}
		if (mStrafeRight)
		{
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() - mActualAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() < -mMaxSpeedXYZ)
			{
				mActualSpeedXYZ.set_x(-mMaxSpeedXYZ);
			}
		}
	}
	else if (mActualSpeedXYZ.get_x() != 0.0)
	{
		if (mActualSpeedXYZ.get_x() * mActualSpeedXYZ.get_x() <
				mMaxSpeedSquaredXYZ * 0.01)
		{
			mActualSpeedXYZ.set_x(0.0);
		}
		else
		{
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() * mFrictionXYZ);
		}
	}
	//z axis
	if (mUp or mDown)
	{
		if (mUp)
		{
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() + mActualAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() > mMaxSpeedXYZ)
			{
				mActualSpeedXYZ.set_z(mMaxSpeedXYZ);
			}
		}
		if (mDown)
		{
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() - mActualAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() < -mMaxSpeedXYZ)
			{
				mActualSpeedXYZ.set_z(-mMaxSpeedXYZ);
			}
		}
	}
	else if (mActualSpeedXYZ.get_z() != 0.0)
	{
		if (mActualSpeedXYZ.get_z() * mActualSpeedXYZ.get_z() <
				mMaxSpeedSquaredXYZ * 0.01)
		{
			mActualSpeedXYZ.set_z(0.0);
		}
		else
		{
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() * mFrictionXYZ);
		}
	}
	//rotation
	if (mRollLeft or mRollRight)
	{
		if (mRollLeft)
		{
			mActualSpeedH += mActualAccelH * dt;
			if (mActualSpeedH > mMaxSpeedH)
			{
				mActualSpeedH = mMaxSpeedH;
			}
		}
		if (mRollRight)
		{
			mActualSpeedH -= mActualAccelH * dt;
			if (mActualSpeedH < -mMaxSpeedH)
			{
				mActualSpeedH = -mMaxSpeedH;
			}
		}
	}
	else if (mActualSpeedH != 0.0)
	{
		if (mActualSpeedH * mActualSpeedH <
				mMaxSpeedSquaredH * 0.01)
		{
			mActualSpeedH = 0.0;
		}
		else
		{
			mActualSpeedH *= mFrictionH;
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle Driver::_type_handle;

} // namespace ely
