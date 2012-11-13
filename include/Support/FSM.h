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
 * \file /Ely/include/Support/FSM.h
 *
 * \date 02/ago/2012 (09:50:25)
 * \author marco
 */

#ifndef FSM_H_
#define FSM_H_

#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <queue>
#include <list>
#include <algorithm>
#include <utility>
#include <throw_event.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include <referenceCount.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/any.hpp>

#include "Utilities/Tools.h"

/**
 * \brief The Value list type.
 */
typedef std::list<boost::any> ValueList;

/**
 * \brief A Finite State Machine template class.
 *
 * This is intended to be the class of any number of specific machines,
 * which consist of a collection of states and transitions, and rules
 * to switch between states according to arbitrary input data.\n
 * The states of an FSM are defined implicitly by a state key whose
 * type is represented the template parameter StateKey and has the same
 * requirements of the key type for associative containers (suitable to
 * be strict weak ordered) and should be printable to an std::ostream.
 * Also state keys are passed as request that are not state names but
 * arbitrary input that will be first filtered.\n
 * FSM use a function "callback" system to define its behavior.
 * To define specialized behavior when entering or exiting a
 * particular State, define "Enter"/"Exit" State Function(s) for
 * enter to and/or exit from the State.\n
 * There is a way to define specialized transition behavior between
 * two particular states.  This is done by defining a "FromTo" function:
 * from X To Y, where X is the old state and Y is the new state.  If this
 * is defined, it will be run in place of the exit X and enter Y
 * functions, so if you want that behavior, you'll have to call them
 * specifically.  Otherwise, you can completely replace that transition's
 * behavior.\n
 * All functions can access the public FSM interface through a pointer.\n
 * All functions are optional.  If a function is omitted, the
 * state is still defined, but nothing is done during transition
 * into (or out of) the state (this is the default behavior).\n
 * Additionally, you may define a filter State StateFunction for each
 * state.  The purpose of this function is to decide what state to
 * transition to next, if any, on receipt of a particular input. The
 * input is always a State and the return value should be a list of
 * values in which the first element is the State key we would to
 * transition into.\n
 * As above, the filter State functions are optional.\n
 * If any is omitted, the defaultFilter() method is called instead.
 * A standard implementation of defaultFilter() is provided.\n
 * The current state may be queried at any time other than
 * during the handling of the enter, exit, fromTo functions. During these
 * functions, current state contains the value InTransition (you are
 * not really in any state during the transition).\n
 * However, during a transition you *can* query the outgoing and incoming
 * states via getCurrentStateOrTransition() or getCurrentOrNextState().
 * At other times, you *can* query the current State via
 * getCurrentOrNextState().\n
 * Initially, the FSM is in state Off State. It does not call enter Off
 * State function at construction time; it is simply in Off State already
 * by convention.\n
 * If you need to call code in enter Off State to initialize your FSM
 * properly, call it explicitly in the constructor.  Similarly, when
 * cleanup () is called or the FSM is cleaned up, the FSM transitions
 * back to Off State by convention.\n
 * To implement nested hierarchical FSM's, simply create a nested FSM
 * and store it on the class within the appropriate enter State
 * function, and clean it up within the corresponding exit State
 * function.
 *
 * Provided functions could be free functions, function objects,
 * boost::function objects and should have these signatures
 * (where Key is the type of the state key):\n
 * - <em>Enter functions</em>: <tt> void f(FSM<Key>*, const ValueList&); </tt>
 * - <em>Exit functions</em>: <tt> void f(FSM<Key>*); </tt>
 * - <em>FromTo functions</em>: <tt> void f(FSM<Key>*, const ValueList&); </tt>
 * - <em>Filter functions</em>: <tt> ValueList f(const StateKey&, const StateKey&, const ValueList&); </tt>
 * \note the Filter functions must return a ValueList with the first element
 * set to a state key or Null if the transition must be denied.
 *
 */
template<typename StateKey> class FSM
{
public:
	//Public types
	/**
	 * \name The Enter/Exit/FromTo/Filter callbacks boost::function types.
	 *
	 * 	Enter/FromTo/Filter callbacks have a ValueList parameter representing
	 * 	custom data, which can be passed by the various request/Next/Prev,
	 * 	demand and forceTransition FSM functions.
	 */
	///@{
	typedef boost::function<void(FSM<StateKey>*, const ValueList&)> EnterFuncPTR;
	typedef boost::function<void(FSM<StateKey>*)> ExitFuncPTR;
	typedef boost::function<void(FSM<StateKey>*, const ValueList&)> FromToFuncPTR;
	///For a filter the first element of the returned ValueList is a
	///state key corresponding to the state to transition to, or the
	///value pFSM->Null if a transition is denied (here pFSM is the
	///passed pointer to the FSM).
	typedef boost::function<
			ValueList(FSM<StateKey>*, const StateKey&, const ValueList&)> FilterFuncPTR;
	typedef std::set<StateKey> AllowedStateKeySet;
	///@}

	/**
	 * \brief The StateTmpl type.
	 *
	 * Key is the type of the key by which this StateTmpl can be retrieved,
	 * added, updated, deleted from a repository.\n
	 * Key type has the same requirements of the key type for associative
	 * containers (suitable to be strict weak ordered) and should
	 * be printable on an std::ostream.\n
	 * \note To make a type Key printable on an std::ostream, you could
	 * define a global overloading of the "<<" operator, for example:
	 * \code
	 *	std::ostream& operator<<(const std::ostream&, const Key&);
	 * \endcode
	 */
	template<typename Key> struct StateTmpl
	{
		/**
		 * \brief Converting constructor from Key.
		 *
		 * For using Key in find algorithms.
		 */
		StateTmpl(const Key& _name) :
				keyName(_name)
		{
		}

		/**
		 * \name Strict ordering.
		 *
		 * Sequence key requirement.
		 */
		bool operator<(const StateTmpl& other) const
		{
			return keyName < other.keyName;
		}

		/**
		 * \name Members.
		 */
		///@{
		Key keyName;
		EnterFuncPTR enter;
		ExitFuncPTR exit;
		FilterFuncPTR filter;

		///These are the default allowed state keys for transitions
		///from this state and are examined by the default filter;
		///if empty all state keys are allowed.
		AllowedStateKeySet allowedStateKeys;
		///@}
	};

protected:
	//Protected Interface
	/**
	 * \name Default enter definitions.
	 */
	///@{
	/**
	 * This is the default function that is called if there is no
	 * enter State function for a particular state name.
	 * @param fsm The current FSM.
	 * @param data The custom data passed to this callback.
	 */
	static void defaultEnter(FSM<StateKey>* fsm, const ValueList& data);
	EnterFuncPTR defaultEnterPTR;
	///@}

	/**
	 * \name Default exit definitions.
	 */
	///@{
	/**
	 * This is the default function that is called if there is no
	 * exit State function for a particular state name.
	 * @param fsm The current FSM.
	 */
	static void defaultExit(FSM<StateKey>* fsm);
	ExitFuncPTR defaultExitPTR;
	///@}

	/**
	 * \name Default filter definitions.
	 */
	///@{
	/**
	 * This is the function that is called if there is no
	 * filter State function for a particular state name.\n
	 * This default filter function behaves in one of two modes:
	 * - if state.allowedStateKeys is empty, allow any request
	 * which is assumed to be a direct request to a particular state.
	 * - if state.allowedStateKeys is not empty, allow only those
	 * requests explicitly identified in this set.
	 * \note: the Filter functions must return a ValueList with the
	 * first element set to a state key or Null if the transition
	 * is denied.
	 * @param fsm The current FSM.
	 * @param toStateKey The destination state.
	 * @param data The custom data passed to this callback.
	 */
	static ValueList defaultFilter(FSM<StateKey>* fsm,
			const StateKey& toStateKey, const ValueList& data);
	FilterFuncPTR defaultFilterPTR;
	///@}

	/**
	 * \name Off State filter function definitions.
	 */
	///@{
	/**
	 * This Off State filter function allows always to go directly into
	 * any other state.
	 * \note: the Filter functions must return a ValueList with the
	 * first element set to a state key or Null if the transition
	 * is denied.
	 *
	 * @param fsm The current FSM.
	 * @param toStateKey The destination state.
	 * @param data The custom data passed to this callback.
	 */
	static ValueList filterOff(FSM<StateKey>* fsm, const StateKey& toStateKey,
			const ValueList& data);
	FilterFuncPTR filterOffPTR;
	///@}

	/**
	 * \brief Return the current filter function.
	 * @return The current filter function.
	 */
	FilterFuncPTR getCurrentFilter();

	/**
	 * \brief Internal function to change unconditionally to the indicated
	 * state if possible.
	 *
	 * @param newStateKey The destination state.
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 * @return The current state of this FSM.
	 */
	StateKey setState(const StateKey& newStateKey, const ValueList& data =
			ValueList());

	/**
	 * \brief Initialization function.
	 */
	void initialize(const StateKey& name);

	/**
	 * \brief The FSM "InTransition" StateKey constant.
	 *
	 * Represents an internal flag meaning that FSM is in
	 * the midst of a transition.
	 */
	const StateKey InTransition;

public:
	//Public Interface
	/**
	 * \brief The State set type.
	 */
	typedef std::set<StateTmpl<StateKey> > StateSet;
	/**
	 * \brief The FromToFunctions' table type.
	 */
	typedef std::map<std::pair<StateKey, StateKey>, FromToFuncPTR> FromToFunctionTable;

	/**
	 * \brief The FSM queued methods' boost::function type.
	 */
	typedef boost::function<void(void)> FSMMethodPTR;

	/**
	 * \brief The FSM "Null" StateKey constant.
	 *
	 * Indicates an error condition or denied transition.
	 */
	const StateKey Null;

	/**
	 * \brief The FSM "Off" StateKey constant.
	 *
	 * Represents the default startup state shared by all FMS instances.
	 */
	const StateKey Off;

	/**
	 * \brief Constructor.
	 *
	 * This constructor should be specialized to differentiate between
	 * Null, Off, InTransition state key constants, because them are
	 * all default initialized in the primary template.
	 * @param name The FSM's name.
	 */
	FSM(const StateKey& name);

	/**
	 * \brief Destructor.
	 */
	virtual ~FSM();

	/**
	 * \brief A convenience function to force the FSM to clean itself up
	 * by transitioning to the OffState.
	 */
	void cleanup();

	/**
	 * \brief Returns the current state or next state.
	 *
	 * Returns the current state if this is called from external code,
	 * or the state we are transitioning into if this is called by an enter,
	 * exit or fromTo function (i.e. we are performing a transition)
	 * for a state.
	 * @return The current or next state.
	 */
	const StateKey& getCurrentOrNextState();

	/**
	 * \brief Returns the current (and the next) state.
	 *
	 * If it is called from external code returns true: currState will
	 * contain the current state key (nextState is ignored).\n
	 * If it is called by an enter, exit or fromTo function (i.e. we are
	 * performing a transition) returns false: currState and
	 * toState will contain the "from-state" and the "to-state" of the
	 * transition.
	 * @param currStateKey A reference to a variable that will contain the
	 * current or the transition from-state (out parameter).
	 * @param toStateKey A reference to a variable that will be ignored or
	 * will contain the transition to-state (out parameter).
	 * @return True if we are in a state, false if we are performing a
	 * transition.
	 */
	bool getCurrentStateOrTransition(StateKey& currStateKey,
			StateKey& toStateKey);

	/**
	 * \brief Return if a transition is currently active.
	 * @return If a transition is currently active.
	 */
	bool isInTransition();

	/**
	 * \brief Changes unconditionally to the indicated state.
	 *
	 * This bypasses the filter function, and just calls exit function
	 * followed by enter or a fromTo function.\n
	 * This method could be called from external code and the change
	 * (if possible) is performed immediately.\n
	 * This method could be called by an enter, exit or fromTo function
	 * (i.e. when we are performing a transition) and the request is queued
	 * up and will be executed when the current transition finishes. Multiple
	 * requests will queue up in sequence.\n
	 * Nothing is returned, see setState() when an error occurs.
	 * @param stateKey The destination state.
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 */
	void forceTransition(const StateKey& stateKey, const ValueList& data =
			ValueList());

	/**
	 * \brief Requests a state transition, by code that does not expect
	 * the request to be denied.
	 *
	 * This method could be called from external code and the change
	 * (if possible) is performed immediately.\n
	 * This method could be called by an enter, exit or fromTo function
	 * (i.e. when we are performing a transition) and the request is queued
	 * up and will be executed when the current transition finishes. Multiple
	 * requests will queue up in sequence.\n
	 * Nothing is returned, see request() when an error occurs.
	 * @param stateKey The destination state.\n
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 */
	void demand(const StateKey& stateKey, const ValueList& data = ValueList());

	/**
	 * \brief Requests a state transition (or other behavior).
	 *
	 * The request may be denied by the FSM's filter function.
	 * If it is denied, the filter function return a Null key.
	 * The request parameter should the new state key.\n
	 * The request, along with any additional arguments, is passed to
	 * the current filter State function.  If filter State returns a
	 * state key, the FSM transitions to that state.\n
	 * The return value is the same as the return value of the setState()
	 * function (that is, the state the FSM is currently into).\n
	 * This method *cannot* be called by an enter, exit or fromTo function
	 * (i.e. when we are performing a transition): if this happens it
	 * returns Null (implicitly the current state is unchanged).\n
	 * Moreover this function *cannot* be called in a filter function.\n
	 * See demand(), which will queue these requests up and apply when the
	 * transition is complete.
	 * @param newStateKey The destination state.
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 * @return The state the FSM is currently into after the request
	 * or Null if an unauthorized call or an error occurred.
	 */
	StateKey request(const StateKey& newStateKey, const ValueList& data =
			ValueList());

	/**
	 * \brief Request the 'next' state in the predefined state array.
	 *
	 * This method *cannot* be called by an enter, exit or fromTo function
	 * (i.e. when we are performing a transition): if this happens it
	 * returns Null (implicitly the current state is unchanged).\n
	 * Moreover this function *cannot* be called in a filter function.
	 * \note The elements in a StateSet are always sorted from lower to
	 * higher following the specific strict weak ordering criterion of
	 * the state key type.
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 * @return See request().
	 */
	StateKey requestNext(const ValueList& data = ValueList());

	/**
	 * \brief Request the 'previous' state in the predefined state array.
	 *
	 * This method *cannot* be called by an enter, exit or fromTo function
	 * (i.e. when we are performing a transition): if this happens it
	 * returns Null (implicitly the current state is unchanged).\n
	 * Moreover this function *cannot* be called in a filter function.
	 * \note The elements in a std::set are always sorted from lower to
	 * higher following a specific strict weak ordering criterion set
	 * on container construction
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 * @return See request().
	 */
	StateKey requestPrev(const ValueList& data = ValueList());

	/**
	 * \name State change broadcasting.
	 *
	 * If setBroadcastStateChanges(true), this event will be sent through
	 * the EventHandler on every state change.\n
	 * The new and old states are accessible as mOldState and mNewState,
	 * and the transition functions will already have been called.
	 */
	///@{
	/**
	 * @param doBroadcast Flag to enable/disable broadcast.
	 */
	void setBroadcastStateChanges(bool doBroadcast);
	std::string getStateChangeEvent();
	///@}

	/**
	 * \name FSM construction functions.
	 */
	///@{
	bool addState(const StateKey& stateKey,
			const FSM<StateKey>::EnterFuncPTR& enterFunc,
			const FSM<StateKey>::ExitFuncPTR& exitFunc,
			const FSM<StateKey>::FilterFuncPTR& filterFunc,
			const FSM<StateKey>::AllowedStateKeySet& allowedStateKeys =
					AllowedStateKeySet());
	bool removeState(const StateKey& stateKey);
	bool addFromToFunc(const StateKey& stateFrom, const StateKey& stateTo,
			const FSM<StateKey>::FromToFuncPTR& fromToFunc);
	bool removeFromToFunc(const StateKey& stateFrom, const StateKey& stateTo);
	///@}

	/**
	 * \brief Set the states' set of this FSM in one shot.
	 * @param stateSet The state set.
	 */
	void setStateSet(const StateSet& stateSet);

	/**
	 * \brief Return the set of state keys.
	 * @return The set of state keys.
	 */
	std::set<StateKey> getKeyStateSet();

	/**
	 * \brief Return the number of the states belonging to this FSM
	 * other than the Off initial state.
	 * @return The number of the states (Off apart).
	 */
	unsigned int getNumStates();

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

protected:
	//Data section
	///The Global serial number.
	static int SerialNum;
	/**
	 * \brief Return a  unique global serial number.
	 * @return
	 */
	int getSerialNum();
	///The serial number of this fsm.
	int mSerialNum;
	///The name of this fsm.
	StateKey mName;
	///The state set.
	StateSet mStateSet;
	///The FromToFunction table
	FromToFunctionTable mFromToFunctions;

	///@{
	///The current/old/new states.
	///Initially, we are in the Off state by convention.
	///During a transition mState is set in the "Null" State.
	StateKey mStateKey, mOldStateKey, mNewStateKey;
	///@}

	///Flag for notifying on every state change.
	bool mBroadcastStateChanges;

	///Flag indicating if a filter is being called.
	bool mFiltering;

	///This member records transition requests made by demand() or
	///forceTransition() while the FSM is in transition between
	///states.
	std::queue<FSMMethodPTR> mRequestQueue;

	///The (reentrant) mutex associated with this FSM.
	ReMutex mMutex;
};
////////////////////////////////////////////////////////////////////////
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
	HOLDMUTEX(mMutex)
}

template<typename StateKey> void FSM<StateKey>::initialize(const StateKey& name)
{
	//set FSM name
	mName = name;
	//set serial number
	mSerialNum = FSM<StateKey>::getSerialNum();
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
	defaultEnterPTR = &FSM<StateKey>::defaultEnter;
	defaultExitPTR = &FSM<StateKey>::defaultExit;
	defaultFilterPTR = &FSM<StateKey>::defaultFilter;
	filterOffPTR = &FSM<StateKey>::filterOff;
}

template<typename StateKey> int FSM<StateKey>::getSerialNum()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return ++FSM<StateKey>::SerialNum;
}

template<typename StateKey> void FSM<StateKey>::cleanup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey != Off)
	{
		setState(Off);
	}
	//cleanup the FSM: reset all data
	mStateSet.clear();
	mFromToFunctions.clear();
	//set broadcast state change
	mBroadcastStateChanges = false;
}

template<typename StateKey> void FSM<StateKey>::setBroadcastStateChanges(
		bool doBroadcast)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mBroadcastStateChanges = doBroadcast;
}

template<typename StateKey> std::string FSM<StateKey>::getStateChangeEvent()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	std::ostringstream stateChange;
	stateChange << "FSM-" << mSerialNum << "-" << mName << "-stateChange";
	return stateChange.str();
}

template<typename StateKey> const StateKey& FSM<StateKey>::getCurrentOrNextState()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey != InTransition)
	{
		return mStateKey;
	}
	return mNewStateKey;
}

template<typename StateKey> bool FSM<StateKey>::getCurrentStateOrTransition(
		StateKey& currStateKey, StateKey& toStateKey)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey != InTransition)
	{
		currStateKey = mStateKey;
		return true;
	}
	currStateKey = mOldStateKey;
	toStateKey = mNewStateKey;
	return false;
}

template<typename StateKey> bool FSM<StateKey>::isInTransition()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mStateKey == InTransition;
}

template<typename StateKey> void FSM<StateKey>::forceTransition(
		const StateKey& stateKey, const ValueList& data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		// Queue up the request for later calling.
		FSMMethodPTR forceTransitionPTR = boost::bind(
				&FSM<StateKey>::forceTransition, boost::ref(*this), stateKey,
				data);
		mRequestQueue.push(forceTransitionPTR);
		return;
	}
	setState(stateKey, data);
}

template<typename StateKey> void FSM<StateKey>::demand(const StateKey& stateKey,
		const ValueList& data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
	HOLDMUTEX(mMutex)

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
		ValueList result = getCurrentFilter()(this, newStateKey, data);
		//filtering off
		mFiltering = false;
		try
		{
			//check first if value is of State key type
			acceptedStateKey = boost::any_cast<StateKey>(result.front());
			//remove first value
			result.pop_front();
			//go to (potentially) new key state (could be Null)
			acceptedStateKey = setState(acceptedStateKey, result);
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

template<typename StateKey> void FSM<StateKey>::setStateSet(
		const StateSet& stateSet)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mStateSet = stateSet;
}

template<typename StateKey> std::set<StateKey> FSM<StateKey>::getKeyStateSet()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	std::set<StateKey> keyStateSet;
	typename StateSet::iterator iter;
	for (iter = mStateSet.begin(); iter != mStateSet.end(); ++iter)
	{
		keyStateSet.insert(iter->keyName);
	}
	return keyStateSet;
}

template<typename StateKey> unsigned int FSM<StateKey>::getNumStates()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mStateSet.size();
}

template<typename StateKey> StateKey FSM<StateKey>::requestNext(
		const ValueList& data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	StateKey newState = Null;
	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::requestNext: Cannot be called in enter/exit/fromTo functions: (state: '"
				<< mOldStateKey << "')" << std::endl;
	}
	else if (mFiltering)
	{
		std::cerr
				<< "FSM::requestNext: Cannot be called in filter functions: (state: '"
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

template<typename StateKey> StateKey FSM<StateKey>::requestPrev(
		const ValueList& data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	StateKey newState = Null;
	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::requestPrev: Cannot be called in enter/exit/fromTo functions: (state: '"
				<< mOldStateKey << "')" << std::endl;
	}
	else if (mFiltering)
	{
		std::cerr
				<< "FSM::requestPrev: Cannot be called in filter functions: (state: '"
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

template<typename StateKey> typename FSM<StateKey>::FilterFuncPTR FSM<StateKey>::getCurrentFilter()
{
	if (mStateKey == Off)
	{
		return filterOffPTR;
	}
	typename StateSet::iterator iter = mStateSet.find(mStateKey);
	if (iter->filter.empty())
	{
		//If there's no matching filterState() function, call
		//defaultFilter() instead.
		return defaultFilterPTR;
	}
	//
	return iter->filter;
}

template<typename StateKey> StateKey FSM<StateKey>::setState(
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
			std::cerr << "FSM::setState: State transition has been denied"
					<< std::endl;
		}
		else
		{
			std::cerr << "FSM::setState: State '" << newStateKey
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
				defaultExitPTR(this);
			}
		}
		else
		{
			//old State == Off State
			defaultExitPTR(this);
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
				defaultEnterPTR(this, data);
			}
		}
		else
		{
			//new State == Off State
			defaultEnterPTR(this, data);
		}
	}
	//
	if (mBroadcastStateChanges)
	{
		//send broadcast event
		throw_event(getStateChangeEvent());
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

template<typename StateKey> void FSM<StateKey>::defaultEnter(FSM<StateKey>* fsm,
		const ValueList& data)
{
}

template<typename StateKey> void FSM<StateKey>::defaultExit(FSM<StateKey>* fsm)
{
}

template<typename StateKey> ValueList FSM<StateKey>::defaultFilter(
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
	StateKey fromStateKey = fsm->getCurrentOrNextState();
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
	std::cerr << "FSM::defaultFilter: No allowed states" << std::endl;
	answerList.clear();
	answerList.push_front(fsm->Null);
	return answerList;
}

template<typename StateKey> ValueList FSM<StateKey>::filterOff(
		FSM<StateKey>* fsm, const StateKey& toStateKey, const ValueList& data)
{
	ValueList answerList = data;
	//from Off we can always go directly to any other state.
	answerList.push_front(toStateKey);
	return answerList;
}

template<typename StateKey>
bool FSM<StateKey>::addState(const StateKey& stateKey,
		const FSM<StateKey>::EnterFuncPTR& enterFunc,
		const FSM<StateKey>::ExitFuncPTR& exitFunc,
		const FSM<StateKey>::FilterFuncPTR& filterFunc,
		const FSM<StateKey>::AllowedStateKeySet& allowedStateKeys)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::addState: Cannot be called in enter/exit/fromTo functions"
				<< std::endl;
		return false;
	}
	//check if stateKey already exists
	typename StateSet::iterator iter = mStateSet.find(stateKey);
	if (iter != mStateSet.end())
	{
		//stateKey exists: try to remove it
		if (not removeState(stateKey))
		{
			//stateKey cannot be removed
			std::cerr << "FSM::addState: The state << '" << stateKey
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
bool FSM<StateKey>::removeState(const StateKey& stateKey)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::removeState: Cannot be called in enter/exit/fromTo functions"
				<< std::endl;
		return false;
	}
	//check if stateKey is the current state or Off state
	if ((stateKey == mStateKey) or (stateKey == Off))
	{
		std::cerr << "FSM::removeState: Current state << '" << stateKey
				<< "' cannot be removed" << std::endl;
		return false;
	}
	//check if stateKey exists and remove it
	if (mStateSet.erase(stateKey) == 0)
	{
		std::cerr << "FSM::removeState: State '" << stateKey
				<< "' doesn't exist" << std::endl;
		return false;
	}
	//stateKey removed
	return true;
}

template<typename StateKey>
bool FSM<StateKey>::addFromToFunc(const StateKey& stateFrom,
		const StateKey& stateTo, const FSM<StateKey>::FromToFuncPTR& fromToFunc)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::addFromToFunc: Cannot be called in enter/exit/fromTo functions"
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
		if (not removeFromToFunc(stateFrom, stateTo))
		{
			//fromToFunc cannot be removed
			std::cerr << "FSM::addFromToFunc: Function from state '"
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
bool FSM<StateKey>::removeFromToFunc(const StateKey& stateFrom,
		const StateKey& stateTo)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStateKey == InTransition)
	{
		std::cerr
				<< "FSM::removeFromToFunc: Cannot be called in enter/exit/fromTo functions"
				<< std::endl;
		return false;
	}
	//check if from to function already exists and remove it
	std::pair<StateKey, StateKey> fromToStates(stateFrom, stateTo);
	if (mFromToFunctions.erase(fromToStates) == 0)
	{
		std::cerr << "FSM::removeFromToFunc: Function from state '" << stateFrom
				<< "' to state '" << stateTo << "' doesn't exist" << std::endl;
		return false;
	}
	//from to function removed
	return true;
}

template<typename StateKey> ReMutex& FSM<StateKey>::getMutex()
{
	return mMutex;
}

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

#endif /* FSM_H_ */
