// Filename: bulletContactCallbackData.h
// Created by:  enn0x (22Nov12)
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

#ifndef __BULLET_CONTACT_CALLBACK_DATA_H__
#define __BULLET_CONTACT_CALLBACK_DATA_H__

#include <pandabase.h>
#include <callbackData.h>
#include <callbackObject.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletManifoldPoint.h"

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletContactCallbackData
// Description : 
////////////////////////////////////////////////////////////////////
class BulletContactCallbackData: public CallbackData
{

public:
	inline BulletContactCallbackData(BulletManifoldPoint &mp, PandaNode *node0,
			PandaNode *node1, int id0, int id1, int index0, int index1);

	inline BulletManifoldPoint &get_manifold() const;
	inline PandaNode *get_node0() const;
	inline PandaNode *get_node1() const;
	inline int get_part_id0() const;
	inline int get_part_id1() const;
	inline int get_index0() const;
	inline int get_index1() const;

private:
	BulletManifoldPoint &_mp;
	PandaNode *_node0;
	PandaNode *_node1;
	int _id0;
	int _id1;
	int _index0;
	int _index1;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		CallbackData::init_type();
		register_type(_type_handle, "BulletContactCallbackData",
				CallbackData::get_class_type());
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
//     Function: BulletContactCallbackData::Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletContactCallbackData::BulletContactCallbackData(
		BulletManifoldPoint &mp, PandaNode *node0, PandaNode *node1, int id0,
		int id1, int index0, int index1) :
		_mp(mp), _node0(node0), _node1(node1), _id0(id0), _id1(id1), _index0(
				index0), _index1(index1)
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactCallbackData::get_manifold
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletManifoldPoint &BulletContactCallbackData::get_manifold() const
{

	return _mp;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactCallbackData::get_node0
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletContactCallbackData::get_node0() const
{

	return _node0;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactCallbackData::get_node1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletContactCallbackData::get_node1() const
{

	return _node1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactCallbackData::get_part_id0
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletContactCallbackData::get_part_id0() const
{

	return _id0;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactCallbackData::get_part_id1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletContactCallbackData::get_part_id1() const
{

	return _id1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactCallbackData::get_index0
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletContactCallbackData::get_index0() const
{

	return _index0;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactCallbackData::get_index1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletContactCallbackData::get_index1() const
{

	return _index1;
}

}
#endif // __BULLET_CONTACT_CALLBACK_DATA_H__
