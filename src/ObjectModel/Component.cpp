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
		mOwnerObject(NULL), mHandlersRegistered(false), mHandlersLoaded(false)
{
}

Component::~Component()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//unload event handlers
	unloadEventHandlers();
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
void Component::loadEventHandlers()
{
}

void Component::unloadEventHandlers()
{
}
#else
void Component::loadEventHandlers()
{
	if (mHandlersLoaded)
	{
		return;
	}
	mHandlerLib = NULL;
	//some events should be present: check anyway
	if (not mHandlerTable.empty())
	{
		//load the event handlers library
		mHandlerLib = dlopen(HANDLERS_SO, RTLD_LAZY);
		if (not mHandlerLib)
		{
			std::cerr << "Error loading library: " << dlerror() << std::endl;
			return;
		}
		// reset errors
		dlerror();
		//check the default handler
		PHANDLER pDefaultHandler = (PHANDLER) dlsym(mHandlerLib,
				DEFAULT_HANDLER);
		const char* dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot find default handler " << DEFAULT_HANDLER
			<< dlsymError << std::endl;
			//Close the event handlers library
			if (dlclose(mHandlerLib) != 0)
			{
				std::cerr << "Error closing library: " << HANDLERS_SO << std::endl;
			}
			return;
		}
		//load every handler
		std::map<std::string, PHANDLER>::iterator iter;
		for (iter = mHandlerTable.begin(); iter != mHandlerTable.end(); ++iter)
		{
			//reset errors
			dlerror();
			//load the variable whose value is the name
			//of the handler: <EVENT>_<FAMILYTYPE>_<OBJECTID>
			std::string variableName = (*iter) + "_" + std::string(familyType())
					+ "_" + std::string(mOwnerObject->objectId());
			PHANDLERNAME pHandlerName = (PHANDLERNAME) dlsym(mHandlerLib,
					variableName.c_str());
			dlsymError = dlerror();
			if (dlsymError)
			{
				std::cerr << "Cannot load variable " << variableName
						<< dlsymError << std::endl;
				//set default handler for this event
				mHandlerTable[iter->first] = pDefaultHandler;
				//continue with the next event
				continue;
			}
			//reset errors
			dlerror();
			//load the handler
			PHANDLER pHandler = (PHANDLER) dlsym(mHandlerLib,
					pHandlerName->c_str());
			dlsymError = dlerror();
			if (dlsymError)
			{
				std::cerr << "Cannot load handler " << pHandlerName
						<< dlsymError << std::endl;
				//set default handler for this event
				mHandlerTable[iter->first] = pDefaultHandler;
				//continue with the next event
				continue;
			}
			//set handler for this event
			mHandlerTable[iter->first] = pHandler;
		}
	}
	//handlers loaded
	mHandlersLoaded = true;
}

void Component::unloadEventHandlers()
{
	if (not mHandlersLoaded)
	{
		return;
	}
	//handlers should be unregistered: check anyway
	if (mHandlersRegistered)
	{
		unregisterEventHandlers();
	}
	mHandlerTable.clear();
	//Close the event handlers library
	if (dlclose(mHandlerLib) != 0)
	{
		std::cerr << "Error closing library: " << HANDLERS_SO << std::endl;
	}
	//handlers unloaded
	mHandlersLoaded = false;
}
#endif

void Component::setupEvents()
{
	mHandlerTable.clear();
	//setup events (if any)
	std::list<std::string>::iterator iter;
	std::list<std::string> eventList = mTmpl->parameterList(
			std::string("events"));
	if (not eventList.empty())
	{
		//populate the handler table with NULL for each event
		for (iter = eventList.begin(); iter != eventList.end(); ++iter)
		{
			std::pair<std::string, PHANDLER> tableItem(*iter, NULL);
			mHandlerTable.insert(tableItem);
		}
		//load event handlers
		loadEventHandlers();
	}
}

void Component::registerEventHandlers()
{
	if ((not mHandlersLoaded) or mHandlersRegistered)
	{
		return;
	}
	//register every handler
	std::map<std::string, PHANDLER>::iterator iter;
	for (iter = mHandlerTable.begin(); iter != mHandlerTable.end(); ++iter)
	{
		//first==event, second==handler
		mTmpl->pandaFramework()->define_key(iter->first, iter->first,
				iter->second, (void*) this);
	}
	//handlers registered
	mHandlersRegistered = true;
}

void Component::unregisterEventHandlers()
{




	if ((not mEventSet.empty()) and mOwnerObject and mHandlersRegistered)
	{
		//Unregister the handlers
		mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
				(void*) this);
		//Close the event handlers library
		if (dlclose(mHandlerLib) != 0)
		{
			std::cerr << "Error closing library: " << HANDLERS_SO << std::endl;
		}
		//library unloaded
		mHandlersRegistered = false;
	}
}

ReMutex& Component::getMutex()
{
	return mMutex;
}

//TypedObject semantics: hardcoded
TypeHandle Component::_type_handle;

