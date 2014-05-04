// Filename: bulletHelper.h
// Created by:  enn0x (19Jan11)
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

#ifndef __BULLET_HELPER_H__
#define __BULLET_HELPER_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletSoftBodyNode.h"

#include <collisionNode.h>
#include <nodePath.h>
#include <nodePathCollection.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletHelper
// Description : 
////////////////////////////////////////////////////////////////////
class BulletHelper
{

public:

	// Collision shapes
	static NodePathCollection from_collision_solids(NodePath &np, bool clear =
			false);

	// Internal names
	inline static PT(InternalName)get_sb_index();
	inline static PT(InternalName) get_sb_flip();

	// Geom vertex data
	static CPT(GeomVertexFormat) add_sb_index_column(const GeomVertexFormat *format);
	static CPT(GeomVertexFormat) add_sb_flip_column(const GeomVertexFormat *format);

	// Geom utils
	static PT(Geom) make_geom_from_faces(BulletSoftBodyNode *node,
			const GeomVertexFormat *format=NULL,
			bool two_sided=false);

	static PT(Geom) make_geom_from_links(BulletSoftBodyNode *node,
			const GeomVertexFormat *format=NULL);

	static void make_texcoords_for_patch(Geom *geom, int resx, int resy);

private:
	static PT(InternalName) _sb_index;
	static PT(InternalName) _sb_flip;

	static bool is_tangible(CollisionNode *cnode);

	static PT(Geom) make_geom(BulletSoftBodyNode *node,
			const GeomVertexFormat *format,
			bool use_faces,
			bool two_sided);
};

////////////////////////////////////////////////////////////////////
//     Function: BulletHelper::get_sb_index
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PT(InternalName)BulletHelper::
get_sb_index()
{

	if (_sb_index == (InternalName *)NULL)
	{
		_sb_index = InternalName::make("sb_index");
	}
	return _sb_index;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletHelper::get_sb_flip
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PT(InternalName)BulletHelper::
get_sb_flip()
{

	if (_sb_flip == (InternalName *)NULL)
	{
		_sb_flip = InternalName::make("sb_flip");
	}
	return _sb_flip;
}
}
#endif // __BULLET_HELPER_H__
