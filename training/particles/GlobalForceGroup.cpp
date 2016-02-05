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
 * \file /Ely/src/Support/particles/GlobalForceGroup.cpp
 *
 * \date 2016-02-04
 * \author consultit
 */

#include "GlobalForceGroup.h"
#include <physicsManager.h>

namespace ely
{

GlobalForceGroup::GlobalForceGroup(std::string name) :
		ForceGroup(name)
{
	///TODO
	physicsMgr = NULL; //TOBE inizialized from global value
	addAngularIntegrator = NULL; //TOBE inizialized from global value see ShowBase.addAngularIntegrator()
	///
}

GlobalForceGroup::~GlobalForceGroup()
{
}

void GlobalForceGroup::addForce(SMARTPTR(BaseForce)force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ForceGroup::addForce(force);
	if (force->is_linear() == false)
	{
		// Physics manager will need an angular integrator
		addAngularIntegrator();
	}
	if (force->is_linear() == true)
	{
		physicsMgr->add_linear_force(dynamic_cast<LinearForce*>(force.p()));
	}
	else
	{
		physicsMgr->add_angular_force(dynamic_cast<AngularForce*>(force.p()));
	}
}

void GlobalForceGroup::removeForce(SMARTPTR(BaseForce)force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ForceGroup::removeForce(force);
	if (force->is_linear() == true)
	{
		physicsMgr->remove_linear_force(dynamic_cast<LinearForce*>(force.p()));
	}
	else
	{
		physicsMgr->remove_angular_force(dynamic_cast<AngularForce*>(force.p()));
	}
}

//TypedObject semantics: hardcoded
TypeHandle GlobalForceGroup::_type_handle;

}
/* namespace ely */
