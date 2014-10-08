// Filename: bulletConeShape.h
// Created by:  enn0x (24Jan10)
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

#ifndef __BULLET_CONE_SHAPE_H__
#define __BULLET_CONE_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletShape.h"

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletConeShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletConeShape: public BulletShape
{

public:
	BulletConeShape(PN_stdfloat radius, PN_stdfloat height, BulletUpAxis up =
			Z_up);
	inline BulletConeShape(const BulletConeShape &copy);
	inline void operator =(const BulletConeShape &copy);
	inline ~BulletConeShape();

	inline PN_stdfloat get_radius() const;
	inline PN_stdfloat get_height() const;

public:
	virtual btCollisionShape *ptr() const;

private:
	btConeShape *_shape;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletConeShape",
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
//     Function: BulletConeShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletConeShape::~BulletConeShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConeShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletConeShape::BulletConeShape(const BulletConeShape &copy) :
		_shape(copy._shape)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConeShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletConeShape::operator =(const BulletConeShape &copy)
{
	_shape = copy._shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConeShape::get_radius
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletConeShape::get_radius() const
{

	return (PN_stdfloat) _shape->getRadius();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConeShape::get_height
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletConeShape::get_height() const
{

	return (PN_stdfloat) _shape->getHeight();
}
}
#endif // __BULLET_CONE_SHAPE_H__
