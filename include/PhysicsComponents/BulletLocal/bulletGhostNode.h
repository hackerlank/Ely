// Filename: bulletGhostNode.h
// Created by:  enn0x (19Nov10)
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

#ifndef __BULLET_GHOST_NODE_H__
#define __BULLET_GHOST_NODE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletBodyNode.h"

#include <pandaNode.h>
#include <collideMask.h>

namespace ely
{
class BulletShape;

////////////////////////////////////////////////////////////////////
//       Class : BulletGhostNode
// Description : 
////////////////////////////////////////////////////////////////////
class BulletGhostNode: public BulletBodyNode
{

public:
	BulletGhostNode(const char *name = "ghost");
	inline ~BulletGhostNode();

	// Overlapping
	inline int get_num_overlapping_nodes() const;
	inline PandaNode *get_overlapping_node(int idx) const;MAKE_SEQ(get_overlapping_nodes, get_num_overlapping_nodes, get_overlapping_node)
	;

public:
	virtual btCollisionObject *get_object() const;

	void sync_p2b();
	void sync_b2p();

protected:
	virtual void parents_changed();
	virtual void transform_changed();

private:
	CPT(TransformState)_sync;
	bool _sync_disable;
	bool _sync_local;

	btPairCachingGhostObject *_ghost;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletBodyNode::init_type();
		register_type(_type_handle, "BulletGhostNode",
				BulletBodyNode::get_class_type());
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
//     Function: BulletGhostNode::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletGhostNode::~BulletGhostNode()
{

	delete _ghost;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletGhostNode::get_num_overlapping_nodes
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletGhostNode::get_num_overlapping_nodes() const
{

	return _ghost->getNumOverlappingObjects();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletGhostNode::get_overlapping_node
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletGhostNode::get_overlapping_node(int idx) const
{

	nassertr(idx >= 0 && idx < _ghost->getNumOverlappingObjects(), NULL);

	btCollisionObject *object = _ghost->getOverlappingObject(idx);
	return (object) ? (PandaNode *) object->getUserPointer() : NULL;
}
}
#endif // __BULLET_GHOST_NODE_H__

