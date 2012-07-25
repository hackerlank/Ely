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

#include <iostream>
#include <vector>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <character.h>
#include <animControlCollection.h>
#include <pandaFramework.h>

#include <panda3d/bulletWorld.h>

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
#include <event.h>
#include <throw_event.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include <pointerTo.h>
#include <typedWritableReferenceCount.h>

/**
 * \brief The State type.
 */
typedef std::string State;
/**
 * \brief The State set type.
 */
typedef set<State> StateSet;
/**
 * \brief The transition function type.
 */
typedef void TransitionFunction(void*);
/**
 * \brief The filter function type.
 */
typedef void FilterFunction(const State&, void*);
/**
 * \name Function tables indexed by State.
 */
///@{
typedef std::map<State,TransitionFunction> TransitionFunctionTable;
typedef std::map<State,FilterFunction> FilterFunctionTable;
///@}

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

	void forceTransition(const State& state, void* data);


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
	///@{
	///The current/old/new states.
	///Initially, we are in the Off state by convention.
	///During a transition mState is set in the "Null" State.
	State mState, mOldState, mNewState;
	///@}
	///Flag for notifying on every state change.
	bool mBroadcastStateChanges;
	///This member records transition requests made by demand() or
	///forceTransition() while the FSM is in transition between
	///states.
	std::queue<State> mRequestQueue;

	///The (reentrant) mutex associated with this fsm.
	ReMutex mMutex;
};

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection anim_collection;
AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data);
AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);

int main(int argc, char **argv)
{
	///setup
	// Load your configuration
	load_prc_file("config.prc");
	PandaFramework panda = PandaFramework();
	panda.open_framework(argc, argv);
	panda.set_window_title("animation training");
	WindowFramework* window = panda.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}
	//physics
	PT(BulletWorld) physicsWorld = new BulletWorld();
	//physics: advance the simulation state
//	AsyncTask* task = new GenericAsyncTask("update physics", &update_physics,
//			reinterpret_cast<void*>(&actualAnim));
//	task->set_delay(3);
//	panda.get_task_mgr().add(task);

	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			"bvw-f2004--airbladepilot/pilot-model");
	PT(Character) character =
			DCAST(Character, Actor.find("**/+Character").node());
	PT(PartBundle) pbundle = character->get_bundle(0);
	//Load Animations
	std::vector<std::string> animations;
	animations.push_back(std::string("pilot-chargeshoot"));
	animations.push_back(std::string("pilot-discloop"));
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, "bvw-f2004--airbladepilot/" + animations[i]);
	}
	auto_bind(Actor.node(), anim_collection);
	pbundle->set_anim_blend_flag(true);
	pbundle->set_control_effect(anim_collection.get_anim(0), 0.5);
	pbundle->set_control_effect(anim_collection.get_anim(1), 0.5);
	int actualAnim = 0;
	//switch among animations
	AsyncTask* task = new GenericAsyncTask("check playing", &check_playing,
			reinterpret_cast<void*>(&actualAnim));
	task->set_delay(3);
	panda.get_task_mgr().add(task);
	//attach to scene
	Actor.reparent_to(window->get_render());
	Actor.set_pos(0.0, 100.0, -30.0);

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data)
{
	//Control the Animations
	double time = ClockObject::get_global_clock()->get_real_time();
	int *actualAnim = reinterpret_cast<int*>(data);
	int num = *actualAnim % 3;
	if (num == 0)
	{
		std::cout << time << " - Blending" << std::endl;
		if (not anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->play();
		}
		if (not anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->play();
		}
	}
	else if (num == 1)
	{
		std::cout << time << " - Playing: " << anim_collection.get_anim_name(0)
				<< std::endl;
		if (not anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->play();
		}
		if (anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->stop();
		}
	}
	else
	{
		std::cout << time << " - Playing: " << anim_collection.get_anim_name(1)
				<< std::endl;
		anim_collection.get_anim(1)->play();
		if (anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->stop();
		}
		if (not anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->play();
		}
	}
	*actualAnim += 1;
	return AsyncTask::DS_again;
}

//////////////////////////////////////////////////////////////////////
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

