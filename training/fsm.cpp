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
 * \file /Ely/training/fsm.cpp
 *
 * \date 24/lug/2012 (19:28:21)
 * \author marco
 */

//#include <iostream>
//#include <vector>
//#include <load_prc_file.h>
//#include <auto_bind.h>
//#include <partBundleHandle.h>
//#include <character.h>
//#include <animControlCollection.h>
//#include <pandaFramework.h>
//
//#include <panda3d/bulletWorld.h>
//// $Id: FSM.java 12 2009-11-09 22:58:47Z gabe.johnson $
//
////package org.six11.util.data;
//
////import org.six11.util.Debug;
//
////import java.util.ArrayList;
////import java.util.HashMap;
////import java.util.List;
////import java.util.Map;
////import java.util.NoSuchElementException;
////
////import javax.swing.event.ChangeEvent;
////import javax.swing.event.ChangeListener;
//
////http://www.java2s.com/Code/Java/Collections-Data-Structure/AprogrammableFiniteStateMachineimplementation.htm
//
//#include <string>
//#include <map>
//#include <pointerTo.h>
//#include <typedWritableReferenceCount.h>
//
///**
// * \brief Type of transition function.
// */
//typedef void TransitionFunction(void*);
//
///**
// * Create a new transition. See the documentation for addEvent and
// * addTransition in FSM.
// */
//class Transition: public TypedWritableReferenceCount
//{
//public:
//	/**
//	 * \brief Constructor.
//	 *
//	 * Create a transition object that responds to the given event when in
//	 * the given startState, and puts the FSM into the endState provided.
//	 */
//	Transition(std::string evtName, std::string startState,
//			std::string endState)
//	{
//		mEvtName = evtName;
//		mStartState = startState;
//		mEndState = endState;
//	}
//
//	/**
//	 * \brief Destructor.
//	 */
//	virtual ~Transition()
//	{
//	}
//
//	/**
//	 * \brief Executes just before a transition.
//	 *
//	 * Override this to have FSM execute code immediately before following a
//	 * state transition.
//	 */
//	virtual void doBeforeTransition()
//	{
//	}
//
//	/**
//	 * \brief Executes just after a transition.
//	 *
//	 * Override this to have FSM execute code immediately after following a
//	 * state transition.
//	 */
//	virtual void doAfterTransition()
//	{
//	}
//
//	std::string& endState() const
//	{
//		return mEndState;
//	}
//
//	std::string& evtName() const
//	{
//		return mEvtName;
//	}
//
//	std::string& startState() const
//	{
//		return mStartState;
//	}
//
//private:
//	std::string mEvtName;
//	std::string mStartState;
//	std::string mEndState;
//
//};
//
///**
// * Represents a state with some number of associated transitions.
// */
//class State: public TypedWritableReferenceCount
//{
//public:
//
//	typedef std::map<std::string, PT(Transition)> TransitionTable;
//	typedef TransitionTable::value_type TransitionTableEntry;
//
//	State(TransitionFunction* entryCode, TransitionFunction* exitCode,
//			TransitionFunction* alwaysRunCode)
//	{
//		mAutoTransitionState = NULL;
//		mTransitions.clear();
//		mEntryCode = entryCode;
//		mExitCode = exitCode;
//		mAlwaysRunCode = alwaysRunCode;
//	}
//
//	void addTransition(Transition* trans)
//	{
//		mTransitions.insert(TransitionTableEntry(trans->evtName(), trans));
//	}
//
//	void runEntryCode(void* data)
//	{
//		if (mEntryCode != NULL)
//		{
//			mEntryCode(data);
//		}
//	}
//
//	void runExitCode(void* data)
//	{
//		if (mExitCode != NULL)
//		{
//			mExitCode(data);
//		}
//	}
//
//	void runAlwaysCode(void* data)
//	{
//		if (mAlwaysRunCode != NULL)
//		{
//			mAlwaysRunCode(data);
//		}
//	}
//
//	TransitionFunction*& alwaysRunCode()
//	{
//		return mAlwaysRunCode;
//	}
//
//	TransitionFunction*& entryCode()
//	{
//		return mEntryCode;
//	}
//
//	TransitionFunction*& exitCode()
//	{
//		return mExitCode;
//	}
//
//protected:
//	TransitionTable mTransitions;
//	std::string mAutoTransitionState;
//	TransitionFunction* mEntryCode;
//	TransitionFunction* mExitCode;
//	TransitionFunction* mAlwaysRunCode;
//
//};
//
///**
// * \brief A programmable Finite State Machine implementation.
// *
// * To use this class, establish any number of states with the 'addState' method.
// * Next, add some FSM.Transition objects (the Transition class is designed to be
// * used as an superclass for your anonymous implementation). Each Transition object
// * has two useful methods that can be defined by your implementation:
// * doBeforeTransition and doAfterTransition. To drive your FSM, simply give it
// * events using the addEvent method with the name of an event. If there is an
// * appropriate transition for the current state, the transition's doBefore/doAfter
// * methods are called and the FSM is put into the new state. It is legal (and highly
// * useful) for the start/end states of a transition to be the same state.
// **/
//class FSM
//{
//	// This class implements a Flying Spaghetti Monster
//
//public:
//
//	typedef std::map<std::string, PT(State)> StateTable;
//	typedef StateTable::value_type StateTableEntry;
//
//	/**
//	 * \brief Creates a blank FSM with the given name (which is arbitrary).
//	 */
//	FSM(const std::string& name)
//	{
//		mName = name;
//		mStates.clear();
//		mCurrentState = std::string("");
////    this.changeListeners = new ArrayList<ChangeListener>();
//	}
//
//	/**
//	 * Turn debugging on/off.
//	 */
//	void setDebugMode(bool debug)
//	{
//		mDebug = debug;
//	}
//
//	/**
//	 * \brief Report the current state of the finite state machine.
//	 */
//	const std::string& currentState()
//	{
//		return mCurrentState;
//	}
//
//	/**
//	 * \brief Adds a new state with no entry or exit code.
//	 */
//	void addState(const std::string& state)
//	{
//		addState(state, NULL, NULL, NULL);
//	}
//
//	/**
//	 * \brief Establishes a new state the FSM is aware of.
//	 *
//	 * If the FSM does not currently have any states, this state becomes
//	 * the current, initial state. This is the only way to put the FSM
//	 * into an initial state.
//	 *
//	 * The entryCode, exitCode, and alwaysRunCode are TransitionFunction that
//	 * the FSM executes during the course of a transition. entryCode and
//	 * exitCode are run only if the transition is between two distinct states
//	 * (i.e. A->B where A != B). alwaysRunCode is executed even if the
//	 * transition is re-entrant (i.e. A->B where A = B).
//	 **/
//	void addState(const std::string& state, TransitionFunction* entryCode,
//			TransitionFunction* exitCode, TransitionFunction* alwaysRunCode)
//	{
//		bool isInitial = (mStates.size() == 0 ? true : false);
//		if (mStates.count(state) == 0)
//		{
//mStates		.insert(StateTableEntry(state,
//						new State(entryCode, exitCode, alwaysRunCode));
//			}
//
//		if (isInitial)
//		{
//			setState(state);
//		}
//	}
//
//	void setStateEntryCode(const std::string& state,
//			TransitionFunction* entryCode)
//	{
//		if (mStates.count(state) != 0)
//		{
//			mStates[state]->entryCode() = entryCode;
//		}
//	}
//
//	void setStateExitCode(const std::string& state,
//			TransitionFunction* exitCode)
//	{
//		if (mStates.count(state) != 0)
//		{
//			mStates[state]->exitCode() = exitCode;
//		}
//	}
//
//	void setStateAlwaysRunCode(const std::string& state,
//			TransitionFunction* alwaysRunCode)
//	{
//		if (mStates.count(state) != 0)
//		{
//			mStates[state]->alwaysRunCode() = alwaysRunCode;
//		}
//	}
//
//	/**
//	 * There are cases where a state is meant to be transitional, and the FSM
//	 * should always immediately transition to some other state. In those cases,
//	 * use this method to specify the start and end states. After the startState
//	 * has fully transitioned (and any change events have been fired) the FSM
//	 * will check to see if there is another state that the FSM should
//	 * automatically transition to. If there is one, addEvent(endState) is
//	 * called.
//	 *
//	 * Note: this creates a special transition in the lookup table called
//	 * "(auto)".
//	 */
//	void setAutoTransition(const std::string& startState, const std::string& endState)
//	{
//		// if (debug) {
//		// Debug.out("FSM", "Establishing auto transition for " + startState +
//		// " -> " + endState);
//		// }
//		mStates.get(startState).autoTransitionState = endState;
//		addTransition(new Transition("(auto)", startState, endState));
//	}
//
//	/**
//	 * Sets the current state without following a transition. This will cause a
//	 * change event to be fired.
//	 */
//	void setState(const std::string& state)
//	{
//		setState(state, true);
//	}
//
//	/**
//	 * Sets the current state without following a transition, and optionally
//	 * causing a change event to be triggered. During state transitions (with
//	 * the 'addEvent' method), this method is used with the triggerEvent
//	 * parameter as false.
//	 *
//	 * The FSM executes non-null runnables according to the following logic,
//	 * given start and end states A and B:
//	 *
//	 * <ol>
//	 * <li>If A and B are distinct, run A's exit code.</li>
//	 * <li>Record current state as B.</li>
//	 * <li>Run B's "alwaysRunCode".</li>
//	 * <li>If A and B are distinct, run B's entry code.</li>
//	 * </ol>
//	 */
//	void setState(String state, boolean triggerEvent)
//	{
//		boolean runExtraCode = !state.equals(currentState);
//		if (runExtraCode && currentState != null)
//		{
//			mStates.get(currentState).runExitCode();
//		}
//		currentState = state;
//		mStates.get(currentState).runAlwaysCode();
//		if (runExtraCode)
//		{
//			mStates.get(currentState).runEntryCode();
//		}
//		if (triggerEvent)
//		{
//			fireChangeEvent();
//		}
//	}
//
//	/**
//	 * Establish a new transition. You might use this method something like
//	 * this:
//	 *
//	 * fsm.addTransition(new FSM.Transition("someEvent", "firstState",
//	 * "secondState") { public void doBeforeTransition() {
//	 * System.out.println("about to transition..."); } public void
//	 * doAfterTransition() { fancyOperation(); } });
//	 */
//	void addTransition(Transition trans)
//	{
//		State st = mStates.get(trans.startState);
//		if (st == null)
//		{
//			throw new NoSuchElementException(
//					"Missing state: " + trans.startState);
//		}
//		st.addTransition(trans);
//	}
//
//	/**
//	 * Add a change listener -- this is a standard java change listener and is
//	 * only used to report changes that have already happened. ChangeEvents are
//	 * only fired AFTER a transition's doAfterTransition is called.
//	 */
//	void addChangeListener(ChangeListener cl)
//	{
//		if (!changeListeners.contains(cl))
//		{
//			changeListeners.add(cl);
//		}
//	}
//
//	/**
//	 * Feed the FSM with the named event. If the current state has a transition
//	 * that responds to the given event, the FSM will performed the transition
//	 * using the following steps, assume start and end states are A and B:
//	 *
//	 * <ol>
//	 * <li>Execute the transition's "doBeforeTransition" method</li>
//	 * <li>Run fsm.setState(B) -- see docs for that method</li>
//	 * <li>Execute the transition's "doAfterTransition" method</li>
//	 * <li>Fire a change event, notifying interested observers that the
//	 * transition has completed.</li>
//	 * <li>Now firmly in state B, see if B has a third state C that we must
//	 * automatically transition to via addEvent(C).</li>
//	 * </ol>
//	 */
//	void addEvent(String evtName)
//	{
//		State state = mStates.get(currentState);
//		if (state.transitions.containsKey(evtName))
//		{
//			Transition trans = state.transitions.get(evtName);
//			// if (debug) {
//			// Debug.out("FSM", "Event: " + evtName + ", " + trans.startState +
//			// " --> " + trans.endState);
//			// }
//			trans.doBeforeTransition();
//			setState(trans.endState, false);
//			trans.doAfterTransition();
//			fireChangeEvent();
//			if (mStates.get(trans.endState).autoTransitionState != null)
//			{
//				// if (debug) {
//				// Debug.out("FSM", "Automatically transitioning from " +
//				// trans.endState + " to "
//				// + states.get(trans.endState).autoTransitionState);
//				// }
//				addEvent("(auto)");
//			}
//		}
//	}
//
//protected:
//	std::string mName;
//	std::string mCurrentState;
//	StateTable mStates;
//	//  protected List<ChangeListener> changeListeners;
//	bool mDebug;
//};
//
///**
// * Fire a change event to registered listeners.
// */
//void fireChangeEvent()
//{
//	ChangeEvent changeEvent = new ChangeEvent(this);
//	for (ChangeListener cl : changeListeners)
//	{
//		cl.stateChanged(changeEvent);
//	}
//}
#include <string>
#include <set>
#include <queue>
#include <utility>
#include <event.h>
#include <throw_event.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include <pointerTo.h>
#include <referenceCount.h>
//#include <typedWritableReferenceCount.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

/**
 * \brief The State type.
 */
typedef std::string State;

/**
 * \brief The State set type.
 */
typedef set<State> StateSet;

/**
 * \brief The transition/filter/fromTo function type.
 */
struct FSMFunction: public ReferenceCount
{
	boost::function<void(void)> mFSMFunction;
};

/**
 * \brief The transition function type.
 */
//typedef void TransitionFunction(void*);
//typedef boost::function<void(void)> TransitionFunction;

/**
 * \brief The filter function type.
 */
//typedef void FilterFunction(void*);
//typedef boost::function<void(void)> FilterFunction;

/**
 * \brief The FromTo function type.
 */
//typedef void FromToFunction(const State&, const State&, void*);
//typedef boost::function<void(const State&, const State&)> FromToFunction;

/**
 * \name Function tables indexed by State(s).
 */
///@{
typedef std::map<State, PT(FSMFunction)> TransitionFunctionTable;
typedef std::map<State, PT(FSMFunction)> FilterFunctionTable;
typedef std::map<std::pair<State, State>, PT(FSMFunction)> FromToFunctionTable;
///@}

/**
 * \brief A Finite State Machine.
 *
 * This is intended to be the class of any number of specific machines,
 * which consist of a collection of states and transitions, and rules
 * to switch between states according to arbitrary input data.
 * The states of an FSM are defined implicitly.
 * Each state is identified by a string, which by convention begins
 * with a capital letter.  (Also by convention, strings passed to
 * request that are not state names should begin with a lowercase letter.).
 * To define specialized behavior when entering or exiting a
 * particular State, define FSMFunction(s) for enter to and/or
 * exit from the State.
 * Both functions can access the previous state name as
 * mOldState, and the new state name we are transitioning to as
 * mNewState.  (Of course, in enter "Red" State, mNewState will
 * always be "Red", and the in exit "Red" State, mOldState will
 * always be "Red".)
 * Both functions are optional.  If either function is omitted, the
 * state is still defined, but nothing is done when transitioning
 * into (or out of) the state.
 * Additionally, you may define a filter State FSMFunction for each
 * state.  The purpose of this function is to decide what state to
 * transition to next, if any, on receipt of a particular input.  The
 * input is always a State and the return value should either FSM::NullState
 * to do nothing, or the name of the state to transition into.
 * As above, the filter State functions are optional.  If any is
 * omitted, the defaultFilter() method is called instead.  A standard
 * implementation of defaultFilter() is provided, which may be
 * overridden in a derived class to change the behavior on an
 * unexpected transition.
 * The current state may be queried at any time other than
 * during the handling of the enter() and exit() functions.  During these
 * functions, current state contains the value FSM::NullState (you are
 * not really in any state during the transition).  However, during a
 * transition you *can* query the outgoing and incoming states,
 * respectively, via oldState() and newState().  At other times,
 * currentState contains the name of the current state.
 * Initially, the FSM is in state FSM::OffState. It does not call
 * enter OffState at construction time; it is simply in OffState already
 * by convention.
 * If you need to call code in enterOff() to initialize your FSM
    properly, call it explicitly in the constructor.  Similarly, when
    cleanup() is called or the FSM is destructed, the FSM transitions
    back to 'Off' by convention.  (It does call enterOff() at this
    point, but does not call exitOff().)

    To implement nested hierarchical FSM's, simply create a nested FSM
    and store it on the class within the appropriate enterState()
    function, and clean it up within the corresponding exitState()
    function.

    There is a way to define specialized transition behavior between
    two particular states.  This is done by defining a from<X>To<Y>()
    function, where X is the old state and Y is the new state.  If this
    is defined, it will be run in place of the exit<X> and enter<Y>
    functions, so if you want that behavior, you'll have to call them
    specifically.  Otherwise, you can completely replace that transition's
    behavior.

    See the code in SampleFSM.py for further examples.
 */
class FSM
{
public:
	/**
	 * \brief The FSM "Null" State.
	 */
	static State NullState;
	/**
	 * \brief The FSM "Off" State.
	 */
	static State OffState;

	/**
	 * \brief Constructor.
	 * @param name The FSM's name.
	 */
	FSM(const std::string& name);

	/**
	 * \brief A convenience function to force the FSM to clean itself up
	 * by transitioning to the "Off" State.
	 */
	void cleanup();

	/**
	 * \name State change broadcasting.
	 *
	 * If setBroadcastStateChanges(true), this event will be sent through
	 * the EventHandler on every state change. The new and old states are
	 * accessible as mOldState and mNewState, and the transition
	 * functions will already have been called.
	 * @param doBroadcast Flag to enable/disable broadcast.
	 */
	///@{
	void setBroadcastStateChanges(bool doBroadcast);
	Event getStateChangeEvent();
	///@}

	FilterFunction* getCurrentFilter();

	/**
	 * \brief Returns the current state if we are in a state now, or the
	 * state we are transitioning into if we are currently within
	 * the enter or exit function for a state.
	 * @return The current or next state.
	 */
	const State& getCurrentOrNextState();

	/**
	 * \brief Returns the current (and the next) state.
	 *
	 * If we are in a state now returns true: currState will contain this
	 * state (nextState is ignored).
	 * If we are performing a transition (if we are currently within
	 * the enter or exit function for a state) returns false: currState and
	 * toState will contain the "from-state" and the "to-state" of the transition.
	 * @param currState The current or the transition from-state (out parameter).
	 * @param toState Ignored or the transition to-state (out parameter).
	 * @return
	 */
	bool getCurrentStateOrTransition(State& currState, State& toState);

	bool isInTransition();

	/**
	 * \brief Changes unconditionally to the indicated state.
	 *
	 * This bypasses the filterState() function, and just calls
	 * exit State function followed by enterState().
	 * @param state The destination state.
	 */
	void forceTransition(const State& state);

	/**
	 * \brief Requests a state transition, by code that does not expect
	 * the request to be denied.
	 *
	 * If the request is denied, raises a RequestDenied exception.
	 * Unlike request(), this method allows a new request to be made
	 * while the FSM is currently in transition.  In this case, the
	 * request is queued up and will be executed when the current
	 * transition finishes.  Multiple requests will queue up in sequence.
	 * @param state The destination state.
	 */
	void demand(const State& state);

	/**
	 * \brief Requests a state transition (or other behavior).
	 *
	 * The request may be denied by the FSM's filter function.  If it is
	 * denied, the filter function may either raise an exception
	 * (RequestDenied), or it may simply return None, without
	 * changing the FSM's state.
	 * The request parameter should be a string.  The request, along
	 * with any additional arguments, is passed to the current
	 * filterState() function.  If filterState() returns a string,
	 * the FSM transitions to that state.
	 * The return value is the same as the return value of
	 * filter State function (that is, None if the request does not provoke a
	 * state transition, otherwise it is a tuple containing the name
	 * of the state followed by any optional args.)
	 * If the FSM is currently in transition (i.e. in the middle of
	 * executing an enterState or exit State function), an
	 * AlreadyInTransition exception is raised (but see demand(),
	 * which will queue these requests up and apply when the
	 * transition is complete).
	 * @param state The destination state.
	 */
	void request(const State& state);

	/**
	 * \brief Set of unique states to iterate through
	 * @param stateArray The state set.
	 */
	void setStateArray(const StateSet& states);

	/**
	 * \brief Request the 'next' state in the predefined state array.
	 *
	 * \note The elements in a std::set are always sorted from lower to
	 * higher following a specific strict weak ordering criterion set
	 * on container construction
	 */
	void requestNext();

	/**
	 * \brief Request the 'previous' state in the predefined state array.
	 *
	 * \note The elements in a std::set are always sorted from lower to
	 * higher following a specific strict weak ordering criterion set
	 * on container construction
	 */
	void requestPrev();

	bool addState(const State& state, TransitionFunction* enterFunc = NULL,
			TransitionFunction* exitFunc = NULL);
	bool removeState(const State& state);
	bool addFilter(const State& state, FilterFunction* filterFunc);
	bool removeFilter(const State& state);
	bool addFromToFunction(const State& stateFrom, const State& stateTo,
			FromToFunction* fromToFunc);
	bool removeFromToFunction(const State& stateFrom, const State& stateTo);

protected:
	///The serial number of this fsm.
	static int mSerialNum;
	/**
	 * \brief Return an serial number for this fsm.
	 * @return
	 */
	int getSerialNum();
	///The name of this fsm.
	std::string mName;
	///The state set.
	StateSet mStates;
	///The enter/exit functions' tables.
	TransitionFunctionTable mEnterFunctions, mExitFunctions;
	///The filter functions' table.
	FilterFunctionTable mFilterFunctions;
	///The FromTo functions' table.
	FromToFunctionTable mFromToFunctions;

	///@{
	///The current/old/new states.
	///Initially, we are in the Off state by convention.
	///During a transition mState is set in the "Null" State.
	State mState, mOldState, mNewState;
	///@}

	///Flag for notifying on every state change.
	bool mBroadcastStateChanges;

	///@{
	///This member records transition requests made by demand() or
	///forceTransition() while the FSM is in transition between
	///states.
	typedef boost::function<void(const State&)> QueuedFunction;
	std::queue<QueuedFunction> mRequestQueue;
	///@}

	/**
	 * \brief This is the default function that is called if there is no
	 * enter State function for a particular state name.
	 */
	virtual void defaultEnter();

	/**
	 * \brief This is the default function that is called if there is no
	 * exit State function for a particular state name.
	 */
	virtual void defaultExit();

	/**
	 * \brief This is the function that is called if there is no
	 * filter State function for a particular state name.
	 *
	 * This default filter function behaves in one of two modes:
	 * \li \c if self.defaultTransitions is None, allow any request
	 * whose name begins with a capital letter, which is assumed to
	 * be a direct request to a particular state.  This is similar to
	 * the old ClassicFSM onUndefTransition=ALLOW, with no explicit
	 * state transitions listed.
	 * \li \c if self.defaultTransitions is not None, allow only those
	 * requests explicitly identified in this map.  This is similar
	 * to the old ClassicFSM onUndefTransition=DISALLOW, with an
	 * explicit list of allowed state transitions.
	 * Specialized FSM's may wish to redefine this default filter
	 * (for instance, to always return the request itself, thus
	 * allowing any transition.).
	 * @param state The destination state.
	 */
	virtual void defaultFilter(const State& state);

	/**
	 * \brief From the off state, we can always go directly to any other
	 * state.
	 *
	 * @param state The destination state.
	 */
	void filterOff(const State& state);

	/**
	 * \brief Internal function to change unconditionally to the indicated
	 * state.
	 *
	 * @param state The destination state.
	 */
	void setState(const State& state);

	///The (reentrant) mutex associated with this fsm.
	ReMutex mMutex;
};

////Bind the Model and the Animation
//// don't use PT or CPT with AnimControlCollection
//AnimControlCollection anim_collection;
//AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data);
//AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);
//
//int main(int argc, char **argv)
//{
//	///setup
//	// Load your configuration
//	load_prc_file("config.prc");
//	PandaFramework panda = PandaFramework();
//	panda.open_framework(argc, argv);
//	panda.set_window_title("animation training");
//	WindowFramework* window = panda.open_window();
//	if (window != (WindowFramework *) NULL)
//	{
//		std::cout << "Opened the window successfully!\n";
//		// common setup
//		window->enable_keyboard(); // Enable keyboard detection
//		window->setup_trackball(); // Enable default camera movement
//	}
//	//physics
//	PT(BulletWorld) physicsWorld = new BulletWorld();
//	//physics: advance the simulation state
////	AsyncTask* task = new GenericAsyncTask("update physics", &update_physics,
////			reinterpret_cast<void*>(&actualAnim));
////	task->set_delay(3);
////	panda.get_task_mgr().add(task);
//
//	//Load the Actor Model
//	NodePath Actor = window->load_model(window->get_render(),
//			"bvw-f2004--airbladepilot/pilot-model");
//	PT(Character) character =
//			DCAST(Character, Actor.find("**/+Character").node());
//	PT(PartBundle) pbundle = character->get_bundle(0);
//	//Load Animations
//	std::vector<std::string> animations;
//	animations.push_back(std::string("pilot-chargeshoot"));
//	animations.push_back(std::string("pilot-discloop"));
//	for (unsigned int i = 0; i < animations.size(); ++i)
//	{
//		window->load_model(Actor, "bvw-f2004--airbladepilot/" + animations[i]);
//	}
//	auto_bind(Actor.node(), anim_collection);
//	pbundle->set_anim_blend_flag(true);
//	pbundle->set_control_effect(anim_collection.get_anim(0), 0.5);
//	pbundle->set_control_effect(anim_collection.get_anim(1), 0.5);
//	int actualAnim = 0;
//	//switch among animations
//	AsyncTask* task = new GenericAsyncTask("check playing", &check_playing,
//			reinterpret_cast<void*>(&actualAnim));
//	task->set_delay(3);
//	panda.get_task_mgr().add(task);
//	//attach to scene
//	Actor.reparent_to(window->get_render());
//	Actor.set_pos(0.0, 100.0, -30.0);
//
//	// Do the main loop
//	panda.main_loop();
//	// close the framework
//	panda.close_framework();
//	return 0;
//}
//
//AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data)
//{
//	//Control the Animations
//	double time = ClockObject::get_global_clock()->get_real_time();
//	int *actualAnim = reinterpret_cast<int*>(data);
//	int num = *actualAnim % 3;
//	if (num == 0)
//	{
//		std::cout << time << " - Blending" << std::endl;
//		if (not anim_collection.get_anim(0)->is_playing())
//		{
//			anim_collection.get_anim(0)->play();
//		}
//		if (not anim_collection.get_anim(1)->is_playing())
//		{
//			anim_collection.get_anim(1)->play();
//		}
//	}
//	else if (num == 1)
//	{
//		std::cout << time << " - Playing: " << anim_collection.get_anim_name(0)
//				<< std::endl;
//		if (not anim_collection.get_anim(0)->is_playing())
//		{
//			anim_collection.get_anim(0)->play();
//		}
//		if (anim_collection.get_anim(1)->is_playing())
//		{
//			anim_collection.get_anim(1)->stop();
//		}
//	}
//	else
//	{
//		std::cout << time << " - Playing: " << anim_collection.get_anim_name(1)
//				<< std::endl;
//		anim_collection.get_anim(1)->play();
//		if (anim_collection.get_anim(0)->is_playing())
//		{
//			anim_collection.get_anim(0)->stop();
//		}
//		if (not anim_collection.get_anim(1)->is_playing())
//		{
//			anim_collection.get_anim(1)->play();
//		}
//	}
//	*actualAnim += 1;
//	return AsyncTask::DS_again;
//}
//
////////////////////////////////////////////////////////////////////////
State FSM::NullState = std::string("NullState");
State FSM::OffState = std::string("OffState");

inline FSM::FSM(const std::string& name) :
		mName(name), mState("Off"), mBroadcastStateChanges(false)
{
}

inline int FSM::getSerialNum()
{
	//lock (guard) the mutex
	ReMutexHolder guard(mMutex);

	return ++mSerialNum;
}

