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
#include "Game/GameManager.h"

namespace ely
{

Activity::Activity() :
		mFSM("FSM"), mTransitionLib(NULL), mTransitionsLoaded(false)
{
	// TODO Auto-generated constructor stub
}

Activity::Activity(SMARTPTR(ActivityTemplate)tmpl) :
		mFSM("FSM"), mTransitionLib(NULL), mTransitionsLoaded(false)
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
	///State transitions.
	mStateTransitionListParam = mTmpl->parameterList(
			std::string("states_transition"));
	///FromTo transitions.
	mFromToTransitionListParam = mTmpl->parameterList(
			std::string("from_to_transition"));
	//
	return result;
}

void Activity::doSetupHelperData()
{
	//clear helper data
	mStateTransitionTable.clear();
	mStatePairFromToTable.clear();
	//
	std::list<std::string>::const_iterator iter;
	//fill up transition functions' names table
	//with null string for transition function names
	std::list<std::string> states =
			mOwnerObject->objectTmpl()->componentParameterValues(
					std::string("states"), componentType());
	for (iter = states.begin(); iter != states.end(); ++iter)
	{
		//a valid state has a not empty value
		if (not iter->empty())
		{
			//set empty transitions' names for each state
			mStateTransitionTable[*iter] = TransitionNameTriple();
		}
	}
	//override transition functions on a per Object basis
	for (iter = mStateTransitionListParam.begin();
			iter != mStateTransitionListParam.end(); ++iter)
	{
		//any "states_transition" string is a "compound" one, i.e. could
		//have the form:
		// "state1:state2:...:stateN$enterName,exitName,filterName"
		//parse string as a (stateList,transitionNameTriple) pair
		std::vector<std::string> stateListFuncTriple = parseCompoundString(
				*iter, '$');
		//override only if there is (at least) a (stateList,FuncTriple) pair
		if (stateListFuncTriple.size() >= 2)
		{
			TransitionNameTriple transitionNameTriple;
			//parse second element as a func-name triple
			std::vector<std::string> funcNameList = parseCompoundString(
					stateListFuncTriple[1], ',');
			//set only if there is (at least) a triple
			if (funcNameList.size() >= 3)
			{
				//any one could be empty
				transitionNameTriple.mEnter = funcNameList[0];
				transitionNameTriple.mExit = funcNameList[1];
				transitionNameTriple.mFilter = funcNameList[2];
				//parse first element as a state list
				std::vector<std::string> states = parseCompoundString(
						stateListFuncTriple[0], ':');
				std::vector<std::string>::const_iterator iterState;
				for (iterState = states.begin(); iterState != states.end();
						++iterState)
				{
					//insert only if it is a valid state
					if (mStateTransitionTable.find(*iterState)
							!= mStateTransitionTable.end())
					{
						//set transitions' names for each state
						mStateTransitionTable[*iterState] =
								transitionNameTriple;
					}
				}
			}
		}
	}
	//fill up FromTo transition functions' names table
	//with null string for fromTo transition function names
	//only for valid fromTo transitions
	std::list<std::string> fromTos =
			mOwnerObject->objectTmpl()->componentParameterValues(
					std::string("from_to"), componentType());
	for (iter = fromTos.begin(); iter != fromTos.end(); ++iter)
	{
		//parse element as state pair
		std::vector<std::string> statePair = parseCompoundString(*iter, '@');
		//a valid fromTo transition must have valid states (== statePair[i])
		if (mOwnerObject->objectTmpl()->isComponentParameterValue("states",
				statePair[0], componentType()) and (not statePair[0].empty())
				and mOwnerObject->objectTmpl()->isComponentParameterValue(
						"states", statePair[1], componentType())
				and (not statePair[1].empty()))
		{
			//insert empty name into the FromTo functions' names table
			mStatePairFromToTable[StatePair(statePair[0], statePair[1])] =
					std::string("");
		}
	}
	//override FromTo transition functions on a per Object basis
	for (iter = mFromToTransitionListParam.begin();
			iter != mFromToTransitionListParam.end(); ++iter)
	{
		//any "from_to_transition" string is a "compound" one, i.e.
		//could have the form:
		// "state11@state21:state12@state22:...:state1N@state2N$fromToName"
		//parse string as a (statePairs,fromToName) pair
		std::vector<std::string> statePairsFromTo = parseCompoundString(*iter,
				'$');
		//override only if there is (at least) a (statePair,FromToName) pair
		if (statePairsFromTo.size() >= 2)
		{
			//set second element as FromToName (could be empty)
			std::string fromToName = statePairsFromTo[1];
			//parse first element as a state pair list
			std::vector<std::string> statePairs = parseCompoundString(
					statePairsFromTo[0], ':');
			std::vector<std::string>::const_iterator iterStatePair;
			for (iterStatePair = statePairs.begin();
					iterStatePair != statePairs.end(); ++iterStatePair)
			{
				//parse element as a state pair
				std::vector<std::string> statePair = parseCompoundString(
						*iterStatePair, '@');
				//insert only if it is a valid fromTo transition
				if (mStatePairFromToTable.find(
						StatePair(statePair[0], statePair[1]))
						!= mStatePairFromToTable.end())
				{
					//insert into the FromTo functions' names table
					mStatePairFromToTable[StatePair(statePair[0], statePair[1])] =
							fromToName;
				}
			}
		}
	}
}

void Activity::onAddToObjectSetup()
{
	//setup the FSM
	doSetupHelperData();

	//load transitions library
	doLoadTransitionFunctions();
}

void Activity::onRemoveFromObjectCleanup()
{
	//
	reset();
	//unload transitions library
	doUnloadTransitionFunctions();
}

void Activity::doLoadTransitionFunctions()
{
	//if no states or transitions loaded do nothing
	if ((mStateTransitionTable.size() == 0) or mTransitionsLoaded)
	{
		return;
	}
	// reset errors
	lt_dlerror();
	//load the transition functions library
	mTransitionLib = lt_dlopen(
			GameManager::GetSingletonPtr()->getDataInfo(
					GameManager::TRANSITIONS).c_str());
	if (mTransitionLib == NULL)
	{
		std::cerr << "Error loading library: "
				<< GameManager::GetSingletonPtr()->getDataInfo(
						GameManager::TRANSITIONS) << ": " << lt_dlerror()
				<< std::endl;
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
			functionName = iterTable->second.mEnter;
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
			functionName = iterTable->second.mExit;
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
			functionName = iterTable->second.mFilter;
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
		//eventually add state
		mFSM.addState(iterTable->first, enterFunc, exitFunc, filterFunc);
	}

	//load FromTo transition functions if any
	std::map<StatePair, std::string>::const_iterator iterTable1;
	for (iterTable1 = mStatePairFromToTable.begin();
			iterTable1 != mStatePairFromToTable.end(); ++iterTable1)
	{
		const char* dlsymError;
		std::string stateA, stateB, functionName, functionNameTmp;

		//reset errors
		lt_dlerror();
		//set states
		stateA = iterTable1->first.first;
		stateB = iterTable1->first.second;
		//set FromTo functionName
		if (not iterTable1->second.empty())
		{
			//set FromTo function name as specified (if any)
			functionName = iterTable1->second;
		}
		else
		{
			//set FromTo function name as "<STATEA>_FromTo_<STATEB>_<OBJECTYPE>" (if any)
			functionNameTmp = stateA + "_FromTo_" + stateB + "_"
					+ mOwnerObject->objectTmpl()->objectType();
			functionName = replaceCharacter(functionNameTmp, '-', '_');
		}
		PFROMTO pFromToFunction = (PFROMTO) lt_dlsym(mTransitionLib,
				functionName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT_ERR("Cannot load " << functionName << ": " << dlsymError);
			pFromToFunction = NULL;
		}
		//add the FromTo function
		mFSM.addFromToFunc(stateA, stateB,
				boost::bind(pFromToFunction, _1, boost::ref(*this), _2));
	}
	//clear no more needed helper data
	mStateTransitionTable.clear();
	mStatePairFromToTable.clear();

	//transitions loaded
	mTransitionsLoaded = true;
}

void Activity::doUnloadTransitionFunctions()
{
	//if transitions not loaded do nothing
	if (not mTransitionsLoaded)
	{
		return;
	}
	//Close the transition functions library
	// reset errors
	lt_dlerror();
	if (lt_dlclose(mTransitionLib) != 0)
	{
		std::cerr << "Error closing library: "
				<< GameManager::GetSingletonPtr()->getDataInfo(
						GameManager::TRANSITIONS) << ": " << lt_dlerror()
				<< std::endl;
	}
	//transitions unloaded
	mTransitionsLoaded = false;
}

//TypedObject semantics: hardcoded
TypeHandle Activity::_type_handle;

} // namespace ely
