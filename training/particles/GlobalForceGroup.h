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
 * \file /Ely/include/Support/particles/GlobalForceGroup.h
 *
 * \date 2016-02-04
 * \author consultit
 */
#ifndef GLOBALFORCEGROUP_H_
#define GLOBALFORCEGROUP_H_

#include "ForceGroup.h"
#include <physicsManager.h>

namespace ely
{

class GlobalForceGroup: public ForceGroup
{
private:
	PhysicsManager* physicsMgr;
	//
#ifdef ELY_THREAD
	///The mutex associated with this Component.
	ReMutex mMutex;
#endif

public:
	GlobalForceGroup(const std::string& name = std::string());
	virtual ~GlobalForceGroup();

	virtual void addForce(SMARTPTR(BaseForce) force);
	virtual void removeForce(SMARTPTR(BaseForce) force);

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		ForceGroup::init_type();
		register_type(_type_handle, "GlobalForceGroup", ForceGroup::get_class_type());
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

///inline definitions

#ifdef ELY_THREAD
inline ReMutex& GlobalForceGroup::getMutex()
{
	return mMutex;
}
#endif

}
/* namespace ely */

#endif /* GLOBALFORCEGROUP_H_ */
