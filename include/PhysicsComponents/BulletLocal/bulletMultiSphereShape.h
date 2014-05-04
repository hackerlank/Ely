// Filename: bulletMultiSphereShape.h
// Created by:  enn0x (04Jan12)
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

#ifndef __BULLET_MULTI_SPHERE_SHAPE_H__
#define __BULLET_MULTI_SPHERE_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bulletShape.h"

#include <pta_LVecBase3.h>
#include <pta_stdfloat.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletMultiSphereShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletMultiSphereShape: public BulletShape
{

public:
	BulletMultiSphereShape(const PTA_LVecBase3 &points,
			const PTA_stdfloat &radii);
	inline BulletMultiSphereShape(const BulletMultiSphereShape &copy);
	inline void operator =(const BulletMultiSphereShape &copy);
	inline ~BulletMultiSphereShape();

	inline int get_sphere_count() const;
	inline LPoint3 get_sphere_pos(int index) const;
	inline PN_stdfloat get_sphere_radius(int index) const;

public:
	virtual btCollisionShape *ptr() const;

private:
	btMultiSphereShape *_shape;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletMultiSphereShape",
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
//     Function: BulletMultiSphereShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletMultiSphereShape::~BulletMultiSphereShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMultiSphereShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletMultiSphereShape::BulletMultiSphereShape(
		const BulletMultiSphereShape &copy) :
		_shape(copy._shape)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMultiSphereShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletMultiSphereShape::operator =(
		const BulletMultiSphereShape &copy)
{
	_shape = copy._shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMultiSphereShape::get_sphere_count
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletMultiSphereShape::get_sphere_count() const
{

	return _shape->getSphereCount();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMultiSphereShape::get_sphere_pos
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LPoint3 BulletMultiSphereShape::get_sphere_pos(int index) const
{

	nassertr(index >= 0 && index < _shape->getSphereCount(), LPoint3::zero());
	return btVector3_to_LPoint3(_shape->getSpherePosition(index));
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMultiSphereShape::get_sphere_radius
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletMultiSphereShape::get_sphere_radius(int index) const
{

	nassertr(index >= 0 && index < _shape->getSphereCount(), 0.0);
	return (PN_stdfloat) _shape->getSphereRadius(index);
}
}

#endif // __BULLET_MULTI_SPHERE_SHAPE_H__
