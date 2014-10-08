// Filename: bulletShape.h
// Created by:  enn0x (23Jan10)
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

#ifndef __BULLET_SHAPE_H__
#define __BULLET_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"

#include <luse.h>
#include <typedReferenceCount.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletShape: public TypedReferenceCount
{

public:
	inline virtual ~BulletShape();

	inline bool is_polyhedral() const;
	inline bool is_convex() const;
	inline bool is_convex_2d() const;
	inline bool is_concave() const;
	inline bool is_infinite() const;
	inline bool is_non_moving() const;
	inline bool is_soft_body() const;

	void set_margin(PN_stdfloat margin);
	const char *get_name() const;

	PN_stdfloat get_margin() const;

public:
	virtual btCollisionShape *ptr() const = 0;

	LVecBase3 get_local_scale() const;
	void set_local_scale(const LVecBase3 &scale);

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedReferenceCount::init_type();
		register_type(_type_handle, "BulletShape",
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
//     Function: BulletShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletShape::~BulletShape()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletShape::is_polyhedral
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletShape::is_polyhedral() const
{

	return ptr()->isPolyhedral();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletShape::is_convex
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletShape::is_convex() const
{

	return ptr()->isConvex();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletShape::is_convex_2d
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletShape::is_convex_2d() const
{

	return ptr()->isConvex2d();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletShape::is_concave
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletShape::is_concave() const
{

	return ptr()->isConcave();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletShape::is_infinite
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletShape::is_infinite() const
{

	return ptr()->isInfinite();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletShape::is_non_moving
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletShape::is_non_moving() const
{

	return ptr()->isNonMoving();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletShape::is_soft_body
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletShape::is_soft_body() const
{

	return ptr()->isSoftBody();
}
}

#endif // __BULLET_SHAPE_H__
