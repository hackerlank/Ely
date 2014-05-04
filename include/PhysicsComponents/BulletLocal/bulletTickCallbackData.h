// Filename: bulletTickCallbackData.h
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

#ifndef __BULLET_TICK_CALLBACK_DATA_H__
#define __BULLET_TICK_CALLBACK_DATA_H__

#include <pandabase.h>
#include <callbackData.h>
#include <callbackObject.h>

#include "bullet_includes.h"

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletTickCallbackData
// Description : 
////////////////////////////////////////////////////////////////////
class BulletTickCallbackData: public CallbackData
{

public:
	inline BulletTickCallbackData(btScalar timestep);

	inline PN_stdfloat get_timestep() const;

private:
	btScalar _timestep;

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		CallbackData::init_type();
		register_type(_type_handle, "BulletTickCallbackData",
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
//     Function: BulletTickCallbackData::Constructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletTickCallbackData::BulletTickCallbackData(btScalar timestep) :
		_timestep(timestep)
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletTickCallbackData::get_timestep
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletTickCallbackData::get_timestep() const
{

	return (PN_stdfloat) _timestep;
}
}
#endif // __BULLET_TICK_CALLBACK_DATA_H__
