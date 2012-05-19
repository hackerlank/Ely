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
 * \file /Ely/src/ObjectModel/Object.cpp
 *
 * \date 07/mag/2012 (18:10:37)
 * \author marco
 */

#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

Object::Object(const ObjectId& objectId)
{
	mObjectId = objectId;
}

Object::~Object()
{
}

ObjectId& Object::objectId()
{
	return mObjectId;
}

void Object::clearComponents()
{
	mComponents.clear();
}

Component* Object::getComponent(const ComponentFamilyId& familyID)
{
	ComponentTable::iterator it = mComponents.find(familyID);
	if (it == mComponents.end())
	{
		return NULL;
	}
	return (*it).second;
}

PT(Component) Object::addComponent(Component* newComponent)
{
	if (not newComponent)
	{
		throw GameException("Object::setComponent: NULL new Component");
	}
	PT(Component) previousComp(NULL);
	ComponentFamilyId familyId = newComponent->familyID();
	ComponentTable::iterator it = mComponents.find(familyId);
	if (it != mComponents.end())
	{
		// a previous component of that family already existed
		previousComp = (*it).second;
		mComponents.erase(it);
	}
	//set the component owner
	newComponent->ownerObject() = this;
	//insert the new component into the table
	mComponents[familyId] = PT(Component)(newComponent);
	return previousComp;
}
