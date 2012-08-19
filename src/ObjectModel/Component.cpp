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
		mOwnerObject(NULL), mHandlerLibLoaded(false)
{
}

Component::~Component()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//unregister events (if any)
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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

}

void Component::unloadEventHandlers()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

}
#else
void Component::loadEventHandlers()
{
	mHandlerLib = NULL;
	//register only if some events is present and
	//this component has been already added to an object
	if ((not mEventSet.empty()) and mOwnerObject)
	{
		//load the event handlers library
		if (not mHandlerLibLoaded)
		{
			mHandlerLib = dlopen(HANDLERS_SO, RTLD_LAZY);
			if (not mHandlerLib)
			{
				std::cerr << "Error loading library: " << dlerror()
						<< std::endl;
				return;
			}
			//library loaded
			mHandlerLibLoaded = true;
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
			return;
		}
		//register every event
		std::set<std::string>::iterator iter;
		for (iter = mEventSet.begin(); iter != mEventSet.end(); ++iter)
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
				//register default handler for this event
				mTmpl->pandaFramework()->define_key((*iter), (*iter),
						pDefaultHandler, (void*) this);
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
				//register default handler for this event
				mTmpl->pandaFramework()->define_key((*iter), (*iter),
						pDefaultHandler, (void*) this);
				//continue with the next event
				continue;
			}
			//register handler for this event
			mTmpl->pandaFramework()->define_key((*iter), (*iter), pHandler,
					(void*) this);
		}
	}
}

void Component::unloadEventHandlers()
{
	if ((not mEventSet.empty()) and mOwnerObject and mHandlerLibLoaded)
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
		mHandlerLibLoaded = false;
	}
}
#endif

void Component::setupEvents()
{
	//setup events (if any)
	std::list<std::string>::iterator iter;
	std::list<std::string> eventList = mTmpl->parameterList(
			std::string("events"));
	if (not eventList.empty())
	{
		for (iter = eventList.begin(); iter != eventList.end(); ++iter)
		{
			mEventSet.insert(*iter);
		}
	}
}

ReMutex& Component::getMutex()
{
	return mMutex;
}

//TypedObject semantics: hardcoded
TypeHandle Component::_type_handle;

