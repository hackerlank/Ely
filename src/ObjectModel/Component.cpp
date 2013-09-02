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
#include "Game/GameManager.h"

namespace ely
{

Component::Component() :
		mCallbackLib(NULL), mCallbacksLoaded(false), mCallbacksRegistered(false)
#ifdef ELY_THREAD
				, mDestroying(false)
#endif
{
	mTmpl.clear();
	mComponentId = ComponentId();
	mOwnerObject.clear();
	doCleanupEventTables();
}

Component::~Component()
{
}

void Component::addToObjectSetup()
{
	//setup event tables (if any)
	doSetupEventTables();
	//load event callbacks (if any)
	doLoadEventCallbacks();
	//call onAddToObjectSetup
	onAddToObjectSetup();
	//register event callbacks (if any)
	registerEventCallbacks();
}

void Component::removeFromObjectCleanup()
{
	//unregister event callbacks (if any)
	unregisterEventCallbacks();
	//call onRemoveFromObjectCleanup
	onRemoveFromObjectCleanup();
	//unload event callbacks (if any)
	doUnloadEventCallbacks();
	//cleanup event tables (if any)
	doCleanupEventTables();
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
	std::list<std::string>::const_iterator iter;
	//fill up event tables with event values == event type,
	//callbackName == empty string and callback function == NULL
	std::list<std::string> eventTypes =
			mOwnerObject->objectTmpl()->componentParameterValues(
					std::string("event_types"), componentType());
	for (iter = eventTypes.begin(); iter != eventTypes.end(); ++iter)
	{
		//a valid event type must be not empty
		if (not iter->empty())
		{
			//insert event keyed by eventType;
			mEventTable[*iter] = *iter;
			//insert eventType keyed by event;
			mEventTypeTable[*iter] = *iter;
			//insert the <callbackName,NULL> pair keyed by eventType;
			mCallbackTable[*iter] = NameCallbackPair(std::string(""), NULL);
		}
	}
	//override event values and callback functions on a per Object basis
	std::list<std::string> eventList = mTmpl->parameterList(
			std::string("events"));
	for (iter = eventList.begin(); iter != eventList.end(); ++iter)
	{
		//any "events" string is a "compound" one, i.e. could have the form:
		//"evType1@evValue1:evType2@evValue2:...:evTypeN@evValueN$callbackName"
		//parse string as a ((evType,evValue)s,callback) pair
		std::vector<std::string> typeValuesCallback = parseCompoundString(*iter,
				'$');
		//check if there is (at least) a pair
		if (typeValuesCallback.size() >= 2)
		{
			//set the callback name as the second element (could be empty)
			std::string callbackName = typeValuesCallback[1];
			//parse first element as (evType,evValue) pair list
			std::vector<std::string> typeValues = parseCompoundString(
					typeValuesCallback[0], ':');
			std::vector<std::string>::const_iterator iterTypeValue;
			for (iterTypeValue = typeValues.begin();
					iterTypeValue != typeValues.end(); ++iterTypeValue)
			{
				//get event type and event value
				std::vector<std::string> typeValue = parseCompoundString(
						*iterTypeValue, '@');
				//check if there is (at least) a pair
				if (typeValue.size() >= 2)
				{
					//insert only if it is a valid event type (== typeValue[0])
					if (mEventTable.find(typeValue[0]) != mEventTable.end())
					{
						//change only non empty event value (== typeValue[1])
						if (not typeValue[1].empty())
						{
							//insert event keyed by eventType;
							mEventTable[typeValue[0]] = typeValue[1];
							//insert eventType keyed by event;
							mEventTypeTable[typeValue[1]] = typeValue[0];
						}
						//insert the <callbackName,NULL> pair keyed by eventType;
						mCallbackTable[typeValue[0]] = NameCallbackPair(
								callbackName, NULL);
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
	mCallbackLib =
			lt_dlopen(
					GameManager::GetSingletonPtr()->getDataInfo(
							GameManager::CALLBACKS).c_str());
	if (mCallbackLib == NULL)
	{
		std::cerr << "Error loading library: "
				<< GameManager::GetSingletonPtr()->getDataInfo(
						GameManager::CALLBACKS) << ": " << lt_dlerror()
				<< std::endl;
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
			std::cerr << "Error closing library: "
					<< GameManager::GetSingletonPtr()->getDataInfo(
							GameManager::CALLBACKS) << std::endl;
		}
		return;
	}
	//load every callback, as specified in callbacks' name table
	std::map<std::string, NameCallbackPair>::const_iterator iterCallbackTable;
	for (iterCallbackTable = mCallbackTable.begin();
			iterCallbackTable != mCallbackTable.end(); ++iterCallbackTable)
	{
		std::string callbackName;
		if (not iterCallbackTable->second.first.empty())
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
			PRINT_ERR_DEBUG(
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

	//Close the event callbacks library
	// reset errors
	lt_dlerror();
	if (lt_dlclose(mCallbackLib) != 0)
	{
		std::cerr << "Error closing library: "
				<< GameManager::GetSingletonPtr()->getDataInfo(
						GameManager::CALLBACKS) << ": " << lt_dlerror()
				<< std::endl;
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
	EventHandler::get_global_event_handler()->remove_hooks_with((void*) this);

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
