// Filename: bulletSphereShape.h
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

#ifndef __BULLET_SPHERE_SHAPE_H__
#define __BULLET_SPHERE_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletShape.h"

#include <collisionSphere.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletSphereShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletSphereShape: public BulletShape
{

public:
	BulletSphereShape(PN_stdfloat radius);
	inline BulletSphereShape(const BulletSphereShape &copy);
	inline void operator =(const BulletSphereShape &copy);
	inline ~BulletSphereShape();

	inline PN_stdfloat get_radius() const;

	static BulletSphereShape *make_from_solid(const CollisionSphere *solid);

public:
	virtual btCollisionShape *ptr() const;

private:
	btSphereShape *_shape;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletSphereShape",
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
//     Function: BulletSphereShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletSphereShape::~BulletSphereShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSphereShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletSphereShape::BulletSphereShape(const BulletSphereShape &copy) :
		_shape(copy._shape)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSphereShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletSphereShape::operator =(const BulletSphereShape &copy)
{
	_shape = copy._shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSphereShape::get_radius
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSphereShape::get_radius() const
{

	return _shape->getRadius();
}

}
#endif // __BULLET_SPHERE_SHAPE_H__
