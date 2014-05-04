// Filename: bulletConvexPointCloudShape.h
// Created by:  enn0x (30Jan10)
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

#ifndef __BULLET_CONVEX_POINT_CLOUD_SHAPE_H__
#define __BULLET_CONVEX_POINT_CLOUD_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bulletShape.h"

#include <geom.h>
#include <pta_LVecBase3.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletConvexPointCloudShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletConvexPointCloudShape: public BulletShape
{

public:
	BulletConvexPointCloudShape(const PTA_LVecBase3 &points, LVecBase3 scale =
			LVecBase3(1.));
	BulletConvexPointCloudShape(const Geom *geom,
			LVecBase3 scale = LVecBase3(1.));
	inline BulletConvexPointCloudShape(const BulletConvexPointCloudShape &copy);
	inline void operator =(const BulletConvexPointCloudShape &copy);
	inline ~BulletConvexPointCloudShape();

	inline int get_num_points() const;

public:
	virtual btCollisionShape *ptr() const;

private:
	btConvexPointCloudShape *_shape;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletConvexPointCloudShape",
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
//     Function: BulletConvexPointCloudShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletConvexPointCloudShape::~BulletConvexPointCloudShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConvexPointCloudShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletConvexPointCloudShape::BulletConvexPointCloudShape(
		const BulletConvexPointCloudShape &copy) :
		_shape(copy._shape)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConvexPointCloudShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletConvexPointCloudShape::operator =(
		const BulletConvexPointCloudShape &copy)
{
	_shape = copy._shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletConvexPointCloudShape::get_num_points
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletConvexPointCloudShape::get_num_points() const
{

	return _shape->getNumPoints();
}
}
#endif // __BULLET_CONVEX_POINT_CLOUD_SHAPE_H__
