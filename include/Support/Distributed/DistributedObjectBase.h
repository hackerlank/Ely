/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/include/Support/Distributed/DistributedObjectBase.h
 *
 * \date 27/nov/2012 (17:13:08)
 * \author consultit
 */

#ifndef DISTRIBUTEDOBJECTBASE_H_
#define DISTRIBUTEDOBJECTBASE_H_

#include <typedWritableReferenceCount.h>

namespace ely
{
/**
 * The Distributed Object class is the base class for all network based
 * (i.e. distributed) objects.  These will usually (always?) have a
 * dclass entry in a *.dc file.
 */
class DistributedObjectBase: public TypedWritableReferenceCount
{
public:
	DistributedObjectBase();
	virtual ~DistributedObjectBase();

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedObject::init_type();
		register_type(_type_handle, "DistributedObjectBase", TypedObject::get_class_type());
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

} // namespace ely

#endif /* DISTRIBUTEDOBJECTBASE_H_ */
