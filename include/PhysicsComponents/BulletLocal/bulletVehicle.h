// Filename: bulletVehicle.h
// Created by:  enn0x (16Feb10)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#ifndef __BULLET_VEHICLE_H__
#define __BULLET_VEHICLE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"

#include <typedReferenceCount.h>
#include <luse.h>

namespace ely
{
class BulletWorld;
class BulletRigidBodyNode;
class BulletWheel;

////////////////////////////////////////////////////////////////////
//       Class : BulletVehicleTuning
// Description : 
////////////////////////////////////////////////////////////////////
class BulletVehicleTuning
{

public:
	inline void set_suspension_stiffness(PN_stdfloat value);
	inline void set_suspension_compression(PN_stdfloat value);
	inline void set_suspension_damping(PN_stdfloat value);
	inline void set_max_suspension_travel_cm(PN_stdfloat value);
	inline void set_friction_slip(PN_stdfloat value);
	inline void set_max_suspension_force(PN_stdfloat value);

	inline PN_stdfloat get_suspension_stiffness() const;
	inline PN_stdfloat get_suspension_compression() const;
	inline PN_stdfloat get_suspension_damping() const;
	inline PN_stdfloat get_max_suspension_travel_cm() const;
	inline PN_stdfloat get_friction_slip() const;
	inline PN_stdfloat get_max_suspension_force() const;

private:
	btRaycastVehicle::btVehicleTuning _;

	friend class BulletVehicle;
};

////////////////////////////////////////////////////////////////////
//       Class : BulletVehicle
// Description : Simulates a raycast vehicle which casts a ray per
//               wheel at the ground as a cheap replacement for
//               complex suspension simulation. The suspension can
//               be tuned in various ways. It is possible to add a
//               (probably) arbitrary number of wheels.
////////////////////////////////////////////////////////////////////
class BulletVehicle: public TypedReferenceCount
{

public:
	BulletVehicle(BulletWorld *world, BulletRigidBodyNode *chassis);
	inline ~BulletVehicle();

	void set_coordinate_system(BulletUpAxis up);
	void set_steering_value(PN_stdfloat steering, int idx);
	void set_brake(PN_stdfloat brake, int idx);
	void set_pitch_control(PN_stdfloat pitch);

	BulletRigidBodyNode *get_chassis();
	PN_stdfloat get_current_speed_km_hour() const;
	PN_stdfloat get_steering_value(int idx) const;
	LVector3 get_forward_vector() const;

	void reset_suspension();
	void apply_engine_force(PN_stdfloat force, int idx);

	// Wheels
	BulletWheel create_wheel();

	inline int get_num_wheels() const;
	BulletWheel get_wheel(int idx) const;
	;

	// Tuning
	inline BulletVehicleTuning &get_tuning();

public:
	inline btRaycastVehicle *get_vehicle() const;

	void sync_b2p();

private:
	btRaycastVehicle *_vehicle;
	btVehicleRaycaster *_raycaster;

	BulletVehicleTuning _tuning;

	static btVector3 get_axis(int idx);

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedReferenceCount::init_type();
		register_type(_type_handle, "BulletVehicle",
				TypedReferenceCount::get_class_type());
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

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicle::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletVehicle::~BulletVehicle()
{

	delete _vehicle;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicle::get_vehicle
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
btRaycastVehicle *BulletVehicle::get_vehicle() const
{

	return _vehicle;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicle::get_tuning
//       Access: Published
//  Description: Returns a reference to the BulletVehicleTuning
//               object of this vehicle which offers various
//               vehicle-global tuning options. Make sure to
//               configure this before adding wheels!
////////////////////////////////////////////////////////////////////
BulletVehicleTuning &BulletVehicle::get_tuning()
{

	return _tuning;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicle::get_num_wheels
//       Access: Published
//  Description: Returns the number of wheels this vehicle has.
////////////////////////////////////////////////////////////////////
inline int BulletVehicle::get_num_wheels() const
{

	return _vehicle->getNumWheels();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::set_suspension_stiffness
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletVehicleTuning::set_suspension_stiffness(PN_stdfloat value)
{

	_.m_suspensionStiffness = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::set_suspension_compression
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletVehicleTuning::set_suspension_compression(PN_stdfloat value)
{

	_.m_suspensionCompression = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::set_suspension_damping
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletVehicleTuning::set_suspension_damping(PN_stdfloat value)
{

	_.m_suspensionDamping = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::set_max_suspension_travel_cm
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletVehicleTuning::set_max_suspension_travel_cm(PN_stdfloat value)
{

	_.m_maxSuspensionTravelCm = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::set_friction_slip
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletVehicleTuning::set_friction_slip(PN_stdfloat value)
{

	_.m_frictionSlip = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::set_max_suspension_force
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletVehicleTuning::set_max_suspension_force(PN_stdfloat value)
{

	_.m_maxSuspensionForce = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::get_suspension_stiffness
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
PN_stdfloat BulletVehicleTuning::get_suspension_stiffness() const
{

	return (PN_stdfloat) _.m_suspensionStiffness;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::get_suspension_compression
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
PN_stdfloat BulletVehicleTuning::get_suspension_compression() const
{

	return (PN_stdfloat) _.m_suspensionCompression;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::get_suspension_damping
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
PN_stdfloat BulletVehicleTuning::get_suspension_damping() const
{

	return (PN_stdfloat) _.m_suspensionDamping;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::get_max_suspension_travel_cm
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
PN_stdfloat BulletVehicleTuning::get_max_suspension_travel_cm() const
{

	return (PN_stdfloat) _.m_maxSuspensionTravelCm;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::get_friction_slip
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
PN_stdfloat BulletVehicleTuning::get_friction_slip() const
{

	return (PN_stdfloat) _.m_frictionSlip;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletVehicleTuning::get_max_suspension_force
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
PN_stdfloat BulletVehicleTuning::get_max_suspension_force() const
{

	return (PN_stdfloat) _.m_maxSuspensionForce;
}

}
#endif // __BULLET_VEHICLE_H__
