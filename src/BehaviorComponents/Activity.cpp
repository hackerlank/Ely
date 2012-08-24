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
 * \file /Ely/src/BehaviorComponents/Activity.cpp
 *
 * \date 17/ago/2012 (09:28:59)
 * \author marco
 */

#include "BehaviorComponents/Activity.h"
#include "BehaviorComponents/ActivityTemplate.h"

Activity::Activity()
{
	// TODO Auto-generated constructor stub
}

Activity::Activity(ActivityTemplate* tmpl) :
		mFSM("FSM"), mTransitionsLoaded(false)
{
	mTmpl = tmpl;
}

Activity::~Activity()
{
	unloadTransitionFunctions();
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
	//setup states
	std::list<std::string>::iterator iter;
	std::list<std::string> stateList = mTmpl->parameterList(
			std::string("states"));
	//set default transitions for each state
	for (iter = stateList.begin(); iter != stateList.end(); ++iter)
	{
		mFSM.addState(*iter, NULL, NULL, NULL);
	}
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void Activity::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//load transitions library
	loadTransitionFunctions();

	//register event callbacks if any
	registerEventCallbacks();
}

#ifdef WIN32
void Activity::loadTransitionFunctions()
{
}

void Activity::unloadTransitionFunctions()
{
}
#else
void Activity::loadTransitionFunctions()
{
	//if no states or transitions loaded do nothing
	if ((mFSM.getNumStates() == 0) or mTransitionsLoaded)
	{
		return;
	}
	mTransitionLib = NULL;
	//load the transition functions library
	mTransitionLib = dlopen(TRANSITIONS_SO, RTLD_LAZY);
	if (not mTransitionLib)
	{
		std::cerr << "Error loading library: " << dlerror() << std::endl;
		return;
	}

	//for each state load transition functions if any
	std::set<std::string> stateSet = mFSM.getKeyStateSet();
	std::set<std::string>::iterator iter;
	for (iter = stateSet.begin(); iter != stateSet.end(); ++iter)
	{
		const char* dlsymError;
		std::string functionName;

		// reset errors
		dlerror();
		//load enter function (if any): EnterState
		functionName = std::string("Enter") + (*iter);
		PENTER pEnterFunction = (PENTER) dlsym(mTransitionLib,
				functionName.c_str());
		const char* dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load " << functionName << ": " << dlsymError
					<< std::endl;
			pEnterFunction = NULL;
		}

		// reset errors
		dlerror();
		//load exit function (if any): ExitState
		functionName = std::string("Exit") + (*iter);
		PEXIT pExitFunction = (PEXIT) dlsym(mTransitionLib,
				functionName.c_str());
		const char* dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load " << functionName << ": " << dlsymError
					<< std::endl;
			pExitFunction = NULL;
		}
		//////////////////////////////////////////
		////////TO BE CONTINUED.........
	}
	//transitions loaded
	mTransitionsLoaded = true;

	//////////////////NO GOOD
	//load every callback
	std::map<std::string, PCALLBACK>::iterator iter;
	for (iter = mTransitionTable.begin(); iter != mTransitionTable.end();
			++iter)
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
		PCALLBACKNAME pTransitionName = (PCALLBACKNAME) dlsym(mTransitionLib,
				variableName.c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load variable " << variableName << ": "
					<< dlsymError << std::endl;
			//set default callback for this event
			mTransitionTable[iter->first] = pDefaultTransition;
			//continue with the next event
			continue;
		}
		//reset errors
		dlerror();
		//load the callback
		PCALLBACK pTransition = (PCALLBACK) dlsym(mTransitionLib,
				pTransitionName->c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load callback " << pTransitionName << ": "
					<< dlsymError << std::endl;
			//set default callback for this event
			mTransitionTable[iter->first] = pDefaultTransition;
			//continue with the next event
			continue;
		}
		//set callback for this event
		mTransitionTable[iter->first] = pTransition;
	}
}

void Activity::unloadTransitionFunctions()
{
	//if transitions not loaded do nothing
	if (not mTransitionsLoaded)
	{
		return;
	}
	mTransitionTable.clear();
	//Close the event transitions library
	if (dlclose(mTransitionLib) != 0)
	{
		std::cerr << "Error closing library: " << CALLBACKS_SO << std::endl;
	}
	//transitions unloaded
	mTransitionsLoaded = false;
}
#endif

//TypedObject semantics: hardcoded
TypeHandle Activity::_type_handle;

