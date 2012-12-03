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
 * \file /Ely/src/AIComponents/Steering.cpp
 *
 * \date 03/dic/2012 (13:39:59)
 * \author marco
 */

#include "AIComponents/Steering.h"
#include "AIComponents/SteeringTemplate.h"

Steering::Steering()
{
	// TODO Auto-generated constructor stub
}

Steering::Steering(SMARTPTR(SteeringTemplate)tmpl)
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"Steering::Steering: invalid GameAIManager")
	mTmpl = tmpl;
}

Steering::~Steering()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath.remove_node();
}

const ComponentFamilyType Steering::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Steering::componentType() const
{
	return mTmpl->componentType();
}

bool Steering::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//
	return result;
}

void Steering::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

}

NodePath Steering::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void Steering::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

//TypedObject semantics: hardcoded
TypeHandle Steering::_type_handle;
