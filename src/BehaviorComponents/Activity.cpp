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

Activity::Activity() :
		mFSM("FSM")
{
	// TODO Auto-generated constructor stub
}

Activity::Activity(SMARTPTR(ActivityTemplate)tmpl) :
mFSM("FSM"), mTransitionsLoaded(false)
{
	mTmpl = tmpl;
}

Activity::~Activity()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	unloadTransitionFunctions();
}

const ComponentFamilyType Activity::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Activity::componentType() const
{
	return mTmpl.p()->componentType();
}

bool Activity::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	std::list<std::string>::iterator iter;
	//setup states
	std::list<std::string> stateList = mTmpl->parameterList(
			std::string("states"));
	for (iter = stateList.begin(); iter != stateList.end(); ++iter)
	{
		//set default transitions for each state
		mFSM.addState(*iter, NULL, NULL, NULL);
	}
	//setup FromTo transition function set
	std::list<std::string> fromToList = mTmpl->parameterList(
			std::string("from_to"));
	for (iter = fromToList.begin(); iter != fromToList.end(); ++iter)
	{
		mFromToFunctionSet.insert(*iter);
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
	std::string objectId = std::string(mOwnerObject->objectId());
	for (iter = stateSet.begin(); iter != stateSet.end(); ++iter)
	{
		const char* dlsymError;
		std::string functionName;

		//load enter function (if any): Enter_<STATE>_<OBJECTID>
		// reset errors
		dlerror();
		functionName = std::string("Enter") + "_" + (*iter) + "_" + objectId;
		PENTER pEnterFunction = (PENTER) dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load " << functionName << ": " << dlsymError
					<< std::endl;
			pEnterFunction = NULL;
		}

		//load exit function (if any): Exit_<STATE>_<OBJECTID>
		// reset errors
		dlerror();
		functionName = std::string("Exit") + "_" + (*iter) + "_" + objectId;
		PEXIT pExitFunction = (PEXIT) dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load " << functionName << ": " << dlsymError
					<< std::endl;
			pExitFunction = NULL;
		}

		//load filter function (if any): Filter_<STATE>_<OBJECTID>
		// reset errors
		dlerror();
		functionName = std::string("Filter") + "_" + (*iter) + "_" + objectId;
		PFILTER pFilterFunction = (PFILTER) dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load " << functionName << ": " << dlsymError
					<< std::endl;
			pFilterFunction = NULL;
		}
		//re-add the state with the current functions
		fsm::EnterFuncPTR enterFunc = NULL;
		if (pEnterFunction != NULL)
		{
			enterFunc = boost::bind(pEnterFunction, _1, boost::ref(*this), _2);
		}
		fsm::ExitFuncPTR exitFunc = NULL;
		if (pExitFunction != NULL)
		{
			exitFunc = boost::bind(pExitFunction, _1, boost::ref(*this));
		}
		fsm::FilterFuncPTR filterFunc = NULL;
		if (pFilterFunction != NULL)
		{
			filterFunc = boost::bind(pFilterFunction, _1, boost::ref(*this), _2,
					_3);
		}
		//
		mFSM.addState((*iter), enterFunc, exitFunc, filterFunc);
	}

	//load FromTo transition functions if any
	for (iter = mFromToFunctionSet.begin(); iter != mFromToFunctionSet.end();
			++iter)
	{
		const char* dlsymError;
		std::string functionName;

		//load FromTo function: <STATEA>_FromTo_<STATEB>_<OBJECTID>
		// reset errors
		dlerror();
		functionName = (*iter) + "_" + objectId;
		PFROMTO pFromToFunction = (PFROMTO) dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = dlerror();
		if (dlsymError)
		{
			std::cerr << "Cannot load " << functionName << ": " << dlsymError
					<< std::endl;
			pFromToFunction = NULL;
		}
		//get the position of the "_FromTo_" substring in (*iter)
		size_t fromToPos = (*iter).find("_FromTo_");
		if (fromToPos == string::npos)
		{
			//the function name doesn't contain "_FromTo_":
			//continue with the next function
			continue;
		}
		//get 2 state names
		std::string stateFrom = (*iter).substr(0, fromToPos);
		std::string stateTo = (*iter).substr(
				fromToPos + std::string("_FromTo_").length());
		//add the FromTo function
		mFSM.addFromToFunc(stateFrom, stateTo,
				boost::bind(pFromToFunction, _1, boost::ref(*this), _2));
	}

	//transitions loaded
	mTransitionsLoaded = true;
}

Activity::operator fsm&()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mFSM;
}

void Activity::unloadTransitionFunctions()
{
	//if transitions not loaded do nothing
	if ((mFSM.getNumStates() == 0) or (not mTransitionsLoaded))
	{
		return;
	}
	mFromToFunctionSet.clear();
	//Close the transition functions library
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

