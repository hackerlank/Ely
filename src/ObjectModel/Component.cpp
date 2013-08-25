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

Component::Component()
#ifdef ELY_THREAD
	:mDestroying(false)
#endif
{
	mTmpl.clear();
	mComponentId = ComponentId();
	mOwnerObject.clear();
	mCallbackLib = NULL;
	mCallbacksLoaded = false;
	mCallbacksRegistered = false;
	doCleanupEventTables();
}

Component::~Component()
{
}

void Component::addToObjectSetup()
{
	//call onAddToObjectSetup
	onAddToObjectSetup();
	//setup event tables (if any)
	doSetupEventTables();
	//load event callbacks (if any)
	doLoadEventCallbacks();
	//register event callbacks (if any)
	registerEventCallbacks();
}

void Component::removeFromObjectCleanup()
{
	//unregister event callbacks (if any)
	unregisterEventCallbacks();
	//unload event callbacks (if any)
	doUnloadEventCallbacks();
	//cleanup event tables (if any)
	doCleanupEventTables();
	//call onRemoveFromObjectCleanup
	onRemoveFromObjectCleanup();
}

SMARTPTR(Object)Component::getOwnerObject() const
{
	return mOwnerObject;
}

void Component::setOwnerObject(SMARTPTR(Object)ownerObject)
{
	mOwnerObject = ownerObject;
}

AsyncTask::DoneStatus Component::update(GenericAsyncTask* task)
{
	return AsyncTask::DS_done;
}

void Component::doSetupEventTables()
{
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
			// "eventType1@event1:eventType2@event2:...:eventTypeN@eventN$callback"
			//parse string as a ((type,event)s,callback) pair
			std::vector<std::string> typeEventsCallbackPair = parseCompoundString(
					*iter, '$');
			//check if there is a pair and set the callback name
			//as the second element (if any) that could be empty
			std::string callbackName = std::string("");
			if (typeEventsCallbackPair.size() >= 2)
			{
				callbackName = typeEventsCallbackPair[1];
			}
			//parse first element as (type,event) pair list
			std::vector<std::string> typeEventPairs = parseCompoundString(
					typeEventsCallbackPair[0], ':');
			std::vector<std::string>::const_iterator iterPair;
			for (iterPair = typeEventPairs.begin();
					iterPair != typeEventPairs.end(); ++iterPair)
			{
				//an empty eventType@event pair is ignored
				if (not iterPair->empty())
				{
					//get event type and event
					std::vector<std::string> typeEventPair =
							parseCompoundString(*iterPair, '@');
					//check only if there is a pair
					if (typeEventPair.size() >= 2)
					{
						//ignore a not existent event type (== typeEventPair[0])
						//or a not existent event (== typeEventPair[1])
						if (mOwnerObject->objectTmpl()->isComponentParameter(
								"event_types", typeEventPair[0],
								componentType())
								and (not typeEventPair[1].empty()))
						{
							//insert event keyed by eventType;
							mEventTable[typeEventPair[0]] = typeEventPair[1];
							//insert eventType keyed by event;
							mEventTypeTable[typeEventPair[1]] =
									typeEventPair[0];
							//insert the <callbackName,NULL> pair keyed by eventType;
							mCallbackTable[typeEventPair[0]] =
									NameCallbackPair(callbackName, NULL);
						}
					}
				}
			}
		}
	}
}

void Component::doCleanupEventTables()
{
	mEventTable.clear();
	mEventTypeTable.clear();
	mCallbackTable.clear();
}

void Component::doLoadEventCallbacks()
{
	//if callbacks already loaded do nothing
	RETURN_ON_COND(mCallbacksLoaded,)

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
	//load every callback, as specified in callbacks' name table
	std::map<std::string, NameCallbackPair>::iterator iterCallbackTable;
	for (iterCallbackTable = mCallbackTable.begin();
			iterCallbackTable != mCallbackTable.end(); ++iterCallbackTable)
	{
		std::string callbackName;
		if(not iterCallbackTable->second.first.empty())
		{
			//the callback name has been specified as parameter
			callbackName = iterCallbackTable->second.first;
		}
		else
		{
			//the callback name has not been specified as parameter:
			//try the name: <EVENTTYPE>_<COMPONENTTYPE>_<OBJECTTYPE>
			std::string callbackNameTmp = (iterCallbackTable->first) + "_"
					+ std::string(componentType()) + "_"
					+ std::string(mOwnerObject->objectTmpl()->objectType());
			//replace hyphens
			callbackName = replaceCharacter(callbackNameTmp, '-', '_');
		}
		//reset errors
		lt_dlerror();
		//load the callback
		PCALLBACK pCallback = (PCALLBACK) lt_dlsym(mCallbackLib,
				callbackName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT_ERR(
					"Cannot load callback " << callbackName << ": " << dlsymError);
			//set default callback for this event
			mCallbackTable[iterCallbackTable->first].second = pDefaultCallback;
			//continue with the next event
			continue;
		}
		//set callback for this event
		mCallbackTable[iterCallbackTable->first].second = pCallback;
	}

	//callbacks loaded
	mCallbacksLoaded = true;
}

void Component::doUnloadEventCallbacks()
{
	//if callbacks not loaded do nothing
	RETURN_ON_COND(not mCallbacksLoaded,)

	//clear callback tables
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

void Component::registerEventCallbacks()
{
	//if callbacks already registered do nothing
	RETURN_ON_COND(mCallbacksRegistered,)

	//register every handler
	std::map<std::string, NameCallbackPair>::iterator iterCallbackTable;
	for (iterCallbackTable = mCallbackTable.begin();
			iterCallbackTable != mCallbackTable.end(); ++iterCallbackTable)
	{
		//event = mEventTable[iter->first], iter->second.second==handler
		//
//		mTmpl->pandaFramework()->define_key(
//				mEventTable[iterCallbackTable->first],
//				iterCallbackTable->first + "@"
//						+ mEventTable[iterCallbackTable->first],
//				iterCallbackTable->second.second, static_cast<void*>(this));
		//
		EventHandler::get_global_event_handler()->add_hook(
				mEventTable[iterCallbackTable->first],
				iterCallbackTable->second.second, static_cast<void*>(this));
	}

	//handlers registered
	mCallbacksRegistered = true;
}

void Component::unregisterEventCallbacks()
{
	//if callbacks not already registered do nothing
	RETURN_ON_COND(not mCallbacksRegistered,)

	//Unregister the handlers
//	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
//			(void*) this);
	EventHandler::get_global_event_handler()->remove_hooks_with(
			(void*) this);

	//handlers unregistered
	mCallbacksRegistered = false;
}

std::string Component::getEventType(const std::string& event)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	std::string result("");
	if (mEventTypeTable.find(event) != mEventTypeTable.end())
	{
		result = mEventTypeTable[event];
	}
	return result;
}

//TypedObject semantics: hardcoded
TypeHandle Component::_type_handle;

} // namespace ely
