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
 * \file /Ely/src/ObjectModel/Component.cpp
 *
 * \date 09/mag/2012 (16:09:17)
 * \author marco
 */

#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Object.h"

Component::Component() :
		mOwnerObject(NULL)
{
}

Component::~Component()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)
	//unregister events if any
	unRegisterEvents();
}

void Component::update(void* data)
{
}

AsyncTask::DoneStatus Component::update(GenericAsyncTask* task)
{
	return AsyncTask::DS_done;
}

void Component::setOwnerObject(Object* ownerObject)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mOwnerObject = ownerObject;
}

Object* Component::getOwnerObject() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mOwnerObject;
}

void Component::onAddToObjectSetup()
{
}

void Component::onAddToSceneSetup()
{
}

void Component::setComponentId(const ComponentId& componentId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mComponentId = componentId;
}

#ifdef WIN32
void Component::registerEvents()
{
}

void Component::unRegisterEvents()
{
}
#else
void Component::registerEvents()
{
	//register only if some events is present
	if (not mEventSet.empty()){
		//Open the event handlers library

	}
}

void Component::unRegisterEvents()
{

	if (not mEventSet.empty()){
		//Close the event handlers library
	}
}
#endif

ReMutex& Component::getMutex()
{
	return mMutex;
}

//TypedObject semantics: hardcoded
TypeHandle Component::_type_handle;

