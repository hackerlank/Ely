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

Object::Object(const ObjectId& objectId, PandaFramework* mPandaFramework,
		WindowFramework* mWindowFramework)
{
	mObjectId = objectId;
	mPandaFramework = PandaFramework;
	mWindowFramework = WindowFramework;
}

Object::~Object()
{
	mNodePath.remove_node();
}

const ObjectId& Object::getObjectId() const
{
	return mObjectId;
}

void Object::setObjectId(ObjectId& objectId)
{
	mObjectId = objectId;
}

void Object::clearComponents()
{
	mComponents.clear();
}

NodePath& Object::getNodePath()
{
	return mNodePath;
}

void Object::setNodePath(NodePath& nodePath)
{
	mNodePath = nodePath;
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

PT(Component) Object::setComponent(Component* newComponent)
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
	//insert the new component
	mComponents[familyId] = PT(Component)(newComponent);
	return previousComp;
}

Object::operator NodePath()
{
	return mNodePath;
}

