// Filename: bulletMinkowskiSumShape.h
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

#ifndef __BULLET_MINKOWSKI_SUM_SHAPE_H__
#define __BULLET_MINKOWSKI_SUM_SHAPE_H__

#include <pandabase.h>

#include "Utilities/Tools.h"
#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletShape.h"

#include <transformState.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletMinkowskiSumShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletMinkowskiSumShape: public BulletShape
{

public:
	BulletMinkowskiSumShape(const BulletShape *shape_a,
			const BulletShape *shape_b);
	inline BulletMinkowskiSumShape(const BulletMinkowskiSumShape &copy);
	inline void operator =(const BulletMinkowskiSumShape &copy);
	inline ~BulletMinkowskiSumShape();

	inline void set_transform_a(const TransformState *ts);
	inline void set_transform_b(const TransformState *ts);
	inline CPT(TransformState)get_transform_a() const;
	inline CPT(TransformState) get_transform_b() const;

	inline const BulletShape *get_shape_a() const;
	inline const BulletShape *get_shape_b() const;

	inline PN_stdfloat get_margin() const;

public:
	virtual btCollisionShape *ptr() const;

private:
	btMinkowskiSumShape *_shape;

	CPT(BulletShape) _shape_a;
	CPT(BulletShape) _shape_b;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletMinkowskiSumShape",
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
//     Function: BulletMinkowskiSumShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletMinkowskiSumShape::~BulletMinkowskiSumShape()
{

	delete _shape;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletMinkowskiSumShape::BulletMinkowskiSumShape(
		const BulletMinkowskiSumShape &copy) :
		_shape(copy._shape), _shape_a(copy._shape_a), _shape_b(copy._shape_b)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletMinkowskiSumShape::operator =(
		const BulletMinkowskiSumShape &copy)
{
	_shape = copy._shape;
	_shape_a = copy._shape_a;
	_shape_b = copy._shape_b;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::set_transform_a
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletMinkowskiSumShape::set_transform_a(const TransformState *ts)
{

	RETURN_ON_COND(not(ts),)
	_shape->setTransformA(TransformState_to_btTrans(ts));
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::set_transform_b
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletMinkowskiSumShape::set_transform_b(const TransformState *ts)
{

	RETURN_ON_COND(not(ts),)
	_shape->setTransformB(TransformState_to_btTrans(ts));
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::get_transform_a
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline CPT(TransformState)BulletMinkowskiSumShape::
get_transform_a() const
{

	return btTrans_to_TransformState(_shape->getTransformA());
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::get_transform_b
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline CPT(TransformState)BulletMinkowskiSumShape::
get_transform_b() const
{

	return btTrans_to_TransformState(_shape->GetTransformB());
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::get_shape_a
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline const BulletShape *BulletMinkowskiSumShape::get_shape_a() const
{

	return _shape_a;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::get_shape_b
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline const BulletShape *BulletMinkowskiSumShape::get_shape_b() const
{

	return _shape_b;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletMinkowskiSumShape::get_margin
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletMinkowskiSumShape::get_margin() const
{

	return (PN_stdfloat) _shape->getMargin();
}
}

#endif // __BULLET_MINKOWSKI_SUM_SHAPE_H__
