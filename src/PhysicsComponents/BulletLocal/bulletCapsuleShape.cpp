// Filename: bulletCapsuleShape.cxx
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

#include "Utilities/Tools.h"

#include "PhysicsComponents/BulletLocal/bulletCapsuleShape.h"

namespace ely
{
TypeHandle BulletCapsuleShape::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: BulletCapsuleShape::Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
BulletCapsuleShape::BulletCapsuleShape(PN_stdfloat radius, PN_stdfloat height,
		BulletUpAxis up)
{

	switch (up)
	{
	case X_up:
		_shape = new btCapsuleShapeX(radius, height);
		break;
	case Y_up:
		_shape = new btCapsuleShape(radius, height);
		break;
	case Z_up:
		_shape = new btCapsuleShapeZ(radius, height);
		break;
	default:
		PRINT_ERR_DEBUG("invalid up-axis:" << up);
		break;
	}

	_shape->setUserPointer(this);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletCapsuleShape::ptr
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
btCollisionShape *BulletCapsuleShape::ptr() const
{

	return _shape;
}

}