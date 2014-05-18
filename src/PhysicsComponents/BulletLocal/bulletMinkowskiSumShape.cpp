// Filename: bulletMinkowskiSumShape.cxx
// Created by:  enn0x (15Aug13)
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

#include "PhysicsComponents/BulletLocal/bulletMinkowskiSumShape.h"

namespace ely
{
TypeHandle BulletMinkowskiSumShape::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
BulletMinkowskiSumShape::BulletMinkowskiSumShape(const BulletShape *shape_a,
		const BulletShape *shape_b)
{

	RETURN_ON_COND(not (shape_a->is_convex()),)
	RETURN_ON_COND(not (shape_b->is_convex()),)

	const btConvexShape *ptr_a = (const btConvexShape *) shape_a->ptr();
	const btConvexShape *ptr_b = (const btConvexShape *) shape_b->ptr();

	_shape = new btMinkowskiSumShape(ptr_a, ptr_b);
	_shape->setUserPointer(this);

	_shape_a = shape_a;
	_shape_b = shape_b;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::ptr
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
btCollisionShape *BulletMinkowskiSumShape::ptr() const
{

	return _shape;
}

}
