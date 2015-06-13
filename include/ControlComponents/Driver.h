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
 * Each basic movement (forward, backward, head_left, head_right etc...)
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
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *enabled*  				|single| *true* | -
 * | *forward*  				|single| *enabled* | -
 * | *backward*  				|single| *enabled* | -
 * | *head_limit*  				|single| *false@0.0* | specified as "enabled@[limit] with enabled = true,false, with limit >= 0.0
 * | *head_left*  				|single| *enabled* | -
 * | *head_right*  				|single| *enabled* | -
 * | *pitch_limit*  			|single| *false@0.0* | specified as "enabled@[limit] with enabled = true,false, with limit >= 0.0
 * | *pitch_up*  				|single| *enabled* | -
 * | *pitch_down*  				|single| *enabled* | -
 * | *strafe_left*  			|single| *enabled* | -
 * | *strafe_right*  			|single| *enabled* | -
 * | *up*  						|single| *enabled* | -
 * | *down*  					|single| *enabled* | -
 * | *mouse_move*  				|single| *disabled* | -
 * | *mouse_enabled_h*  		|single| *false* | -
 * | *mouse_enabled_p*  		|single| *false* | -
 * | *speed_key*  				|single| *shift* | -
 * | *inverted_translation*  	|single| *false* | -
 * | *inverted_rotation*		|single| *false* | -
 * | *max_linear_speed*  		|single| 5.0 | -
 * | *max_angular_speed*  		|single| 5.0 | -
 * | *linear_accel*  			|single| 5.0 | -
 * | *angular_accel*  			|single| 5.0 | -
 * | *linear_friction*  		|single| 0.1 | -
 * | *angular_friction*  		|single| 0.1 | -
 * | *stop_threshold*	  		|single| 0.01 | -
 * | *fast_factor*  			|single| 5.0 | -
 * | *sens_x*  					|single| 0.2 | -
 * | *sens_y*  					|single| 0.2 | -
 *
 * \note parts inside [] are optional.\n
 */
class Driver: public Component
{
protected:
	friend class DriverTemplate;

	Driver(SMARTPTR(DriverTemplate)tmpl);
	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
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
	void enableHeadLeft(bool enable);
	bool isHeadLeftEnabled();
	void enableHeadRight(bool enable);
	bool isHeadRightEnabled();
	void enablePitchUp(bool enable);
	bool isPitchUpEnabled();
	void enablePitchDown(bool enable);
	bool isPitchDownEnabled();
	void enableMouseMove(bool enable);
	bool isMouseMoveEnabled();
	///@}

	/**
	 * \name Parameters getters/setters.
	 */
	///@{
	//max values
	void setHeadLimit(bool enabled = false, float hLimit = 0.0);
	void setPitchLimit(bool enabled = false, float pLimit = 0.0);
	void setMaxLinearSpeed(const LVector3f& linearSpeed);
	void setMaxAngularSpeed(float angularSpeed);
	LVector3f getMaxSpeeds(float& angularSpeed);
	void setLinearAccel(const LVector3f& linearAccel);
	void setAngularAccel(float angularAccel);
	LVector3f getAccels(float& angularAccel);
	void setLinearFriction(float linearFriction);
	void setAngularFriction(float angularFriction);
	void getFrictions(float& linearFriction, float& angularFriction);
	void setSens(float sensX, float sensY);
	void getSens(float& sensX, float& sensY);
	void setFastFactor(float factor);
	float getFastFactor();
	//speed current values
	LVector3f getCurrentSpeeds(float& angularSpeedH, float& angularSpeedP);
	///@}

private:
	///Enabling flags.
	bool mStartEnabled, mEnabled;
	///@{
	///Key controls and effective keys.
	bool mForward, mBackward, mStrafeLeft, mStrafeRight, mUp, mDown,
	mHeadLeft, mHeadRight, mPitchUp, mPitchDown, mMouseMove;
	bool mForwardKey, mBackwardKey, mStrafeLeftKey, mStrafeRightKey, mUpKey,
	mDownKey, mHeadLeftKey, mHeadRightKey, mPitchUpKey, mPitchDownKey, mMouseMoveKey;
	std::string mSpeedKey;
	///@}
	///@{
	///Key control values.
	bool mMouseEnabledH,	mMouseEnabledP;
	bool mHeadLimitEnabled, mPitchLimitEnabled;
	float mHLimit, mPLimit;
	int mSignOfTranslation, mSignOfMouse;
	///@}
	///@{
	/// Sensitivity settings.
	float mFastFactor;
	LVecBase3f mActualSpeedXYZ, mMaxSpeedXYZ, mMaxSpeedSquaredXYZ;
	float mActualSpeedH, mActualSpeedP, mMaxSpeedHP, mMaxSpeedSquaredHP;
	LVecBase3f mAccelXYZ;
	float mAccelHP;
	float mFrictionXYZ;
	float mFrictionHP;
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
			mHeadLeft = mHeadRight = mPitchUp = mPitchDown = false;
	//by default we consider mouse moved on every update, because
	//we want mouse poll by default; this can be changed by calling
	//the enabler (for example by an handler responding to mouse-move
	//event if it is possible. See: http://www.panda3d.org/forums/viewtopic.php?t=9326
	// http://www.panda3d.org/forums/viewtopic.php?t=6049)
	mMouseMove = true;
	mForwardKey = mBackwardKey = mStrafeLeftKey = mStrafeRightKey = mUpKey,
	mDownKey = mHeadLeftKey = mHeadRightKey = mPitchUpKey = mPitchDownKey = mMouseMoveKey = false;
	mSpeedKey = std::string("shift");
	mMouseEnabledH = mMouseEnabledP = mHeadLimitEnabled = mPitchLimitEnabled = false;
	mHLimit = mPLimit = 0.0;
	mSignOfTranslation = mSignOfMouse = 1;
	mFastFactor = 0.0;
	mActualSpeedXYZ = mMaxSpeedXYZ = mMaxSpeedSquaredXYZ = LVecBase3f::zero();
	mActualSpeedH = mActualSpeedP = mMaxSpeedHP = mMaxSpeedSquaredHP = 0.0;
	mAccelXYZ = LVecBase3f::zero();
	mAccelHP = 0.0;
	mFrictionXYZ = mFrictionHP = 0.0;
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

inline void Driver::enableHeadLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadLeftKey)
	{
		mHeadLeft = enable;
	}
}

inline bool Driver::isHeadLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadLeft;
}

inline void Driver::enableHeadRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadRightKey)
	{
		mHeadRight = enable;
	}
}

inline bool Driver::isHeadRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadRight;
}

inline void Driver::enablePitchUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchUpKey)
	{
		mPitchUp = enable;
	}
}

inline bool Driver::isPitchUpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchUp;
}

inline void Driver::enablePitchDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchDownKey)
	{
		mPitchDown = enable;
	}
}

inline bool Driver::isPitchDownEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchDown;
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

inline void Driver::setHeadLimit(bool enabled, float hLimit)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mHeadLimitEnabled = enabled;
	hLimit >= 0.0 ? mHLimit = hLimit: mHLimit = -hLimit;
}

inline void Driver::setPitchLimit(bool enabled, float pLimit)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mPitchLimitEnabled = enabled;
	pLimit >= 0.0 ? mPLimit = pLimit: mPLimit = -pLimit;
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

	mMaxSpeedHP = maxAngularSpeed;
	mMaxSpeedSquaredHP = maxAngularSpeed * maxAngularSpeed;
}

inline LVector3f Driver::getMaxSpeeds(float& maxAngularSpeed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	maxAngularSpeed = mMaxSpeedHP;
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

	mAccelHP = angularAccel;
}

inline LVector3f Driver::getAccels(float& angularAccel)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	angularAccel = mAccelHP;
	return mAccelXYZ;
}

inline void Driver::setLinearFriction(float linearFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFrictionXYZ = linearFriction;
}

inline void Driver::setAngularFriction(float angularFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFrictionHP = angularFriction;
	if ((mFrictionHP < 0.0) or (mFrictionHP > 1.0))
	{
		mFrictionHP = 0.1;
	}
}

inline void Driver::getFrictions(float& linearFriction, float& angularFriction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	linearFriction	= mFrictionXYZ;
	angularFriction	= mFrictionHP;
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

inline LVector3f Driver::getCurrentSpeeds(float& angularSpeedH, float& angularSpeedP)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	angularSpeedH = mActualSpeedH;
	angularSpeedP = mActualSpeedP;
	return mActualSpeedXYZ;
}

///Template

class DriverTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	DriverTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~DriverTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType familyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "DriverTemplate",
				ComponentTemplate::get_class_type());
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
}  // namespace ely

#endif /* CONTROLBYEVENT_H_ */
