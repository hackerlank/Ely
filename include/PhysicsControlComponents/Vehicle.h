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
 * \file /Ely/include/PhysicsControlComponents/Vehicle.h
 *
 * \date 15/set/2013 (10:40:10)
 * \author consultit
 */

#ifndef VEHICLE_H_
#define VEHICLE_H_

#include <bulletVehicle.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

namespace ely
{
class VehicleTemplate;

/**
 * \brief Component making an Object to behave as a vehicle.
 *
 * The control is accomplished through physics.\n
 * It constructs a vehicle with a "chassis" represented by a RigidBody
 * component, presumably associated to a Scene component (Model or
 * InstanceOf).\n
 * The "wheels" are represented by a series of Objects whose templates
 * are passed as parameters. Wheels Objects are constructed on
 * vehicle construction and destroyed during vehicle destruction.\n
 * Wheel object should have, at least, a Scene (Model or InstanceOf)
 * component.\n
 * The default up axis is the Z axis.\n
 *
 * XML Param(s):
 * - "throw_events"					|single|"false"
 * - "up_axis"						|single|"z" (x,y,z)
 * - "wheels_number"  				|single|"4"
 * - "wheel_object_template"		|single|no default
 * - "wheel_model"  				|multiple|no default ("model@wheelIdx")
 * - "wheel_scale"  				|multiple|"1.0" ("scale@wheelIdx")
 * - "wheel_is_front"				|multiple|"false" ("value@wheelIdx" with
 * value=true,false)
 * - "wheel_connection_point_ratio"	|multiple|no default ("rx,ry,rz@wheelIdx")
 * (pointX,Y,Z=chassisCenterX,Y,Z + chassisHalfDimX,Y,Z * rX,Y,Z)
 * - "wheel_axle"					|multiple|no default ("ax,ay,az@wheelIdx")
 * - "wheel_direction"				|multiple|no default ("dx,dy,dz@wheelIdx")
 * - "wheel_suspension_travel"		|multiple|"40.0" ("st@wheelIdx")
 * - "wheel_suspension_stiffness"	|multiple|"40.0" ("ss@wheelIdx")
 * - "wheel_damping_relaxation"		|multiple|"2.0"  ("dr@wheelIdx")
 * - "wheel_damping_compression"	|multiple|"4.0"  ("dc@wheelIdx")
 * - "wheel_friction_slip"			|multiple|"100.0"  ("fs@wheelIdx")
 * - "wheel_roll_influence"			|multiple|"0.1"  ("ri@wheelIdx")
 * - "max_engine_force"				|single|no default
 * - "max_brake_force"				|single|no default
 * - "steering_clamp"				|single|"45.0" (in degree)
 * - "steering_increment"			|single|"120.0" (in degree/sec)
 * - "steering_decrement"			|single|"60.0" (in degree/sec)
 * - "forward"  					|single|"enabled"
 * - "backward"  					|single|"enabled"
 * - "turn_left"					|single|"enabled"
 * - "turn_right"  					|single|"enabled"
 */
class Vehicle: public Component
{
protected:
	friend class VehicleTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Vehicle();
	Vehicle(SMARTPTR(VehicleTemplate)tmpl);
	virtual ~Vehicle();

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
	 * \name Control keys' enablers.
	 *
	 * These routines should be typically called by event handlers
	 * or AI algorithms, but this is not strictly required.
	 */
	///@{
	void enableForward(bool enable);
	bool isForwardEnabled();
	void enableBackward(bool enable);
	bool isBackwardEnabled();
	void enableTurnLeft(bool enable);
	bool isTurnLeftEnabled();
	void enableTurnRight(bool enable);
	bool isTurnRightEnabled();
	///@}

	/**
	 * \name Speeds getters/setters.
	 */
	///@{
	void setMaxEngineForce(float force);
	float getMaxEngineForce();
	void setMaxBrakeForce(float force);
	float getMaxBrakeForce();
	void setSteeringClamp(float clamp);
	float getSteeringClamp();
	void setSteeringIncrement(float increment);
	float getSteeringIncrement();
	void setSteeringDecrement(float decrement);
	float getSteeringDecrement();
	///@}

	/**
	 * \name BulletVehicle reference getter & conversion function.
	 */
	///@{
	BulletVehicle& getBulletVehicle();
	operator BulletVehicle&();
	///@}

	/**
	 * \name Gets the inner wheels' Objects added to this vehicle.
	 */
	std::vector<SMARTPTR(Object)> getWheelObjects() const;

private:
	///The underlying BulletVehicle (read-only after creation & before destruction).
	SMARTPTR(BulletVehicle) mVehicle;

	///The up axis.
	BulletUpAxis mUpAxis;

	/**
	 * \name Wheels' data.
	 */
	///@{
	std::vector<SMARTPTR(Object)> mWheels;
	int mWheelNumber;
	std::string mWheelTmpl;
	std::vector<std::string> mWheelModelParam, mWheelScaleParam;
	std::vector<bool> mWheelIsFront;
	std::vector<LVecBase3f> mWheelConnectionPointRatio;
	std::vector<LVector3f> mWheelAxle, mWheelDirection;
	std::vector<float> mWheelRadius, mWheelSuspensionTravel,
	mWheelSuspensionStiffness, mWheelDampingRelaxation,
	mWheelDampingCompression, mWheelFrictionSlip, mWheelRollInfluence;
	//helper
	int idxClamp(int value, int min, int max);
	///@}

	///Control and physics functions and parameters.
	///@{
	float mMaxEngineForce, mMaxBrakeForce, mSteering, mSteeringClamp,
	mSteeringIncrement, mSteeringDecrement;
	///Key controls and effective keys.
	bool mForward, mBackward, mTurnLeft, mTurnRight;
	bool mForwardKey, mBackwardKey, mTurnLeftKey, mTurnRightKey;
	///@}

	///Throwing events.
	bool mThrowEvents, mOnStartSent, mOnStopSent;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Vehicle", Component::get_class_type());
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

inline void Vehicle::reset()
{
	//
	mVehicle.clear();
	mWheels.clear();
	mWheelNumber = 0;
	mWheelTmpl.clear();
	mWheelModelParam.clear();
	mWheelScaleParam.clear();
	mWheelIsFront.clear();
	mWheelConnectionPointRatio.clear();
	mWheelAxle.clear();
	mWheelDirection.clear();
	mWheelRadius.clear();
	mWheelSuspensionTravel.clear();
	mWheelSuspensionStiffness.clear();
	mWheelDampingRelaxation.clear();
	mWheelDampingCompression.clear();
	mWheelFrictionSlip.clear();
	mWheelRollInfluence.clear();
	mMaxEngineForce = mMaxBrakeForce = mSteering = mSteeringClamp =
			mSteeringIncrement = mSteeringDecrement = 0.0;
	mForward = mBackward = mTurnLeft = mTurnRight = mForwardKey = mBackwardKey =
			mTurnLeftKey = mTurnRightKey = false;
	mThrowEvents = mOnStartSent = mOnStopSent = false;
}

inline void Vehicle::enableForward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mForwardKey)
	{
		mForward = enable;
	}
}

inline bool Vehicle::isForwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mForward;
}

inline void Vehicle::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

inline bool Vehicle::isBackwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mBackward;
}

inline void Vehicle::enableTurnLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mTurnLeftKey)
	{
		mTurnLeft = enable;
	}
}

inline bool Vehicle::isTurnLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mTurnLeft;
}

inline void Vehicle::enableTurnRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mTurnRightKey)
	{
		mTurnRight = enable;
	}
}

inline bool Vehicle::isTurnRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mTurnRight;
}

inline void Vehicle::setMaxEngineForce(float force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mMaxEngineForce = force;
}

inline float Vehicle::getMaxEngineForce()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMaxEngineForce;
}

inline void Vehicle::setMaxBrakeForce(float force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mMaxBrakeForce = force;
}

inline float Vehicle::getMaxBrakeForce()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMaxBrakeForce;
}

inline void Vehicle::setSteeringClamp(float clamp)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mSteeringClamp = clamp;
}

inline float Vehicle::getSteeringClamp()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mSteeringClamp;
}

inline void Vehicle::setSteeringIncrement(float increment)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mSteeringIncrement = increment;
}

inline float Vehicle::getSteeringIncrement()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mSteeringIncrement;
}

inline void Vehicle::setSteeringDecrement(float decrement)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mSteeringDecrement = decrement;
}

inline float Vehicle::getSteeringDecrement()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mSteeringDecrement;
}

inline BulletVehicle& Vehicle::getBulletVehicle()
{
	return *mVehicle;
}

inline Vehicle::operator BulletVehicle&()
{
	return *mVehicle;
}

inline std::vector<SMARTPTR(Object)> Vehicle::getWheelObjects() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mWheels;
}

inline int Vehicle::idxClamp(int value, int min, int max)
{
	int clamped = value;
	if (value > max)
	{
		clamped = max;
	}
	else if (value < min)
	{
		clamped = min;
	}
	return clamped;
}

} /* namespace ely */
#endif /* VEHICLE_H_ */
