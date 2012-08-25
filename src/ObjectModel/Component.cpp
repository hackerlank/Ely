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
		mOwnerObject(NULL), mCallbacksLoaded(false), mCallbacksRegistered(false)
{
	mCallbackTable.clear();
}

Component::~Component()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//unregister callbacks if any
	unregisterEventCallbacks();
	//unload event callbacks
	unloadEventCallbacks();
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

std::string Component::replaceCharacter(const std::string& source,
		int character, int replacement)
{
	int len = source.size() + 1;
	char* dest = new char[len];
	strncpy(dest, source.c_str(), len);
	//replace hyphens
	char* pch;
	pch = strchr(dest, character);
	while (pch != NULL)
	{
		*pch = replacement;
		pch = strchr(pch + 1, character);
	}
	std::string outStr(dest);
	delete[] dest;
	return outStr;
}

#ifdef WIN32
void Component::loadEventCallbacks()
{
}

void Component::unloadEventCallbacks()
{
}
#else
void Component::loadEventCallbacks()
{
	//if callbacks loaded do nothing
	if (mCallbacksLoaded)
	{
		return;
	}
	mCallbackLib = NULL;
	//load the event callbacks library
	mCallbackLib = dlopen(CALLBACKS_SO, RTLD_LAZY);
	if (not mCallbackLib)
	{
		std::cerr << "Error loading library: " << dlerror() << std::endl;
		return;
	}
	// reset errors
	dlerror();
	//check the default callback
	PCALLBACK pDefaultCallback = (PCALLBACK) dlsym(mCallbackLib,
			DEFAULT_CALLBACK);
	const char* dlsymError = dlerror();
	if (dlsymError)
	{
		std::cerr << "Cannot find default callback " << DEFAULT_CALLBACK
				<< dlsymError << std::endl;
		//Close the event callbacks library
		if (dlclose(mCallbackLib) != 0)
		{
			std::cerr << "Error closing library: " << CALLBACKS_SO << std::endl;
		}
		return;
	}
	//load every callback
	std::map<std::string, PCALLBACK>::iterator iter;
	for (iter = mCallbackTable.begin(); iter != mCallbackTable.end(); ++iter)
	{
		//reset errors
		dlerror();
		//load the variable whose value is the name
		//of the callback: <EVENT>_<COMPONENTTYPE>_<OBJECTID>
		std::string variableTmp = (iter->first) + "_"
				+ std::string(componentType()) + "_"
				+ std::string(mOwnerObject->objectId());
		//replace hyphens
		std::string variableName = replaceCharacter(variableTmp, '-', '_');
		PCALLBACKNAME pCallbackName = (PCALLBACKNAME) dlsym(mCallbackLib,
				variableName.c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			PRINTERR(
					"Cannot load variable " << variableName << ": " << dlsymError);
			//set default callback for this event
			mCallbackTable[iter->first] = pDefaultCallback;
			//continue with the next event
			continue;
		}
		//reset errors
		dlerror();
		//load the callback
		PCALLBACK pCallback = (PCALLBACK) dlsym(mCallbackLib,
				pCallbackName->c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			PRINTERR(
					"Cannot load callback " << pCallbackName << ": " << dlsymError);
			//set default callback for this event
			mCallbackTable[iter->first] = pDefaultCallback;
			//continue with the next event
			continue;
		}
		//set callback for this event
		mCallbackTable[iter->first] = pCallback;
	}
	//callbacks loaded
	mCallbacksLoaded = true;
}

void Component::unloadEventCallbacks()
{
	//if callbacks not loaded do nothing
	if (not mCallbacksLoaded)
	{
		return;
	}
	mCallbackTable.clear();
	//Close the event callbacks library
	if (dlclose(mCallbackLib) != 0)
	{
		std::cerr << "Error closing library: " << CALLBACKS_SO << std::endl;
	}
	//callbacks unloaded
	mCallbacksLoaded = false;
}
#endif

void Component::setupEvents()
{
	mCallbackTable.clear();
	//setup events (if any)
	std::list<std::string>::iterator iter;
	std::list<std::string> eventList = mTmpl->parameterList(
			std::string("events"));
	if (not eventList.empty())
	{
		//populate the callback table with NULL for each event
		for (iter = eventList.begin(); iter != eventList.end(); ++iter)
		{
			std::pair<std::string, PCALLBACK> tableItem(*iter, NULL);
			mCallbackTable.insert(tableItem);
		}
	}
}

void Component::registerEventCallbacks()
{
	if (mCallbackTable.empty() or (not mOwnerObject) or mCallbacksRegistered)
	{
		return;
	}
	//try to load event callbacks if any
	loadEventCallbacks();
	//if callbacks not loaded or owner object not defined or
	//callbacks already registered do nothing
	if (not mCallbacksLoaded)
	{
		return;
	}
	//register every handler
	std::map<std::string, PCALLBACK>::iterator iter;
	for (iter = mCallbackTable.begin(); iter != mCallbackTable.end(); ++iter)
	{
		//first==event, second==handler
		mTmpl->pandaFramework()->define_key(iter->first, iter->first,
				iter->second, (void*) this);
	}
	//handlers registered
	mCallbacksRegistered = true;
}

void Component::unregisterEventCallbacks()
{
	//if callbacks not loaded or owner object not defined or
	//callbacks not registered do nothing
	if (mCallbackTable.empty() or (not mCallbacksLoaded) or (not mOwnerObject)
			or (not mCallbacksRegistered))
	{
		return;
	}
	//Unregister the handlers
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) this);
	//handlers unregistered
	mCallbacksRegistered = false;
}

ReMutex& Component::getMutex()
{
	return mMutex;
}

//TypedObject semantics: hardcoded
TypeHandle Component::_type_handle;

