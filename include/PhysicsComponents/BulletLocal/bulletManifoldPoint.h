// Filename: bulletManifoldPoint.h
// Created by:  enn0x (07Mar10)
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

#ifndef __BULLET_MANIFOLD_POINT_H__
#define __BULLET_MANIFOLD_POINT_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"

#include <luse.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletManifoldPoint
// Description : 
////////////////////////////////////////////////////////////////////
class BulletManifoldPoint
{

public:
	inline ~BulletManifoldPoint();

	int get_life_time() const;
	PN_stdfloat get_distance() const;
	PN_stdfloat get_applied_impulse() const;
	LPoint3 get_position_world_on_a() const;
	LPoint3 get_position_world_on_b() const;
	LPoint3 get_normal_world_on_b() const;
	LPoint3 get_local_point_a() const;
	LPoint3 get_local_point_b() const;

	int get_part_id0() const;
	int get_part_id1() const;
	int get_index0() const;
	int get_index1() const;

	inline void set_lateral_friction_initialized(bool value);
	inline void set_lateral_friction_dir1(const LVecBase3 &dir);
	inline void set_lateral_friction_dir2(const LVecBase3 &dir);
	inline void set_contact_motion1(PN_stdfloat value);
	inline void set_contact_motion2(PN_stdfloat value);
	inline void set_combined_friction(PN_stdfloat value);
	inline void set_combined_restitution(PN_stdfloat value);
	inline void set_applied_impulse(PN_stdfloat value);
	inline void set_applied_impulse_lateral1(PN_stdfloat value);
	inline void set_applied_impulse_lateral2(PN_stdfloat value);
	inline void set_contact_cfm1(PN_stdfloat value);
	inline void set_contact_cfm2(PN_stdfloat value);

	inline bool get_lateral_friction_initialized() const;
	inline LVector3 get_lateral_friction_dir1() const;
	inline LVector3 get_lateral_friction_dir2() const;
	inline PN_stdfloat get_contact_motion1() const;
	inline PN_stdfloat get_contact_motion2() const;
	inline PN_stdfloat get_combined_friction() const;
	inline PN_stdfloat get_combined_restitution() const;
	inline PN_stdfloat get_applied_impulse_lateral1() const;
	inline PN_stdfloat get_applied_impulse_lateral2() const;
	inline PN_stdfloat get_contact_cfm1() const;
	inline PN_stdfloat get_contact_cfm2() const;

public:
	BulletManifoldPoint(btManifoldPoint &pt);

	BulletManifoldPoint(const BulletManifoldPoint &other);
	BulletManifoldPoint& operator=(const BulletManifoldPoint& other);

private:
	btManifoldPoint &_pt;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletManifoldPoint::~BulletManifoldPoint()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_lateral_friction_initialized
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_lateral_friction_initialized(bool value)
{

	_pt.m_lateralFrictionInitialized = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_lateral_friction_initialized
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletManifoldPoint::get_lateral_friction_initialized() const
{

	return _pt.m_lateralFrictionInitialized;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_lateral_friction_dir1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_lateral_friction_dir1(const LVecBase3 &dir)
{

	_pt.m_lateralFrictionDir1 = LVecBase3_to_btVector3(dir);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_lateral_friction_dir1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletManifoldPoint::get_lateral_friction_dir1() const
{

	return btVector3_to_LVector3(_pt.m_lateralFrictionDir1);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_lateral_friction_dir2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_lateral_friction_dir2(const LVecBase3 &dir)
{

	_pt.m_lateralFrictionDir2 = LVecBase3_to_btVector3(dir);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_lateral_friction_dir2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletManifoldPoint::get_lateral_friction_dir2() const
{

	return btVector3_to_LVector3(_pt.m_lateralFrictionDir2);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_contact_motion1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_contact_motion1(PN_stdfloat value)
{

	_pt.m_contactMotion1 = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_contact_motion1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_contact_motion1() const
{

	return (PN_stdfloat) _pt.m_contactMotion1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_contact_motion2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_contact_motion2(PN_stdfloat value)
{

	_pt.m_contactMotion2 = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_contact_motion2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_contact_motion2() const
{

	return (PN_stdfloat) _pt.m_contactMotion2;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_combined_friction
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_combined_friction(PN_stdfloat value)
{

	_pt.m_combinedFriction = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_combined_friction
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_combined_friction() const
{

	return (PN_stdfloat) _pt.m_combinedFriction;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_combined_restitution
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_combined_restitution(PN_stdfloat value)
{

	_pt.m_combinedRestitution = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_combined_restitution
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_combined_restitution() const
{

	return (PN_stdfloat) _pt.m_combinedRestitution;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_applied_impulse
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_applied_impulse(PN_stdfloat value)
{

	_pt.m_appliedImpulse = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_applied_impulse_lateral1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_applied_impulse_lateral1(PN_stdfloat value)
{

	_pt.m_appliedImpulseLateral1 = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_applied_impulse_lateral1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_applied_impulse_lateral1() const
{

	return (PN_stdfloat) _pt.m_appliedImpulseLateral1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_applied_impulse_lateral2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_applied_impulse_lateral2(PN_stdfloat value)
{

	_pt.m_appliedImpulseLateral2 = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_applied_impulse_lateral2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_applied_impulse_lateral2() const
{

	return (PN_stdfloat) _pt.m_appliedImpulseLateral2;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_contact_cfm1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_contact_cfm1(PN_stdfloat value)
{

	_pt.m_contactCFM1 = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_contact_cfm1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_contact_cfm1() const
{

	return (PN_stdfloat) _pt.m_contactCFM1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::set_contact_cfm2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletManifoldPoint::set_contact_cfm2(PN_stdfloat value)
{

	_pt.m_contactCFM2 = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletManifoldPoint::get_contact_cfm2
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletManifoldPoint::get_contact_cfm2() const
{

	return (PN_stdfloat) _pt.m_contactCFM2;
}
}

#endif // __BULLET_MANIFOLD_POINT_H__
