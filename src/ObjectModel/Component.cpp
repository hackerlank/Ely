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
 * \author consultit
 */

#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Object.h"

namespace ely
{

Component::Component() :
		mOwnerObject(NULL), mCallbacksLoaded(false), mCallbacksRegistered(false)
{
	mCallbackTable.clear();
}

Component::~Component()
{
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

void Component::setOwnerObject(SMARTPTR(Object)ownerObject)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mOwnerObject = ownerObject;
}

SMARTPTR(Object)Component::getOwnerObject() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mOwnerObject;
}

void Component::onAddToObjectSetup()
{
}

void Component::navMeshSetup()
{
}

void Component::setComponentId(const ComponentId& componentId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mComponentId = componentId;
}

ComponentId Component::getComponentId() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mComponentId;
}

void Component::loadEventCallbacks()
{
	//if callbacks loaded do nothing
	if (mCallbacksLoaded)
	{
		return;
	}
	mCallbackLib = NULL;
	// reset errors
	lt_dlerror();
	//load the event callbacks library
	mCallbackLib = lt_dlopen(CALLBACKS_LA);
	if (mCallbackLib == NULL)
	{
		std::cerr << "Error loading library: " << CALLBACKS_LA << ": "
				<< lt_dlerror() << std::endl;
		return;
	}
	// reset errors
	lt_dlerror();
	//check the default callback
	PCALLBACK pDefaultCallback = (PCALLBACK) lt_dlsym(mCallbackLib,
			DEFAULT_CALLBACK_NAME);
	const char* dlsymError = lt_dlerror();
	if (dlsymError)
	{
		std::cerr << "Cannot find default callback " << DEFAULT_CALLBACK_NAME
				<< dlsymError << std::endl;
		//Close the event callbacks library
		if (lt_dlclose(mCallbackLib) != 0)
		{
			std::cerr << "Error closing library: " << CALLBACKS_LA << std::endl;
		}
		return;
	}
	//load every callback
	std::map<std::string, PCALLBACK>::iterator iter;
	for (iter = mCallbackTable.begin(); iter != mCallbackTable.end(); ++iter)
	{
		//reset errors
		lt_dlerror();
		//load the variable whose value is the name
		//of the callback: <EVENTTYPE>_<COMPONENTTYPE>_<OBJECTTYPE>
		std::string variableTmp = (iter->first) + "_"
				+ std::string(componentType()) + "_"
				+ std::string(mOwnerObject->objectTmpl()->name());
		//replace hyphens
		std::string variableName = replaceCharacter(variableTmp, '-', '_');
		PCALLBACKNAME pCallbackName = (PCALLBACKNAME) lt_dlsym(mCallbackLib,
				variableName.c_str());
		dlsymError = lt_dlerror();
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
		lt_dlerror();
		//load the callback
		PCALLBACK pCallback = (PCALLBACK) lt_dlsym(mCallbackLib,
				pCallbackName->c_str());
		dlsymError = lt_dlerror();
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
	// reset errors
	lt_dlerror();
	if (lt_dlclose(mCallbackLib) != 0)
	{
		std::cerr << "Error closing library: " << CALLBACKS_LA << ": "
				<< lt_dlerror() << std::endl;
	}
	//callbacks unloaded
	mCallbacksLoaded = false;
}

void Component::setupEvents()
{
	mEventTable.clear();
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
			//any "events" string is a "compound" one, i.e. could have the form:
			// "eventType1@event1:eventType2@event2:...:eventTypeN@eventN"
			std::vector<std::string> typeEventPairs = parseCompoundString(*iter,
					':');
			std::vector<std::string>::const_iterator iterPair;
			for (iterPair = typeEventPairs.begin(); iterPair != typeEventPairs.end();
					++iterPair)
			{
				//an empty eventType@event pair is ignored
				if (not iterPair->empty())
				{
					//get event type and event
					std::vector<std::string> typeEventPair = parseCompoundString(
							*iterPair, '@');
					//check only if there is a pair
					if(typeEventPair.size() >= 2)
					{
						//ignore a not existent event type (== typeEventPair[0])
						//or a not existent event (== typeEventPair[1])
						if (mOwnerObject->objectTmpl()->isComponentParameter(
								"event_types", typeEventPair[0], componentType())
								and (not typeEventPair[1].empty()))
						{
							//insert event keyed by eventType;
							mEventTable[typeEventPair[0]] = typeEventPair[1];
							//insert eventType keyed by event;
							mEventTypeTable[typeEventPair[1]] = typeEventPair[0];
							//insert the NULL callback keyed by eventType;
							mCallbackTable[typeEventPair[0]] = NULL;
						}
					}
				}
			}
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
		//event = mEventTable[iter->first], iter->second==handler
		//
//		mTmpl->pandaFramework()->define_key(mEventTable[iter->first],
//				iter->first + "@" + mEventTable[iter->first], iter->second,
//				static_cast<void*>(this));
		//
		EventHandler::get_global_event_handler()->add_hook(
				mEventTable[iter->first], iter->second,
				static_cast<void*>(this));
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

std::string Component::getEventType(const std::string& event)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	std::string result("");
	if (mEventTypeTable.find(event) != mEventTypeTable.end())
	{
		result = mEventTypeTable[event];
	}
	return result;
}

ReMutex& Component::getMutex()
{
	return mMutex;
}

//TypedObject semantics: hardcoded
TypeHandle Component::_type_handle;

} // namespace ely
