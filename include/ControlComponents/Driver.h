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
 * Movement is, by default, based on acceleration (i.e. "dynamic"):
 * accelerations (and max speeds) can independently set for any (local)
 * direction and for angular movement.\n
 * To obtain a fixed movement (i.e. "kinematic") in any direction or
 * angular, the related acceleration and friction value should be set
 * to zero and one respectively.\n
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
 * - "enabled"  				|single|"true"
 * - "forward"  				|single|"enabled"
 * - "backward"  				|single|"enabled"
 * - "roll_left"  				|single|"enabled"
 * - "roll_right"  				|single|"enabled"
 * - "strafe_left"  			|single|"enabled"
 * - "strafe_right"  			|single|"enabled"
 * - "up"  						|single|"enabled"
 * - "down"  					|single|"enabled"
 * - "mouse_move"  				|single|"disabled"
 * - "mouse_enabled_h"  		|single|"false"
 * - "mouse_enabled_p"  		|single|"false"
 * - "speed_key"  				|single|"shift"
 * - "inverted_translation"  	|single|"false"
 * - "inverted_rotation"		|single|"false"
 * - "max_linear_speed"  		|single|"5.0"
 * - "max_angular_speed"  		|single|"5.0"
 * - "linear_accel"  			|single|"5.0"
 * - "angular_accel"  			|single|"5.0"
 * - "linear_friction"  		|single|"0.1"
 * - "angular_friction"  		|single|"0.1"
 * - "stop_threshold"	  		|single|"0.01"
 * - "fast_factor"  			|single|"5.0"
 * - "sens_x"  					|single|"0.2"
 * - "sens_y"  					|single|"0.2"
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

	struct Result: public Component::Result
	{
		Result(int value):Component::Result(value)
		{
		}
		enum
		{
#ifdef ELY_THREAD
			DRIVER_DISABLING = COMPONENT_RESULT_END + 1,
			DRIVER_RESULT_END
#endif
		};
	};

	/**
	 * \name Enabling/disabling.
	 * \brief Enables/disables this component.
	 */
	///@{
	Result enable();
	Result disable();
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
	//max values
	void setMaxLinearSpeed(const LVector3f& linearSpeed);
	void setMaxAngularSpeed(float angularSpeed);
	LVector3f getMaxSpeeds(float& angularSpeed);
	void setLinearAccel(const LVector3f& linearAccel);
	void setAngularAccel(float angularAccel);
	LVector3f getAccels(float& angularAccel);
	void setLinearFriction(float linearFriction);
	void setAngularFriction(float angularFriction);
	LVector3f getFrictions(float& angularFriction);
	void setSens(float sensX, float sensY);
	void getSens(float& sensX, float& sensY);
	void setFastFactor(float factor);
	float getFastFactor();
	//speed current values
	LVector3f getCurrentSpeeds(float& angularSpeed);
	///@}

private:
	///Enabling flags.
	bool mStartEnabled, mEnabled;
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
	bool mMouseEnabledH,	mMouseEnabledP;
	int mSignOfTranslation, mSignOfMouse;
	///@}
	///@{
	/// Sensitivity settings.
	float mFastFactor;
	LVecBase3f mActualSpeedXYZ, mMaxSpeedXYZ, mMaxSpeedSquaredXYZ;
	float mActualSpeedH, mMaxSpeedH, mMaxSpeedSquaredH;
	LVecBase3f mAccelXYZ;
	float mAccelH;
	LVecBase3f mFrictionXYZ;
	float mFrictionH;
	float mStopThreshold;
	float mSensX, mSensY;
	int mCentX, mCentY;
	///@}
#ifdef ELY_THREAD
	bool mDisabling;
#endif

	/**
	 * \name Actual enabling/disabling.
	 */
	///@{
	void doEnable();
	void doDisable();
	///@}

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
	mStartEnabled = mEnabled = false;
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
	mMouseEnabledH = mMouseEnabledP = false;
	mSignOfTranslation = mSignOfMouse = 1;
	mFastFactor = 0.0;
	mActualSpeedXYZ = mMaxSpeedXYZ = mMaxSpeedSquaredXYZ = LVecBase3f::zero();
	mActualSpeedH = mMaxSpeedH = mMaxSpeedSquaredH = 0.0;
	mAccelXYZ = LVecBase3f::zero();
	mAccelH = 0.0;
	mFrictionXYZ = LVecBase3f::zero();
	mFrictionH = 0.0;
	mStopThreshold = 0.0;
	mSensX = mSensY = 0.0;
	mCentX = mCentY = 0.0;
#ifdef ELY_THREAD
	mDisabling = false;
#endif
}

inline bool Driver::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mEnabled;
}

inline void Driver::enableForward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mForwardKey)
	{
		mForward = enable;
	}
}

inline bool Driver::isForwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mForward;
}

inline void Driver::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

inline bool Driver::isBackwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mBackward;
}

inline void Driver::enableStrafeLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

inline bool Driver::isStrafeLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStrafeLeft;
}

inline void Driver::enableStrafeRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

inline bool Driver::isStrafeRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStrafeRight;
}

inline void Driver::enableUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mUpKey)
	{
		mUp = enable;
	}
}

inline bool Driver::isUpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mUp;
}

inline void Driver::enableDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mDownKey)
	{
		mDown = enable;
	}
}

inline bool Driver::isDownEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mDown;
}

inline void Driver::enableRollLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mRollLeftKey)
	{
		mRollLeft = enable;
	}
}

inline bool Driver::isRollLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mRollLeft;
}

inline void Driver::enableRollRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mRollRightKey)
	{
		mRollRight = enable;
	}
}

inline bool Driver::isRollRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mRollRight;
}

inline void Driver::enableMouseMove(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mMouseMoveKey)
	{
		mMouseMove = enable;
	}
}

inline bool Driver::isMouseMoveEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMouseMove;
}

inline void Driver::setMaxLinearSpeed(const LVector3f& maxLinearSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mMaxSpeedXYZ = maxLinearSpeed;
	mMaxSpeedSquaredXYZ = LVector3f(
			maxLinearSpeed.get_x() * maxLinearSpeed.get_x(),
			maxLinearSpeed.get_y() * maxLinearSpeed.get_y(),
			maxLinearSpeed.get_z() * maxLinearSpeed.get_z());
}

inline void Driver::setMaxAngularSpeed(float maxAngularSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mMaxSpeedH = maxAngularSpeed;
	mMaxSpeedSquaredH = maxAngularSpeed * maxAngularSpeed;
}

inline LVector3f Driver::getMaxSpeeds(float& maxAngularSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	maxAngularSpeed = mMaxSpeedH;
	return mMaxSpeedXYZ;
}

inline void Driver::setLinearAccel(const LVector3f& linearAccel)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mAccelXYZ = linearAccel;
}

inline void Driver::setAngularAccel(float angularAccel)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mAccelH = angularAccel;
}

inline LVector3f Driver::getAccels(float& angularAccel)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	angularAccel = mAccelH;
	return mAccelXYZ;
}

inline void Driver::setLinearFriction(float linearFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFrictionXYZ = linearFriction;
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
}

inline void Driver::setAngularFriction(float angularFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFrictionH = angularFriction;
	if ((mFrictionH < 0.0) or (mFrictionH > 1.0))
	{
		mFrictionH = 0.1;
	}
}

inline LVector3f Driver::getFrictions(float& angularFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	angularFriction	= mFrictionH;
	return mFrictionXYZ;
}

inline void Driver::setSens(float sensX, float sensY)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mSensX = sensX;
	mSensY = sensY;
}

inline void Driver::getSens(float& sensX, float& sensY)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	sensX = mSensX;
	sensY = mSensY;
}

inline void Driver::setFastFactor(float factor)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFastFactor = factor;
}

inline float Driver::getFastFactor()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFastFactor;
}

inline LVector3f Driver::getCurrentSpeeds(float& angularSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	angularSpeed = mActualSpeedH;
	return mActualSpeedXYZ;
}

}  // namespace ely

#endif /* CONTROLBYEVENT_H_ */
