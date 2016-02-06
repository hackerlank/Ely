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
 * \file /Ely/src/Support/particles/ForceGroup.cpp
 *
 * \date 2016-02-04
 * \author consultit
 */

#include "ForceGroup.h"
#include "ParticleEffect.h"
#include "GameParticlesManager.h"

namespace ely
{

unsigned int ForceGroup::id = 1;

ForceGroup::ForceGroup(const std::string& _name) :
		fEnabled(false), particleEffect(NULL)
{
	CHECK_EXISTENCE_DEBUG(GameParticlesManager::GetSingletonPtr(),
			"GameParticlesManager::GameParticlesManager: invalid GameParticlesManager")
	{
		HOLD_REMUTEX(ForceGroup::mMutexId)

		name = _name;
		if (name == "")
		{
			name =
					std::string("ForceGroup-")
							+ dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(
									id)).str();
			id++;
		}
	}
	//
	node = new ForceNode(name);
	nodePath = NodePath(node);
}

ForceGroup::~ForceGroup()
{
}

void ForceGroup::cleanup()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	node.clear();
	nodePath.remove_node();
	nodePath = NodePath();
	particleEffect = NULL;
}

void ForceGroup::enable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	for (int i = 0; i < node->get_num_forces(); ++i)
	{
		SMARTPTR(BaseForce) f = node->get_force(i);
		f->set_active(true);
	}
	fEnabled = true;
}

void ForceGroup::disable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	for (int i = 0; i < node->get_num_forces(); ++i)
	{
		SMARTPTR(BaseForce) f = node->get_force(i);
		f->set_active(false);
	}
	fEnabled = false;
}

void ForceGroup::addForce(SMARTPTR(BaseForce) force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	node->add_force(force);
	if (particleEffect)
	{
		particleEffect->addForce(force);
	}
}

void ForceGroup::removeForce(SMARTPTR(BaseForce) force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	node->remove_force(force);
	if (particleEffect != NULL)
	{
		particleEffect->removeForce(force);
	}
}

SMARTPTR(BaseForce) ForceGroup::operator[](int index)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	int numForces = node->get_num_forces();
	RETURN_ON_COND((index < 0) or (index >= numForces), NULL)
	return node->get_force(index);
}

std::list<SMARTPTR(BaseForce)> ForceGroup::asList()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(BaseForce)> l;
	for (int i = 0; i < node->get_num_forces(); ++i)
	{
		l.push_back(node->get_force(i));
	}
	return l;
}

//TypedObject semantics: hardcoded
TypeHandle ForceGroup::_type_handle;

}
/* namespace ely */
