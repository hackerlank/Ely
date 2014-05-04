// Filename: bulletConstraint.h
// Created by:  enn0x (01Mar10)
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

#ifndef __BULLET_CONSTRAINT_H__
#define __BULLET_CONSTRAINT_H__

#include <pandabase.h>

#include "bullet_includes.h"

#include <typedReferenceCount.h>

namespace ely
{
class BulletRigidBodyNode;

////////////////////////////////////////////////////////////////////
//       Class : BulletConstraint
// Description : 
////////////////////////////////////////////////////////////////////
class BulletConstraint: public TypedReferenceCount
{

public:
	inline virtual ~BulletConstraint();

	BulletRigidBodyNode *get_rigid_body_a();
	BulletRigidBodyNode *get_rigid_body_b();

	void enable_feedback(bool value);
	void set_debug_draw_size(PN_stdfloat size);

	PN_stdfloat get_applied_impulse() const;
	PN_stdfloat get_debug_draw_size();

	inline void set_breaking_threshold(PN_stdfloat threshold);
	inline PN_stdfloat set_breaking_threshold() const;
	inline void set_enabled(bool enabled);
	inline bool is_enabled() const;

	enum ConstraintParam
	{
		CP_erp = 1, CP_stop_erp, CP_cfm, CP_stop_cfm
	};

	void set_param(ConstraintParam num, PN_stdfloat value, int axis = -1);
	PN_stdfloat get_param(ConstraintParam num, int axis = -1);

public:
	virtual btTypedConstraint *ptr() const = 0;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedReferenceCount::init_type();
		register_type(_type_handle, "BulletConstraint",
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
//     Function: BulletConstraint::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletConstraint::~BulletConstraint()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletConstraint::set_breaking_threshold
//       Access: Published
//  Description: Sets the applied impulse limit for breaking the
//               constraint. If the limit is exceeded the constraint
//               will be disabled. Disabled constraints are not
//               removed from the world, and can be re-enabled.
////////////////////////////////////////////////////////////////////
inline void BulletConstraint::set_breaking_threshold(PN_stdfloat threshold)
{

	ptr()->setBreakingImpulseThreshold(threshold);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConstraint::set_breaking_threshold
//       Access: Published
//  Description: Returns the applied impluse limit for breaking the
//               constraint.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletConstraint::set_breaking_threshold() const
{

	return (PN_stdfloat) ptr()->getBreakingImpulseThreshold();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConstraint::set_enabled
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletConstraint::set_enabled(bool enable)
{

	ptr()->setEnabled(enable);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConstraint::is_enabled
//       Access: Published
//  Description: Returns TRUE if the constraint is enabled.
////////////////////////////////////////////////////////////////////
inline bool BulletConstraint::is_enabled() const
{

	return ptr()->isEnabled();
}
}
#endif // __BULLET_CONSTRAINT_H__
