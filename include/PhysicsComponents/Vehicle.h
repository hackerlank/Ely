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
 * \file /Ely/include/PhysicsComponents/Vehicle.h
 *
 * \date 15/set/2013 (10:40:10)
 * \author consultit
 */

#ifndef VEHICLE_H_
#define VEHICLE_H_

#include <bulletVehicle.h>
#include "ObjectModel/Component.h"

namespace ely
{
class VehicleTemplate;

/**
 * \brief Component representing a vehicle attached to an object.
 *
 * The control is accomplished through physics.\n
 * It constructs a vehicle with a "chassis" and a series of "wheels":
 * both of which represented by Objects whose templates are passed as
 * parameters.\n
 * Both chassis and wheels objects are constructed on vehicle construction
 * and destroyed during vehicle destruction.\n
 * Chassis object should have at least a "RigidBody" and a
 * "Model"/"InstanceOf" component; wheel object should have at
 * least a "Model"/"InstanceOf" component.\n
 * The up axis is the Z axis.\n
 *
 * XML Param(s):
 * - "throw_events"					|single|"false"
 * - "chassis_object_template"		|single|no default
 * - "chassis_shape_type"  			|single|"box"
 * - "chassis_shape_size"  			|single|"medium"  (min, medium, max)
 * - "chassis_model"  				|single|no default
 * - "chassis_scale"  				|single|"1.0"
 * - "chassis_mass"  				|single|"1.0"
 * - "chassis_friction"  			|single|"0.8"
 * - "chassis_restitution"  		|single|"0.1"
 * - "chassis_collide_mask"			|single|"all_on"
 * - "wheels_number"  				|single|"4"
 * - "wheel_object_template"		|single|no default
 * - "wheel_model"  				|single|no default ("model@wheelIdx")
 * - "wheel_is_front"				|single|no default ("value@wheelIdx" with
 * value=true,false)
 * - "wheel_radius"					|single|no default ("radius@wheelIdx")
 * - "wheel_connection_point_ratio"	|single|no default ("rx,ry,rz@wheelIdx")
 * (pointX,Y,Z=chassisCenterX,Y,Z + chassisHalfDimX,Y,Z * rX,Y,Z)
 * - "wheel_axle"					|single|no default ("ax,ay,az@wheelIdx")
 * - "wheel_direction"				|single|no default ("dx,dy,dz@wheelIdx")
 * - "wheel_suspension_travel"		|single|"40.0" ("st@wheelIdx")
 * - "wheel_suspension_stiffness"	|single|"40.0" ("ss@wheelIdx")
 * - "wheel_damping_relaxation"		|single|"2.0"  ("dr@wheelIdx")
 * - "wheel_damping_compression"	|single|"4.0"  ("dc@wheelIdx")
 * - "wheel_friction_slip"			|single|"100.0"  ("fs@wheelIdx")
 * - "wheel_roll_influence"			|single|"0.1"  ("ri@wheelIdx")
 *
 *
 *
 * - "use_shape_of"				|single|no default
 * - "shape_radius"  			|single|no default (sphere,cylinder,capsule,cone)
 * - "shape_height"  			|single|no default (cylinder,capsule,cone)
 * - "shape_up"  				|single|no default (cylinder,capsule,cone)
 * - "shape_half_x"  			|single|no default (box)
 * - "shape_half_y"  			|single|no default (box)
 * - "shape_half_z"  			|single|no default (box)
 * - "fall_speed"  				|single|"55.0"
 * - "gravity"  				|single|"29.4" (3G)
 * - "jump_speed"  				|single|"10.0"
 * - "max_slope"  				|single|"45.0" (degrees)
 * - "max_jump_height"  		|single|no default
 * - "forward"  				|single|"enabled"
 * - "backward"  				|single|"enabled"
 * - "up"  						|single|"enabled"
 * - "down"  					|single|"enabled"
 * - "roll_left"  				|single|"enabled"
 * - "roll_right"  				|single|"enabled"
 * - "strafe_left"  			|single|"enabled"
 * - "strafe_right"  			|single|"enabled"
 * - "jump"  					|single|"enabled"
 * - "linear_speed"  			|single|"10.0"
 * - "angular_speed"  			|single|"45.0"
 * - "is_local"  				|single|"true"
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
	 * \brief Gets/sets the node path of this vehicle.
	 * @return The node path of this vehicle.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \name BulletVehicle reference getter & conversion function.
	 */
	///@{
	SMARTPTR(BulletVehicle) getBulletVehicle() const;
	operator SMARTPTR(BulletVehicle)() const;
	///@}

private:
	///The NodePath associated to this vehicle.
	NodePath mNodePath;
	///The underlying BulletVehicle (read-only after creation & before destruction).
	SMARTPTR(BulletVehicle) mVehicle;
	/**
	 * \name Chassis data.
	 */
	///@{
	std::string mChassisTmpl;
	SMARTPTR(Object) mChassis;
	std::string  mChassisShapeTypeParam, mChassisShapeSizeParam,
		mChassisModelParam, mChassisScaleParam, mChassisMassParam,
		mChassisFrictionParam, mChassisRestitutionParam,
		mChassisCollideMaskParam;
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
	mNodePath = NodePath();
	mVehicle.clear();

	mThrowEvents = mOnStartSent = mOnStopSent = false;
}

inline NodePath Vehicle::getNodePath() const
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mNodePath;
}

inline void Vehicle::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	mNodePath = nodePath;
}

inline SMARTPTR(BulletVehicle) Vehicle::getBulletVehicle() const
{
	return mVehicle;
}

inline Vehicle::operator SMARTPTR(BulletVehicle)() const
{
	return mVehicle;
}

} /* namespace ely */
#endif /* VEHICLE_H_ */
