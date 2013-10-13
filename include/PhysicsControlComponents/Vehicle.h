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
 * This component can throw (if enabled) "OnStartVehicle" and
 * "OnStopVehicle" events.
 *
 * XML Param(s):
 * - "throw_events"					|single|"false"
 * - "up_axis"						|single|"z" (x,y,z)
 * - "wheels_number"  				|single|"4" (== N)
 * - "wheel_object_template"		|single|no default
 * - "wheel_model"  				|single|no default (specified as
 * "model1[,...,modelN]")
 * - "wheel_scale"  				|single|"1.0" (specified as "scale1[,...,scaleN]")
 * - "wheel_is_front"				|single|"false" (specified as "bool1[,...,boolN]")
 * - "wheel_set_steering"			|single|"false" (specified as "bool1[,...,boolN]")
 * - "wheel_apply_engine_force"		|single|"false" (specified as "bool1[,...,boolN]")
 * - "wheel_set_brake"				|single|"false" (specified as "bool1[,...,boolN]")
 * - "wheel_connection_point_ratio"	|single|"1.0,1.0,1.0" (specified as
 * "rx1,ry1,rz1[$...$rxN,ryN,rzN]")
 * (pointX,Y,Z=chassisCenterX,Y,Z + chassisHalfDimX,Y,Z * rX,Y,Z)
 * - "wheel_axle"					|single|"1.0,0.0,0.0" (specified as
 * "ax1,ay1,az1[$...$axN,ayN,azN]")
 * - "wheel_direction"				|single|"0.0,0.0,-1.0" (specified as
 * "dx1,dy1,dz1[$...$dxN,dyN,dzN]")
 * - "wheel_suspension_travel"		|single|"40.0" (specified as "st1[,...,stN]")
 * - "wheel_suspension_stiffness"	|single|"40.0" (specified as "ss1[,...,ssN]")
 * - "wheel_damping_relaxation"		|single|"2.0"  (specified as "dr1[,...,drN]")
 * - "wheel_damping_compression"	|single|"4.0"  (specified as "dc1[,...,dcN]")
 * - "wheel_friction_slip"			|single|"100.0"  (specified as "fs1[,...,fsN]")
 * - "wheel_roll_influence"			|single|"0.1"  (specified as "ri1[,...,riN]")
 * - "max_engine_force"				|single|no default
 * - "max_brake"					|single|no default
 * - "steering_clamp"				|single|"45.0" (in degree)
 * - "steering_increment"			|single|"120.0" (in degree/sec)
 * - "steering_decrement"			|single|"60.0" (in degree/sec)
 * - "forward"  					|single|"enabled"
 * - "backward"  					|single|"enabled"
 * - "brake"  						|single|"enabled"
 * - "turn_left"					|single|"enabled"
 * - "turn_right"  					|single|"enabled"
 *
 * \note parts inside [] are optional.\n
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
	void enableBrake(bool enable);
	bool isBrakeEnabled();
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
	std::vector<SMARTPTR(Object)> mWheelObjects;
	unsigned int mWheelNumber;
	std::string mWheelTmpl;
	std::vector<std::string> mWheelModelParam, mWheelScaleParam;
	std::vector<bool> mWheelIsFront, mWheelSetSteering, mWheelApplyEngineForce,
	mWheelSetBrake;
	std::vector<LVecBase3f> mWheelConnectionPointRatio;
	std::vector<LVector3f> mWheelAxle, mWheelDirection;
	std::vector<float> mWheelRadius, mWheelSuspensionTravel,
	mWheelSuspensionStiffness, mWheelDampingRelaxation,
	mWheelDampingCompression, mWheelFrictionSlip, mWheelRollInfluence;
	//helpers
	LVector3f mVehicleDims, mVehicleDeltaCenter;
	float mVehicleRadius;
	///@}

	///Control and physics functions and parameters.
	///@{
	float mMaxEngineForce, mMaxBrake, mSteering, mSteeringClamp,
	mSteeringIncrement, mSteeringDecrement;
	///Key controls and effective keys.
	bool mForward, mBackward, mBrake, mTurnLeft, mTurnRight;
	bool mForwardKey, mBackwardKey, mBrakeKey, mTurnLeftKey, mTurnRightKey;
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
	mUpAxis = Z_up;
	mWheelObjects.clear();
	mWheelNumber = 0;
	mWheelTmpl.clear();
	mWheelModelParam.clear();
	mWheelScaleParam.clear();
	mWheelIsFront.clear();
	mWheelSetSteering.clear();
	mWheelApplyEngineForce.clear();
	mWheelSetBrake.clear();
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
	mVehicleDims = mVehicleDeltaCenter = LVector3f::zero();
	mVehicleRadius = 0.0;
	mMaxEngineForce = mMaxBrake = mSteering = mSteeringClamp =
			mSteeringIncrement = mSteeringDecrement = 0.0;
	mForward = mBackward = mBrake = mTurnLeft = mTurnRight = mForwardKey =
			mBackwardKey = mBrakeKey = mTurnLeftKey = mTurnRightKey = false;
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

inline void Vehicle::enableBrake(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mBrakeKey)
	{
		mBrake = enable;
	}
}

inline bool Vehicle::isBrakeEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mBrake;
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

	mMaxBrake = force;
}

inline float Vehicle::getMaxBrakeForce()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMaxBrake;
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

	return mWheelObjects;
}

} /* namespace ely */
#endif /* VEHICLE_H_ */
