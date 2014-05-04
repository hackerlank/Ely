// Filename: bulletCylinderShape.h
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

#ifndef __BULLET_CYLINDER_SHAPE_H__
#define __BULLET_CYLINDER_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletShape.h"

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletCylinderShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletCylinderShape: public BulletShape
{

public:
	BulletCylinderShape(PN_stdfloat radius, PN_stdfloat height,
			BulletUpAxis up = Z_up);
	BulletCylinderShape(const LVector3 &half_extents, BulletUpAxis up = Z_up);
	inline BulletCylinderShape(const BulletCylinderShape &copy);
	inline void operator =(const BulletCylinderShape &copy);
	inline ~BulletCylinderShape();

	inline PN_stdfloat get_radius() const;
	inline LVecBase3 get_half_extents_without_margin() const;
	inline LVecBase3 get_half_extents_with_margin() const;

public:
	virtual btCollisionShape *ptr() const;

private:
	btCylinderShape *_shape;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletCylinderShape",
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
//     Function: BulletCylinderShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletCylinderShape::~BulletCylinderShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCylinderShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletCylinderShape::BulletCylinderShape(const BulletCylinderShape &copy) :
		_shape(copy._shape)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCylinderShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletCylinderShape::operator =(const BulletCylinderShape &copy)
{
	_shape = copy._shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCylinderShape::get_radius
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletCylinderShape::get_radius() const
{

	return (PN_stdfloat) _shape->getRadius();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCylinderShape::get_half_extents_without_margin
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVecBase3 BulletCylinderShape::get_half_extents_without_margin() const
{

	return btVector3_to_LVecBase3(_shape->getHalfExtentsWithoutMargin());
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCylinderShape::get_half_extents_with_margin
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVecBase3 BulletCylinderShape::get_half_extents_with_margin() const
{

	return btVector3_to_LVecBase3(_shape->getHalfExtentsWithMargin());
}

}
#endif // __BULLET_CYLINDER_SHAPE_H__
