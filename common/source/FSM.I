/**
 * \file FSM.I
 *
 * \date 2016-10-09
 * \author consultit
 */
#ifndef FSM_I_
#define FSM_I_

#include "Utilities/Tools.h"
#include <queue>
#include <list>
#include <throw_event.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>

template<typename StateKey> int FSM<StateKey>::SerialNum = 0;

template<typename StateKey> FSM<StateKey>::FSM(const StateKey& name) :
		Null(StateKey()), InTransition(StateKey()), Off(StateKey())
{
	//any specialization must call this, otherwise no
	//initialization would take place
	initialize(name);
}

template<typename StateKey> FSM<StateKey>::~FSM()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)
}

template<typename StateKey> void FSM<StateKey>::initialize(const StateKey& name)
{
	//set FSM name
	mName = name;
	//set serial number
	mSerialNum = FSM<StateKey>::get_serial_num();
	//reset all data
	mStateSet.clear();
	mFromToFunctions.clear();
	//set the various flags
	mBroadcastStateChanges = false;
	mFiltering = false;
	//Set up initial status
	mStateKey = Off;
	mOldStateKey = Null;
	mNewStateKey = Null;
	//set up bindings of the default functions
	default_enterPTR = &FSM<StateKey>::default_enter;
	default_exitPTR = &FSM<StateKey>::default_exit;
	default_filterPTR = &FSM<StateKey>::default_filter;
	filter_offPTR = &FSM<StateKey>::filter_off;
}

template<typename StateKey> int FSM<StateKey>::get_serial_num()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return ++FSM<StateKey>::SerialNum;
}

template<typename StateKey> void FSM<StateKey>::cleanup()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey != Off)
	{
		set_state(Off);
	}
	//cleanup the FSM: reset all data
	mStateSet.clear();
	mFromToFunctions.clear();
	//set broadcast state change
	mBroadcastStateChanges = false;
}

template<typename StateKey> void FSM<StateKey>::set_broadcast_state_changes(
		bool doBroadcast)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mBroadcastStateChanges = doBroadcast;
}

template<typename StateKey> std::string FSM<StateKey>::get_state_change_event() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::ostringstream stateChange;
	stateChange << "FSM-" << mSerialNum << "-" << mName << "-stateChange";
	return stateChange.str();
}

template<typename StateKey> StateKey FSM<StateKey>::get_current_or_next_state() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey != InTransition)
	{
		return mStateKey;
	}
	return mNewStateKey;
}

template<typename StateKey> bool FSM<StateKey>::get_current_state_or_transition(
		StateKey& currStateKey, StateKey& toStateKey) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey != InTransition)
	{
		currStateKey = mStateKey;
		return true;
	}
	currStateKey = mOldStateKey;
	toStateKey = mNewStateKey;
	return false;
}

template<typename StateKey> bool FSM<StateKey>::is_in_transition() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStateKey == InTransition;
}

template<typename StateKey> void FSM<StateKey>::force_transition(
		const StateKey& stateKey, const ValueList& data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		// Queue up the request for later calling.
		FSMMethodPTR force_transitionPTR = boost::bind(
				&FSM<StateKey>::force_transition, boost::ref(*this), stateKey,
				data);
		mRequestQueue.push(force_transitionPTR);
		return;
	}
	set_state(stateKey, data);
}

template<typename StateKey> void FSM<StateKey>::demand(const StateKey& stateKey,
		const ValueList& data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		// Queue up the request for later calling.
		FSMMethodPTR demandPTR = boost::bind(&FSM<StateKey>::demand,
				boost::ref(*this), stateKey, data);
		mRequestQueue.push(demandPTR);
		return;
	}
	//
	if (request(stateKey, data) == Null)
	{
		std::cerr << "FSM::demand: Request denied from'" << mStateKey
				<< "' to '" << stateKey << "'" << std::endl;
	}
	return;
}

template<typename StateKey> StateKey FSM<StateKey>::request(
		const StateKey& newStateKey, const ValueList& data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	StateKey acceptedStateKey = Null;
	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::request: Cannot be called in enter/exit/fromTo functions: (state: '"
				<< mOldStateKey << "')" << std::endl;
	}
	else if (mFiltering)
	{
		std::cerr
				<< "FSM::request: Cannot be called in filter functions: (state: '"
				<< mStateKey << "')" << std::endl;
	}
	else
	{
		//filtering on
		mFiltering = true;
		ValueList result = get_current_filter()(this, newStateKey, data);
		//filtering off
		mFiltering = false;
		try
		{
			//check first if value is of State key type
			acceptedStateKey = boost::any_cast<StateKey>(result.front());
			//remove first value
			result.pop_front();
			//go to (potentially) new key state (could be Null)
			acceptedStateKey = set_state(acceptedStateKey, result);
			//
		} catch (const boost::bad_any_cast&)
		{
			std::cerr << "FSM::request: Filter for '" << mStateKey
					<< "' returned a bad state key type" << std::endl;
			acceptedStateKey = Null;
		}
	}
	//
	return acceptedStateKey;
}

template<typename StateKey> void FSM<StateKey>::set_state_set(
		const StateSet& stateSet)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mStateSet = stateSet;
}

template<typename StateKey> std::set<StateKey> FSM<StateKey>::get_key_state_set() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::set<StateKey> keyStateSet;
	typename StateSet::iterator iter;
	for (iter = mStateSet.begin(); iter != mStateSet.end(); ++iter)
	{
		keyStateSet.insert(iter->keyName);
	}
	return keyStateSet;
}

template<typename StateKey> unsigned int FSM<StateKey>::get_num_states() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStateSet.size();
}

template<typename StateKey> StateKey FSM<StateKey>::request_next(
		const ValueList& data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	StateKey newState = Null;
	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::request_next: Cannot be called in enter/exit/fromTo functions: (state: '"
				<< mOldStateKey << "')" << std::endl;
	}
	else if (mFiltering)
	{
		std::cerr
				<< "FSM::request_next: Cannot be called in filter functions: (state: '"
				<< mStateKey << "')" << std::endl;
	}
	else if (not mStateSet.empty())
	{
		//there are one or more states (besides Off)
		typename StateSet::iterator iter = mStateSet.find(mStateKey);
		if ((mStateKey == Off) or (++iter == mStateSet.end()))
		{
			//we are in Off or in the last state so go to the first state
			newState = request(mStateSet.begin()->keyName);
		}
		else
		{
			//iter already preincremented so go to the next state
			newState = request(iter->keyName);
		}
	}
	//
	return newState;
}

template<typename StateKey> StateKey FSM<StateKey>::request_prev(
		const ValueList& data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	StateKey newState = Null;
	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::request_prev: Cannot be called in enter/exit/fromTo functions: (state: '"
				<< mOldStateKey << "')" << std::endl;
	}
	else if (mFiltering)
	{
		std::cerr
				<< "FSM::request_prev: Cannot be called in filter functions: (state: '"
				<< mStateKey << "')" << std::endl;
	}
	else if (not mStateSet.empty())
	{
		typename StateSet::iterator iter = mStateSet.find(mStateKey);
		if ((mStateKey == Off) or (iter == mStateSet.begin()))
		{
			//we are in Off or in the first state so go to the last state
			newState = request(mStateSet.rbegin()->keyName);
		}
		else
		{
			//preincrement iter and go to the previous state
			newState = request((--iter)->keyName);
		}
	}
	//
	return newState;

}

template<typename StateKey> typename FSM<StateKey>::FilterFuncPTR FSM<StateKey>::get_current_filter()
{
	if (mStateKey == Off)
	{
		return filter_offPTR;
	}
	typename StateSet::iterator iter = mStateSet.find(mStateKey);
	if (iter->filter.empty())
	{
		//If there's no matching filterState() function, call
		//default_filter() instead.
		return default_filterPTR;
	}
	//
	return iter->filter;
}

template<typename StateKey> StateKey FSM<StateKey>::set_state(
		const StateKey& newStateKey, const ValueList& data)
{
	//Internal function to change unconditionally to the
	//requested new State if it exists or is the Off state
	//(shared among all FMSs); otherwise notifies and returns.
	typename StateSet::iterator iterNew = mStateSet.find(newStateKey);
	if ((newStateKey != Off) and (iterNew == mStateSet.end()))
	{
		//the requested new State key neither exists nor is the Off key
		if (newStateKey == Null)
		{
			std::cerr << "FSM::set_state: State transition has been denied"
					<< std::endl;
		}
		else
		{
			std::cerr << "FSM::set_state: State '" << newStateKey
					<< "' doesn't exist" << std::endl;
		}
		return mStateKey;
	}
	//the new State is the Off state or another existing state
	mOldStateKey = mStateKey;
	mNewStateKey = newStateKey;
	mStateKey = InTransition;
	//check what transition function(s) to call
	std::pair<StateKey, StateKey> fromTo(mOldStateKey, mNewStateKey);
	typename FromToFunctionTable::iterator iter = mFromToFunctions.find(fromTo);
	if (iter != mFromToFunctions.end())
	{
		//call the fromTo function
		(iter->second)(this, data);
	}
	else
	{
		//call the old state exit function
		typename StateSet::iterator iterOld = mStateSet.find(mOldStateKey);
		//at this point (mOldStateKey == Off) is
		//equivalent to (iterOld == mStateSet.end())
		if (iterOld != mStateSet.end())
		{
			//old State != Off State
			if (not iterOld->exit.empty())
			{
				//old State has exit function
				iterOld->exit(this);
			}
			else
			{
				//old State hasn't exit function
				default_exitPTR(this);
			}
		}
		else
		{
			//old State == Off State
			default_exitPTR(this);
		}
		//call the new state enter function
		//at this point (mNewStateKey == Off) is
		//equivalent to (iterNew == mStateSet.end())
		if (iterNew != mStateSet.end())
		{
			//new State != Off State
			if (not iterNew->enter.empty())
			{
				//new State has enter function
				iterNew->enter(this, data);
			}
			else
			{
				//new State hasn't enter function
				default_enterPTR(this, data);
			}
		}
		else
		{
			//new State == Off State
			default_enterPTR(this, data);
		}
	}
	//
	if (mBroadcastStateChanges)
	{
		//send broadcast event
		throw_event(get_state_change_event());
	}
	//transition completed
	mStateKey = mNewStateKey;
	mOldStateKey = Null;
	mNewStateKey = Null;
	//call only the first enqueued request
	if (not mRequestQueue.empty())
	{
		//the correct sequence is:
		//first: copy the enqueued request
		FSMMethodPTR enqueuedMethod = mRequestQueue.front();
		//second: remove it from the queue
		mRequestQueue.pop();
		//third: call it
		enqueuedMethod();
		//This is because if we would call the enqueued request before
		//removing it would find itself on the queue again and again.
	}
	//
	return mStateKey;
}

template<typename StateKey> void FSM<StateKey>::default_enter(FSM<StateKey>* fsm,
		const ValueList& data)
{
}

template<typename StateKey> void FSM<StateKey>::default_exit(FSM<StateKey>* fsm)
{
}

template<typename StateKey> ValueList FSM<StateKey>::default_filter(
		FSM<StateKey>* fsm, const StateKey& toStateKey, const ValueList& data)
{
	ValueList answerList = data;
	//check if to state is Off
	if (toStateKey == fsm->Off)
	{
		//pass through everything: we can always go to the Off state.
		answerList.push_front(toStateKey);
		return answerList;
	}
	//check allowedStateKeys
	StateKey fromStateKey = fsm->get_current_or_next_state();
	typename StateSet::iterator iter = fsm->mStateSet.find(fromStateKey);
	if (iter->allowedStateKeys.empty())
	{
		//allowedStateKeys is empty, it means to accept
		//all requests directed to a particular state.
		answerList.push_front(toStateKey);
		return answerList;
	}
	else
	{
		//allowedStateKeys is not empty, it means to accept
		//only keys present in the set
		if (iter->allowedStateKeys.find(toStateKey)
				!= iter->allowedStateKeys.end())
		{
			//this transition is listed in the allowedStateKeys list;
			//accept it.
			answerList.push_front(toStateKey);
			return answerList;
		}
	}
	//return Null state key in any other case
	std::cerr << "FSM::default_filter: No allowed states" << std::endl;
	answerList.clear();
	answerList.push_front(fsm->Null);
	return answerList;
}

template<typename StateKey> ValueList FSM<StateKey>::filter_off(
		FSM<StateKey>* fsm, const StateKey& toStateKey, const ValueList& data)
{
	ValueList answerList = data;
	//from Off we can always go directly to any other state.
	answerList.push_front(toStateKey);
	return answerList;
}

template<typename StateKey>
bool FSM<StateKey>::add_state(const StateKey& stateKey,
		const FSM<StateKey>::EnterFuncPTR& enterFunc,
		const FSM<StateKey>::ExitFuncPTR& exitFunc,
		const FSM<StateKey>::FilterFuncPTR& filterFunc,
		const FSM<StateKey>::AllowedStateKeySet& allowedStateKeys)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::add_state: Cannot be called in enter/exit/fromTo functions"
				<< std::endl;
		return false;
	}
	//check if stateKey already exists
	typename StateSet::iterator iter = mStateSet.find(stateKey);
	if (iter != mStateSet.end())
	{
		//stateKey exists: try to remove it
		if (not remove_state(stateKey))
		{
			//stateKey cannot be removed
			std::cerr << "FSM::add_state: The state << '" << stateKey
					<< "' cannot be added/updated" << std::endl;
			return false;
		}
	}
	//stateKey doesn't exist: add it
	StateTmpl<StateKey> stateNew(stateKey);
	stateNew.enter = enterFunc;
	stateNew.exit = exitFunc;
	stateNew.filter = filterFunc;
	stateNew.allowedStateKeys = allowedStateKeys;
	//
	mStateSet.insert(stateNew);
	//
	return true;
}

template<typename StateKey>
bool FSM<StateKey>::remove_state(const StateKey& stateKey)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::remove_state: Cannot be called in enter/exit/fromTo functions"
				<< std::endl;
		return false;
	}
	//check if stateKey is the current state or Off state
	if ((stateKey == mStateKey) or (stateKey == Off))
	{
		std::cerr << "FSM::remove_state: Current state << '" << stateKey
				<< "' cannot be removed" << std::endl;
		return false;
	}
	//check if stateKey exists and remove it
	if (mStateSet.erase(stateKey) == 0)
	{
		std::cerr << "FSM::remove_state: State '" << stateKey
				<< "' doesn't exist" << std::endl;
		return false;
	}
	//stateKey removed
	return true;
}

template<typename StateKey>
bool FSM<StateKey>::add_from_to_func(const StateKey& stateFrom,
		const StateKey& stateTo, const FSM<StateKey>::FromToFuncPTR& fromToFunc)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::add_from_to_func: Cannot be called in enter/exit/fromTo functions"
				<< std::endl;
		return false;
	}
	//check if fromToFunc already exists
	std::pair<StateKey, StateKey> fromToStates(stateFrom, stateTo);
	typename FromToFunctionTable::iterator iter = mFromToFunctions.find(
			fromToStates);
	if (iter != mFromToFunctions.end())
	{
		//fromToFunc exists: try to remove it
		if (not remove_from_to_func(stateFrom, stateTo))
		{
			//fromToFunc cannot be removed
			std::cerr << "FSM::add_from_to_func: Function from state '"
					<< stateFrom << "' to state '" << stateTo
					<< "' cannot be added/updated" << std::endl;
			return false;
		}
	}
	//fromToFunc doesn't exist: add it
	mFromToFunctions[fromToStates] = fromToFunc;
	//
	return true;
}

template<typename StateKey>
bool FSM<StateKey>::remove_from_to_func(const StateKey& stateFrom,
		const StateKey& stateTo)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::remove_from_to_func: Cannot be called in enter/exit/fromTo functions"
				<< std::endl;
		return false;
	}
	//check if from to function already exists and remove it
	std::pair<StateKey, StateKey> fromToStates(stateFrom, stateTo);
	if (mFromToFunctions.erase(fromToStates) == 0)
	{
		std::cerr << "FSM::remove_from_to_func: Function from state '" << stateFrom
				<< "' to state '" << stateTo << "' doesn't exist" << std::endl;
		return false;
	}
	//from to function removed
	return true;
}

#ifdef ELY_THREAD
template<typename StateKey> inline ReMutex& FSM<StateKey>::getMutex()
{
	return mMutex;
}
#endif

//specializations' declarations
//StateKey == std::string
template<> FSM<std::string>::FSM(const std::string& name);
//StateKey == int
template<> FSM<int>::FSM(const int& name);

//typedefs
//StateKey == std::string
typedef FSM<std::string> fsm;
typedef FSM<std::string>::StateTmpl<std::string> State;
//StateKey == int
typedef FSM<int> fsmi;
typedef FSM<int>::StateTmpl<int> Statei;

#endif /* FSM_I_ */
