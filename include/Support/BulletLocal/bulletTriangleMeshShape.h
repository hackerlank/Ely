// Filename: bulletTriangleMeshShape.h
// Created by:  enn0x (09Feb10)
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

#ifndef __BULLET_TRIANGLE_MESH_SHAPE_H__
#define __BULLET_TRIANGLE_MESH_SHAPE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bulletShape.h"

#include <luse.h>

namespace ely
{
class BulletTriangleMesh;

////////////////////////////////////////////////////////////////////
//       Class : BulletTriangleMeshShape
// Description : 
////////////////////////////////////////////////////////////////////
class BulletTriangleMeshShape: public BulletShape
{

public:
	BulletTriangleMeshShape(BulletTriangleMesh *mesh, bool dynamic,
			bool compress = true, bool bvh = true);
	inline BulletTriangleMeshShape(const BulletTriangleMeshShape &copy);
	inline void operator =(const BulletTriangleMeshShape &copy);
	inline ~BulletTriangleMeshShape();

	void refit_tree(const LPoint3 &aabb_min, const LPoint3 &aabb_max);

	inline bool is_static() const;
	inline bool is_dynamic() const;

public:
	virtual btCollisionShape *ptr() const;

private:
	btBvhTriangleMeshShape *_bvh_shape;
	btGImpactMeshShape *_gimpact_shape;

	PT(BulletTriangleMesh)_mesh;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletShape::init_type();
		register_type(_type_handle, "BulletTriangleMeshShape",
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
//     Function: BulletTriangleMeshShape::Copy Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletTriangleMeshShape::BulletTriangleMeshShape(
		const BulletTriangleMeshShape &copy) :
		_bvh_shape(copy._bvh_shape), _gimpact_shape(copy._gimpact_shape), _mesh(
				copy._mesh)
{
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTriangleMeshShape::Copy Assignment Operator
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletTriangleMeshShape::operator =(
		const BulletTriangleMeshShape &copy)
{

	_bvh_shape = copy._bvh_shape;
	_gimpact_shape = copy._gimpact_shape;
	_mesh = copy._mesh;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTriangleMeshShape::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletTriangleMeshShape::~BulletTriangleMeshShape()
{

	if (_bvh_shape)
	{
		delete _bvh_shape;
	}

	if (_gimpact_shape)
	{
		delete _gimpact_shape;
	}
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTriangleMeshShape::is_static
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletTriangleMeshShape::is_static() const
{

	return (_bvh_shape != NULL);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletTriangleMeshShape::is_dynamic
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletTriangleMeshShape::is_dynamic() const
{

	return (_gimpact_shape != NULL);
}
}
#endif // __BULLET_TRIANGLE_MESH_SHAPE_H__
