// Filename: bulletFilterCallbackData.h
// Created by:  enn0x (26Nov12)
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

#ifndef __BULLET_FILTER_CALLBACK_DATA_H__
#define __BULLET_FILTER_CALLBACK_DATA_H__

#include <pandabase.h>
#include <callbackData.h>
#include <callbackObject.h>

#include "bullet_includes.h"
#include "bullet_utils.h"

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletFilterCallbackData
// Description : 
////////////////////////////////////////////////////////////////////
class BulletFilterCallbackData: public CallbackData
{

public:
	inline BulletFilterCallbackData(PandaNode *node0, PandaNode *node1);

	inline PandaNode *get_node_0() const;
	inline PandaNode *get_node_1() const;

	inline void set_collide(bool collide);
	inline bool get_collide() const;

private:
	PandaNode *_node0;
	PandaNode *_node1;
	bool _collide;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		CallbackData::init_type();
		register_type(_type_handle, "BulletFilterCallbackData",
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
//     Function: BulletFilterCallbackData::Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletFilterCallbackData::BulletFilterCallbackData(PandaNode *node0,
		PandaNode *node1) :
		_node0(node0), _node1(node1), _collide(false)
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletFilterCallbackData::get_node_0
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletFilterCallbackData::get_node_0() const
{

	return _node0;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletFilterCallbackData::get_node_1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletFilterCallbackData::get_node_1() const
{

	return _node1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletFilterCallbackData::get_collide
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletFilterCallbackData::get_collide() const
{

	return _collide;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletFilterCallbackData::set_collide
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletFilterCallbackData::set_collide(bool collide)
{

	_collide = collide;
}
}

#endif // __BULLET_FILTER_CALLBACK_DATA_H__
