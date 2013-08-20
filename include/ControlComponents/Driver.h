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
 * \file /Ely/include/ControlComponents/Driver.h
 *
 * \date 31/mag/2012 (16:42:14)
 * \author consultit
 */

#ifndef CONTROLBYEVENT_H_
#define CONTROLBYEVENT_H_

#include <lvector3.h>
#include <transformState.h>
#include "ObjectModel/Component.h"

namespace ely
{
class DriverTemplate;

/**
 * \brief Component designed for the control of object movement
 * through keyboard/mouse button events and mouse movement.
 *
 * Each basic movement (forward, backward, roll_left, roll_right etc...)
 * can be enabled/disabled through a corresponding "enabler", which in
 * turn set a control key true or false.\n
 * An event handlers could enable/disable movement calling the "enablers".
 * A task updates the position/orientation of the controlled object
 * based on the value of control keys.\n
 * The component can be enabled/disabled as a whole.\n
 * At configuration level (from xml config file), any "enabler" can be
 * enabled/disabled by setting corresponding configuration key
 * to "enabled"/"disabled".\n
 * Mouse movements tracking are special. Since "mouse move" events are not
 * defined by default (they can by using ButtonThrower::set_move_event(),
 * \see: http://www.panda3d.org/forums/viewtopic.php?t=9326
 * 	http://www.panda3d.org/forums/viewtopic.php?t=6049)),
 * mouse movements are polled by default during the "update" task, that is
 * the corresponding "enabler" is disabled.\n
 * The object HEAD (i.e. YAW) and PITCH control through mouse movements
 * can be enabled/disabled separately and if both are disabled (the default)
 * no control through mouse movements.\n
 * All movements (but up and down) can be inverted (default: not inverted).
 *
 * XML Param(s):
 * - "enabled"  			|single|"true"
 * - "forward"  			|single|"enabled"
 * - "backward"  			|single|"enabled"
 * - "roll_left"  			|single|"enabled"
 * - "roll_right"  			|single|"enabled"
 * - "strafe_left"  		|single|"enabled"
 * - "strafe_right"  		|single|"enabled"
 * - "up"  					|single|"enabled"
 * - "down"  				|single|"enabled"
 * - "mouse_move"  			|single|"disabled"
 * - "mouse_enabled_h"  	|single|"false"
 * - "mouse_enabled_p"  	|single|"false"
 * - "speed_key"  			|single|"shift"
 * - "linear_speed"  		|single|"5.0"
 * - "angular_speed"  		|single|"5.0"
 * - "fast_factor"  		|single|"5.0"
 * - "mov_sens"  			|single|"2.0"
 * - "roll_sens"  			|single|"15.0"
 * - "sens_x"  				|single|"0.2"
 * - "sens_y"  				|single|"0.2"
 * - "inverted_keyboard"  	|single|"false"
 * - "inverted_mouse"  		|single|"false"
 */
class Driver: public Component
{
protected:
	friend class DriverTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Driver();
	Driver(SMARTPTR(DriverTemplate)tmpl);
	virtual ~Driver();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an control manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \name Enabling/disabling.
	 * \brief Enables/disables this component.
	 */
	///@{
	void enable();
	void disable();
	bool isEnabled();
	///@}

	/**
	 * \name Control keys' getters/setters.
	 */
	///@{
	void enableForward(bool enable);
	bool isForwardEnabled();
	void enableBackward(bool enable);
	bool isBackwardEnabled();
	void enableStrafeLeft(bool enable);
	bool isStrafeLeftEnabled();
	void enableStrafeRight(bool enable);
	bool isStrafeRightEnabled();
	void enableUp(bool enable);
	bool isUpEnabled();
	void enableDown(bool enable);
	bool isDownEnabled();
	void enableRollLeft(bool enable);
	bool isRollLeftEnabled();
	void enableRollRight(bool enable);
	bool isRollRightEnabled();
	void enableMouseMove(bool enable);
	bool isMouseMoveEnabled();
	///@}

	/**
	 * \name Speeds getters/setters.
	 */
	///@{
	void setLinearSpeed(const LVector3f& linearSpeed);
	LVector3f getLinearSpeed();
	void setAngularSpeed(float angularSpeed);
	float getAngularSpeed();
	void setFastFactor(float factor);
	float getFastFactor();
	///@}

private:
	///@{
	///Key controls and effective keys.
	bool mForward, mBackward, mStrafeLeft, mStrafeRight, mUp, mDown,
	mRollLeft, mRollRight, mMouseMove;
	bool mForwardKey, mBackwardKey, mStrafeLeftKey, mStrafeRightKey, mUpKey,
	mDownKey, mRollLeftKey, mRollRightKey, mMouseMoveKey;
	std::string mSpeedKey;
	///@}
	///@{
	///Key control values.
	bool mTrue, mFalse, mInvertedKeyboard, mInvertedMouse, mMouseEnabledH,
	mMouseEnabledP;
	///@}
	///@{
	/// Sensitivity settings.
	float mFastFactor;
	LVecBase3f mSpeedActualXYZ;
	float mSpeedActualH;
	float mMovSens, mRollSens;
	float mSensX, mSensY;
	int mCentX, mCentY;
	///@}
	///Enabling flags.
	bool mStartEnabled, mEnabled;
#ifdef ELY_THREAD
	bool mDisabling;
#endif

#ifdef ELY_THREAD
	///Actual transform state.
	CSMARTPTR(TransformState) mActualTransform;
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Driver", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;
};

///inline definitions

inline void Driver::reset()
{
	//
	mForward = mBackward = mStrafeLeft = mStrafeRight = mUp = mDown =
			mRollLeft = mRollRight = false;
	//by default we consider mouse moved on every update, because
	//we want mouse poll by default; this can be changed by calling
	//the enabler (for example by an handler responding to mouse-move
	//event if it is possible. See: http://www.panda3d.org/forums/viewtopic.php?t=9326
	// http://www.panda3d.org/forums/viewtopic.php?t=6049)
	mMouseMove = true;
	mForwardKey = mBackwardKey = mStrafeLeftKey = mStrafeRightKey = mUpKey,
	mDownKey = mRollLeftKey = mRollRightKey = mMouseMoveKey = false;
	mSpeedKey = std::string("shift");
	mTrue = true;
	mFalse = false;
	mInvertedKeyboard = mInvertedMouse = mMouseEnabledH = mMouseEnabledP = false;
	mFastFactor = 0.0;
	mSpeedActualXYZ = LVecBase3f::zero();
	mSpeedActualH = 0.0;
	mMovSens = mRollSens = 0.0;
	mSensX = mSensY = 0.0;
	mCentX = mCentY = 0.0;
	mStartEnabled = mEnabled = false;
#ifdef ELY_THREAD
	mDisabling = false;
#endif
}

inline bool Driver::isEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mEnabled;
}

inline void Driver::enableForward(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mForwardKey)
	{
		mForward = enable;
	}
}

inline bool Driver::isForwardEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mForward;
}

inline void Driver::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

inline bool Driver::isBackwardEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mBackward;
}

inline void Driver::enableStrafeLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

inline bool Driver::isStrafeLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mStrafeLeft;
}

inline void Driver::enableStrafeRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

inline bool Driver::isStrafeRightEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mStrafeRight;
}

inline void Driver::enableUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mUpKey)
	{
		mUp = enable;
	}
}

inline bool Driver::isUpEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mUp;
}

inline void Driver::enableDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mDownKey)
	{
		mDown = enable;
	}
}

inline bool Driver::isDownEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mDown;
}

inline void Driver::enableRollLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mRollLeftKey)
	{
		mRollLeft = enable;
	}
}

inline bool Driver::isRollLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mRollLeft;
}

inline void Driver::enableRollRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mRollRightKey)
	{
		mRollRight = enable;
	}
}

inline bool Driver::isRollRightEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mRollRight;
}

inline void Driver::enableMouseMove(bool enable)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mMouseMoveKey)
	{
		mMouseMove = enable;
	}
}

inline bool Driver::isMouseMoveEnabled()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mMouseMove;
}

inline void Driver::setLinearSpeed(const LVector3f& linearSpeed)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	mSpeedActualXYZ = linearSpeed;
}

inline LVector3f Driver::getLinearSpeed()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mSpeedActualXYZ;
}

inline void Driver::setAngularSpeed(float angularSpeed)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	mSpeedActualH = angularSpeed;
}

inline float Driver::getAngularSpeed()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mSpeedActualH;
}

inline void Driver::setFastFactor(float factor)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	mFastFactor = factor;
}

inline float Driver::getFastFactor()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mFastFactor;
}

}  // namespace ely

#endif /* CONTROLBYEVENT_H_ */
