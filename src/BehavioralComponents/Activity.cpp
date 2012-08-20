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
 * \file /Ely/src/BehavioralComponents/Activity.cpp
 *
 * \date 17/ago/2012 (09:28:59)
 * \author marco
 */

#include "BehavioralComponents/Activity.h"
#include "BehavioralComponents/ActivityTemplate.h"

Activity::Activity()
{
	// TODO Auto-generated constructor stub
}

Activity::Activity(ActivityTemplate* tmpl)
{
	mTmpl = tmpl;
}

Activity::~Activity()
{
	// TODO Auto-generated destructor stub
}

const ComponentFamilyType Activity::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Activity::componentType() const
{
	return mTmpl->componentType();
}

bool Activity::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//setup event callbacks if any
	setupEventCallbacks();
	//
	return result;
}

void Activity::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set the node path of the object to the
	//node path of this model
	mOwnerObject->setNodePath(mNodePath);
	//register event callbacks if any
	registerEventCallbacks();
}

NodePath Activity::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void Activity::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

//TypedObject semantics: hardcoded
TypeHandle Activity::_type_handle;

