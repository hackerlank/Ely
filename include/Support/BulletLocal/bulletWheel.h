// Filename: bulletWheel.h
// Created by:  enn0x (17Feb10)
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

#ifndef __BULLET_WHEEL_H__
#define __BULLET_WHEEL_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"

#include <luse.h>
#include <pandaNode.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletWheelRaycastInfo
// Description : 
////////////////////////////////////////////////////////////////////
class BulletWheelRaycastInfo
{

public:
	inline ~BulletWheelRaycastInfo();

	inline bool is_in_contact() const;
	inline PN_stdfloat get_suspension_length() const;
	inline LVector3 get_contact_normal_ws() const;
	inline LVector3 get_wheel_direction_ws() const;
	inline LVector3 get_wheel_axle_ws() const;
	inline LPoint3 get_contact_point_ws() const;
	inline LPoint3 get_hard_point_ws() const;
	inline PandaNode *get_ground_object() const;

public:
	BulletWheelRaycastInfo(btWheelInfo::RaycastInfo &info);

private:
	btWheelInfo::RaycastInfo &_info;
};

////////////////////////////////////////////////////////////////////
//       Class : BulletWheel
// Description : One wheel of a BulletVehicle. Instances should not
//               be created directly but using the factory method
//               BulletVehicle::create_wheel().
////////////////////////////////////////////////////////////////////
class BulletWheel
{

public:
	inline ~BulletWheel();

	void set_suspension_stiffness(PN_stdfloat value);
	void set_max_suspension_travel_cm(PN_stdfloat value);
	void set_friction_slip(PN_stdfloat value);
	void set_max_suspension_force(PN_stdfloat value);
	void set_wheels_damping_compression(PN_stdfloat value);
	void set_wheels_damping_relaxation(PN_stdfloat value);
	void set_roll_influence(PN_stdfloat value);
	void set_wheel_radius(PN_stdfloat value);
	void set_steering(PN_stdfloat value);
	void set_rotation(PN_stdfloat value);
	void set_delta_rotation(PN_stdfloat value);
	void set_engine_force(PN_stdfloat value);
	void set_brake(PN_stdfloat value);
	void set_skid_info(PN_stdfloat value);
	void set_wheels_suspension_force(PN_stdfloat value);
	void set_suspension_relative_velocity(PN_stdfloat value);
	void set_clipped_inv_connection_point_cs(PN_stdfloat value);
	void set_chassis_connection_point_cs(const LPoint3 &pos);
	void set_wheel_direction_cs(const LVector3 &dir);
	void set_wheel_axle_cs(const LVector3 &axle);
	void set_world_transform(const LMatrix4 &mat);
	void set_front_wheel(bool value);
	void set_node(PandaNode *node);

	PN_stdfloat get_suspension_rest_length() const;
	PN_stdfloat get_suspension_stiffness() const;
	PN_stdfloat get_max_suspension_travel_cm() const;
	PN_stdfloat get_friction_slip() const;
	PN_stdfloat get_max_suspension_force() const;
	PN_stdfloat get_wheels_damping_compression() const;
	PN_stdfloat get_wheels_damping_relaxation() const;
	PN_stdfloat get_roll_influence() const;
	PN_stdfloat get_wheel_radius() const;
	PN_stdfloat get_steering() const;
	PN_stdfloat get_rotation() const;
	PN_stdfloat get_delta_rotation() const;
	PN_stdfloat get_engine_force() const;
	PN_stdfloat get_brake() const;
	PN_stdfloat get_skid_info() const;
	PN_stdfloat get_wheels_suspension_force() const;
	PN_stdfloat get_suspension_relative_velocity() const;
	PN_stdfloat get_clipped_inv_connection_point_cs() const;
	LPoint3 get_chassis_connection_point_cs() const;
	LVector3 get_wheel_direction_cs() const;
	LVector3 get_wheel_axle_cs() const;
	LMatrix4 get_world_transform() const;
	bool is_front_wheel() const;
	PandaNode *get_node() const;
	BulletWheelRaycastInfo get_raycast_info() const;

public:
	BulletWheel(btWheelInfo &info);

	inline static BulletWheel empty();

private:
	btWheelInfo &_info;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletWheel::~BulletWheel()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletWheelRaycastInfo::~BulletWheelRaycastInfo()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::empty
//       Access: Public
//  Description: Named constructor intended to be used for asserts
//               with have to return a concrete value.
////////////////////////////////////////////////////////////////////
inline BulletWheel BulletWheel::empty()
{

	btWheelInfoConstructionInfo ci;
	btWheelInfo info(ci);

	return BulletWheel(info);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::is_in_contact
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletWheelRaycastInfo::is_in_contact() const
{

	return _info.m_isInContact;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::get_suspension_length
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletWheelRaycastInfo::get_suspension_length() const
{

	return _info.m_suspensionLength;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::get_contact_point_ws
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline LPoint3 BulletWheelRaycastInfo::get_contact_point_ws() const
{

	return btVector3_to_LPoint3(_info.m_contactPointWS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::get_hard_point_ws
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline LPoint3 BulletWheelRaycastInfo::get_hard_point_ws() const
{

	return btVector3_to_LPoint3(_info.m_hardPointWS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::get_contact_normal_ws
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletWheelRaycastInfo::get_contact_normal_ws() const
{

	return btVector3_to_LVector3(_info.m_contactNormalWS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::get_wheel_direction_ws
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletWheelRaycastInfo::get_wheel_direction_ws() const
{

	return btVector3_to_LVector3(_info.m_wheelDirectionWS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::get_wheel_axle_ws
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletWheelRaycastInfo::get_wheel_axle_ws() const
{

	return btVector3_to_LVector3(_info.m_wheelAxleWS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheelRaycastInfo::get_ground_object
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletWheelRaycastInfo::get_ground_object() const
{

	return _info.m_groundObject ? (PandaNode *) _info.m_groundObject : NULL;
}

}
#endif // __BULLET_WHEEL_H__
