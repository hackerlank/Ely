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
 * \file /Ely/src/PhysicsComponents/Character.cpp
 *
 * \date 30/ott/2012 17:03:49
 * \author marco
 */

#include "PhysicsComponents/Character.h"
#include "PhysicsComponents/CharacterTemplate.h"

Character::Character()
{
	// TODO Auto-generated constructor stub
}

Character::Character(SMARTPTR(CharacterTemplate)tmpl)
{
	mTmpl = tmpl;
}

Character::~Character()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (GamePhysicsManager::GetSingletonPtr())
	{
		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
				DCAST(TypedObject, mCharacter));
	}
}

const ComponentFamilyType Character::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Character::componentType() const
{
	return mTmpl->componentType();
}

bool Character::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//
	return result;
}

void Character::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

}

void Character::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

}

NodePath Character::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void Character::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

//TypedObject semantics: hardcoded
TypeHandle Character::_type_handle;

