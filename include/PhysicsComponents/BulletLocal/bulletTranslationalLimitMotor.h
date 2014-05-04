// Filename: bulletTranslationalLimitMotor.h
// Created by:  enn0x (03Mar13)
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

#ifndef __BULLET_TRANSLATIONAL_LIMIT_MOTOR_H__
#define __BULLET_TRANSLATIONAL_LIMIT_MOTOR_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"

#include <luse.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletTranslationalLimitMotor
// Description : Rotation Limit structure for generic joints.
////////////////////////////////////////////////////////////////////
class BulletTranslationalLimitMotor
{

public:
	BulletTranslationalLimitMotor(const BulletTranslationalLimitMotor &copy);
	~BulletTranslationalLimitMotor();

	inline void set_motor_enabled(int axis, bool enable);
	inline void set_low_limit(const LVecBase3 &limit);
	inline void set_high_limit(const LVecBase3 & limit);
	inline void set_target_velocity(const LVecBase3&velocity);
	inline void set_max_motor_force(const LVecBase3 &force);
	inline void set_damping(PN_stdfloat damping);
	inline void set_softness(PN_stdfloat softness);
	inline void set_restitution(PN_stdfloat restitution);
	inline void set_normal_cfm(const LVecBase3 &cfm);
	inline void set_stop_erp(const LVecBase3 &erp);
	inline void set_stop_cfm(const LVecBase3 &cfm);

	inline bool is_limited(int axis) const;
	inline bool get_motor_enabled(int axis) const;
	inline int get_current_limit(int axis) const;
	inline LVector3 get_current_error() const;
	inline LPoint3 get_current_diff() const;
	inline LVector3 get_accumulated_impulse() const;

public:
	BulletTranslationalLimitMotor(btTranslationalLimitMotor &motor);

private:
	btTranslationalLimitMotor &_motor;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::is_limited
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletTranslationalLimitMotor::is_limited(int axis) const
{

	nassertr((0 <= axis) && (axis <= 2), false);
	return _motor.isLimited(axis);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_motor_enabled
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_motor_enabled(int axis,
		bool enabled)
{

	nassertv((0 <= axis) && (axis <= 2));
	_motor.m_enableMotor[axis] = enabled;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::get_motor_enabled
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletTranslationalLimitMotor::get_motor_enabled(int axis) const
{

	nassertr((0 <= axis) && (axis <= 2), false);
	return _motor.m_enableMotor[axis];
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_low_limit
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_low_limit(const LVecBase3 &limit)
{

	nassertv(!limit.is_nan());
	_motor.m_lowerLimit = LVecBase3_to_btVector3(limit);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_high_limit
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_high_limit(
		const LVecBase3 &limit)
{

	nassertv(!limit.is_nan());
	_motor.m_upperLimit = LVecBase3_to_btVector3(limit);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_target_velocity
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_target_velocity(
		const LVecBase3 &velocity)
{

	nassertv(!velocity.is_nan());
	_motor.m_targetVelocity = LVecBase3_to_btVector3(velocity);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_max_motor_force
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_max_motor_force(
		const LVecBase3 &force)
{

	nassertv(!force.is_nan());
	_motor.m_maxMotorForce = LVecBase3_to_btVector3(force);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_damping
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_damping(PN_stdfloat damping)
{

	_motor.m_damping = (btScalar) damping;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_softness
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_softness(PN_stdfloat softness)
{

	_motor.m_limitSoftness = (btScalar) softness;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_restitution
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_restitution(
		PN_stdfloat restitution)
{

	_motor.m_restitution = (btScalar) restitution;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_normal_cfm
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_normal_cfm(const LVecBase3 &cfm)
{

	nassertv(!cfm.is_nan());
	_motor.m_normalCFM = LVecBase3_to_btVector3(cfm);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_stop_cfm
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_stop_cfm(const LVecBase3 &cfm)
{

	nassertv(!cfm.is_nan());
	_motor.m_stopCFM = LVecBase3_to_btVector3(cfm);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::set_stop_erp
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTranslationalLimitMotor::set_stop_erp(const LVecBase3 &erp)
{

	nassertv(!erp.is_nan());
	_motor.m_stopERP = LVecBase3_to_btVector3(erp);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::get_current_limit
//       Access: Published
//  Description: Retrieves the current value of angle:
//               0 = free,
//               1 = at low limit,
//               2 = at high limit.
////////////////////////////////////////////////////////////////////
inline int BulletTranslationalLimitMotor::get_current_limit(int axis) const
{

	nassertr((0 < -axis) && (axis <= 2), false);
	return _motor.m_currentLimit[axis];
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::get_current_error
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletTranslationalLimitMotor::get_current_error() const
{

	return btVector3_to_LVector3(_motor.m_currentLimitError);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::get_current_diff
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LPoint3 BulletTranslationalLimitMotor::get_current_diff() const
{

	return btVector3_to_LPoint3(_motor.m_currentLinearDiff);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTranslationalLimitMotor::get_accumulated_impulse
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletTranslationalLimitMotor::get_accumulated_impulse() const
{

	return btVector3_to_LVector3(_motor.m_accumulatedImpulse);
}
}

#endif // __BULLET_TRANSLATIONAL_LIMIT_MOTOR_H__
