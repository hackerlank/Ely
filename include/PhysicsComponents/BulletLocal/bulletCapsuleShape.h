// Filename: bulletCapsuleShape.h
// Created by:  enn0x (27Jan10)
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

#ifndef __BULLET_CAPSULE_SHAPE_H__
#define __BULLET_CAPSULE_SHAPE_H__

#include <pandabase.h>

#include <bullet_includes.h>
#include <bullet_utils.h>
#include <bulletShape.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletCapsuleShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletCapsuleShape: public BulletShape
{

public:
	BulletCapsuleShape(PN_stdfloat radius, PN_stdfloat height, BulletUpAxis up =
			Z_up);
	inline BulletCapsuleShape(const BulletCapsuleShape &copy);
	inline void operator =(const BulletCapsuleShape &copy);
	inline ~BulletCapsuleShape();

	inline PN_stdfloat get_radius() const;
	inline PN_stdfloat get_half_height() const;

public:
	virtual btCollisionShape *ptr() const;

private:
	btCapsuleShape *_shape;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletCapsuleShape",
				BulletShape::get_class_type());
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
//     Function: BulletCapsuleShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletCapsuleShape::~BulletCapsuleShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCapsuleShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletCapsuleShape::BulletCapsuleShape(const BulletCapsuleShape &copy) :
		_shape(copy._shape)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCapsuleShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletCapsuleShape::operator =(const BulletCapsuleShape &copy)
{
	_shape = copy._shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCapsuleShape::get_radius
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletCapsuleShape::get_radius() const
{

	return (PN_stdfloat) _shape->getRadius();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCapsuleShape::get_half_height
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletCapsuleShape::get_half_height() const
{

	return (PN_stdfloat) _shape->getHalfHeight();
}
}

#endif // __BULLET_CAPSULE_SHAPE_H__
