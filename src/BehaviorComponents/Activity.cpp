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
 * \author consultit
 */

#include "BehaviorComponents/Activity.h"
#include "BehaviorComponents/ActivityTemplate.h"
#include "ObjectModel/Object.h"

namespace ely
{

Activity::Activity() :
		mFSM("FSM")
{
	// TODO Auto-generated constructor stub
}

Activity::Activity(SMARTPTR(ActivityTemplate)tmpl) :
		mFSM("FSM")
{
	mTmpl = tmpl;
	reset();
}

Activity::~Activity()
{
}

ComponentFamilyType Activity::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Activity::componentType() const
{
	return mTmpl->componentType();
}

bool Activity::initialize()
{
	bool result = true;
	//
	return result;
}

void Activity::doSetupFSM()
{
	std::list<std::string>::iterator iter;
	//setup states
	std::list<std::string> stateList =
			mOwnerObject->objectTmpl()->componentParameterList(
					std::string("states"), componentType());
	for (iter = stateList.begin(); iter != stateList.end(); ++iter)
	{
		//any "states" string is a "compound" one, i.e. could have the form:
		// "state1:state2:...:stateN$enterName,exitName,filterName"
		//parse string as a (states,transitionNameTriple) pair
		std::vector<std::string> statesFuncNameTriple = parseCompoundString(
				*iter, '$');
		TransitionNameTriple transitionNameTriple;
		//check if there is a pair and set the func-name triple
		if (statesFuncNameTriple.size() >= 2)
		{
			//parse second element as a func-name triple
			std::vector<std::string> funcNameList = parseCompoundString(
					statesFuncNameTriple[1], ',');
			//set only if there is (at least) a triple
			if (funcNameList.size() >= 3)
			{
				transitionNameTriple.mEnter = funcNameList[0];
				transitionNameTriple.mExit = funcNameList[1];
				transitionNameTriple.mFilter = funcNameList[2];
			}
		}
		//parse first element as a state list
		std::vector<std::string> states = parseCompoundString(
				statesFuncNameTriple[0], ':');
		std::vector<std::string>::const_iterator iterState;
		for (iterState = states.begin(); iterState != states.end(); ++iterState)
		{
			//an empty state is ignored
			if (not iterState->empty())
			{
				//set transitions' names for each state
				mStateTransitionTable[*iterState] = transitionNameTriple;
			}
		}
	}
	//setup FromTo transition function set
	std::list<std::string> fromToList =
			mOwnerObject->objectTmpl()->componentParameterList(
					std::string("from_to"), componentType());
	for (iter = fromToList.begin(); iter != fromToList.end(); ++iter)
	{
		//any "from_to" string is a "compound" one, i.e. could have the form:
		// "from_to1:from_to2:...:from_toN"
		std::vector<std::string> fromTos = parseCompoundString(*iter, ':');
		std::vector<std::string>::const_iterator iterFromTo;
		for (iterFromTo = fromTos.begin(); iterFromTo != fromTos.end();
				++iterFromTo)
		{
			//an empty from_to is ignored
			if (not iterFromTo->empty())
			{
				mFromToFunctionSet.insert(*iterFromTo);
			}
		}
	}
}

void Activity::onAddToObjectSetup()
{
	//add even for an empty object node path

	//setup the FSM
	doSetupFSM();

	//load transitions library
	doLoadTransitionFunctions();
}

void Activity::onRemoveFromObjectCleanup()
{
	//unload transitions library
	doUnloadTransitionFunctions();
	//
	reset();
}

void Activity::doLoadTransitionFunctions()
{
	//if no states or transitions loaded do nothing
	if ((mFSM.getNumStates() == 0) or mTransitionsLoaded)
	{
		return;
	}
	mTransitionLib = NULL;
	// reset errors
	lt_dlerror();
	//load the transition functions library
	mTransitionLib = lt_dlopen(TRANSITIONS_LA);
	if (mTransitionLib == NULL)
	{
		std::cerr << "Error loading library: " << TRANSITIONS_LA << ": "
				<< lt_dlerror() << std::endl;
		return;
	}

	//for each state load transition functions' names
	std::map<std::string, TransitionNameTriple>::const_iterator iterTable;
	for (iterTable = mStateTransitionTable.begin();
			iterTable != mStateTransitionTable.end(); ++iterTable)
	{
		const char* dlsymError;
		std::string functionName, functionNameTmp;

		//reset errors
		lt_dlerror();
		//set enter functionName
		if (not iterTable->second.mEnter.empty())
		{
			//set enter function name as specified (if any)
			functionName = iterTable->second.mEnter.empty();
		}
		else
		{
			//set enter function name as "Enter_<STATE>_<OBJECTYPE>" (if any)
			functionNameTmp = std::string("Enter") + "_" + iterTable->first
					+ "_" + mOwnerObject->objectTmpl()->objectType();
			functionName = replaceCharacter(functionNameTmp, '-', '_');

		}
		PENTER pEnterFunction = (PENTER) lt_dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT_ERR("Cannot load " << functionName << ": " << dlsymError);
			pEnterFunction = NULL;
		}

		//reset errors
		lt_dlerror();
		//set exit functionName
		if (not iterTable->second.mExit.empty())
		{
			//set exit function name as specified (if any)
			functionName = iterTable->second.mExit.empty();
		}
		else
		{
			//set exit function name as "Exit_<STATE>_<OBJECTYPE>" (if any)
			functionNameTmp = std::string("Exit") + "_" + iterTable->first + "_"
					+ mOwnerObject->objectTmpl()->objectType();
			functionName = replaceCharacter(functionNameTmp, '-', '_');
		}
		PEXIT pExitFunction = (PEXIT) lt_dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT_ERR("Cannot load " << functionName << ": " << dlsymError);
			pExitFunction = NULL;
		}

		//reset errors
		lt_dlerror();
		//set filter functionName
		if (not iterTable->second.mFilter.empty())
		{
			//set filter function name as specified (if any)
			functionName = iterTable->second.mFilter.empty();
		}
		else
		{
			//set filter function name as "Filter_<STATE>_<OBJECTYPE>" (if any)
			functionNameTmp = std::string("Filter") + "_" + iterTable->first
					+ "_" + mOwnerObject->objectTmpl()->objectType();
			functionName = replaceCharacter(functionNameTmp, '-', '_');
		}
		PFILTER pFilterFunction = (PFILTER) lt_dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT_ERR("Cannot load " << functionName << ": " << dlsymError);
			pFilterFunction = NULL;
		}
		//add the state with the current transition functions
		fsm::EnterFuncPTR enterFunc = (
				pEnterFunction != NULL ?
						boost::bind(pEnterFunction, _1, boost::ref(*this), _2) :
						NULL);
		fsm::ExitFuncPTR exitFunc = (
				pExitFunction != NULL ?
						boost::bind(pExitFunction, _1, boost::ref(*this)) :
						NULL);
		fsm::FilterFuncPTR filterFunc = (
				pFilterFunction != NULL ?
						boost::bind(pFilterFunction, _1, boost::ref(*this), _2,
								_3) :
						NULL);
		//eventually add state
		mFSM.addState(iterTable->first, enterFunc, exitFunc, filterFunc);
	}

	//load FromTo transition functions if any
	std::set<std::string>::const_iterator iter;
	for (iter = mFromToFunctionSet.begin(); iter != mFromToFunctionSet.end();
			++iter)
	{
		const char* dlsymError;
		std::string functionName, functionNameTmp;

		//load FromTo function: <STATEA>_FromTo_<STATEB>_<OBJECTYPE>
		// reset errors
		lt_dlerror();
		functionNameTmp = (*iter) + "_"
				+ mOwnerObject->objectTmpl()->objectType();
		functionName = replaceCharacter(functionNameTmp, '-', '_');
		PFROMTO pFromToFunction = (PFROMTO) lt_dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT_ERR("Cannot load " << functionName << ": " << dlsymError);
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

void Activity::doUnloadTransitionFunctions()
{
	//if transitions not loaded do nothing
	if ((mFSM.getNumStates() == 0) or (not mTransitionsLoaded))
	{
		return;
	}
	mFromToFunctionSet.clear();
	//Close the transition functions library
	// reset errors
	lt_dlerror();
	if (lt_dlclose(mTransitionLib) != 0)
	{
		std::cerr << "Error closing library: " << TRANSITIONS_LA << ": "
				<< lt_dlerror() << std::endl;
	}
	//transitions unloaded
	mTransitionsLoaded = false;
}

//TypedObject semantics: hardcoded
TypeHandle Activity::_type_handle;

} // namespace ely
