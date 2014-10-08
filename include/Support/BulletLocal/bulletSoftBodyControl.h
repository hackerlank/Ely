// Filename: bulletSoftBodyControl.h
// Created by:  enn0x (04Mar10)
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

#ifndef __BULLET_SOFT_BODY_CONTROL_H__
#define __BULLET_SOFT_BODY_CONTROL_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"

#include <luse.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletSoftBodyControl
// Description : 
////////////////////////////////////////////////////////////////////
class BulletSoftBodyControl: public btSoftBody::AJoint::IControl
{

public:
	BulletSoftBodyControl();
	virtual ~BulletSoftBodyControl();

	// Motor
	inline void set_goal(PN_stdfloat goal);
	inline void set_max_torque(PN_stdfloat maxtorque);

	//inline PN_stdfloat get_goal() const;
	//inline PN_stdfloat get_max_torque() const;

	// Steer
	inline void set_angle(PN_stdfloat angle);
	inline void set_sign(PN_stdfloat sign);

	//inline PN_stdfloat get_angle() const;
	//inline PN_stdfloat get_sign() const;

public:
	void Prepare(btSoftBody::AJoint* joint);
	btScalar Speed(btSoftBody::AJoint *joint, btScalar current);

private:
	// Motor
	btScalar _goal;
	btScalar _maxtorque;

	// Steer
	btScalar _angle;
	btScalar _sign;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyControl::set_goal
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyControl::set_goal(PN_stdfloat goal)
{

	_goal = (btScalar) goal;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyControl::set_max_torque
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyControl::set_max_torque(PN_stdfloat maxtorque)
{

	_maxtorque = (btScalar) maxtorque;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyControl::set_angle
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyControl::set_angle(PN_stdfloat angle)
{

	_angle = (btScalar) angle;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyControl::set_sign
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyControl::set_sign(PN_stdfloat sign)
{

	_sign = (btScalar) sign;
}

}
#endif // __BULLET_SOFT_BODY_CONTROL_H__
