// Filename: bulletPlaneShape.h
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

#ifndef __BULLET_PLANE_SHAPE_H__
#define __BULLET_PLANE_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletShape.h"

#include <collisionPlane.h>
#include <luse.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletPlaneShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletPlaneShape: public BulletShape
{

public:
	BulletPlaneShape(const LVector3 &normal, PN_stdfloat constant);
	inline BulletPlaneShape(const BulletPlaneShape &copy);
	inline void operator =(const BulletPlaneShape &copy);
	inline ~BulletPlaneShape();

	inline LVector3 get_plane_normal() const;
	inline PN_stdfloat get_plane_constant() const;

	static BulletPlaneShape *make_from_solid(const CollisionPlane *solid);

public:
	virtual btCollisionShape *ptr() const;

private:
	btStaticPlaneShape *_shape;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletPlaneShape",
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
//     Function: BulletPlaneShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletPlaneShape::~BulletPlaneShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletPlaneShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletPlaneShape::BulletPlaneShape(const BulletPlaneShape &copy) :
		_shape(copy._shape)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletPlaneShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletPlaneShape::operator =(const BulletPlaneShape &copy)
{
	_shape = copy._shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletPlaneShape::get_plane_constant
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletPlaneShape::get_plane_constant() const
{

	return (PN_stdfloat) _shape->getPlaneConstant();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletPlaneShape::get_plane_normal
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletPlaneShape::get_plane_normal() const
{

	return btVector3_to_LVector3(_shape->getPlaneNormal());
}
}

#endif // __BULLET_PLANE_SHAPE_H__
