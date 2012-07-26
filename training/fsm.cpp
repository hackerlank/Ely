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

#include <string>
#include <sstream>
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

#include "Utilities/Tools.h"

class FSM;

/**
 * \brief The State type.
 */
typedef std::string State;

/**
 * \brief The State set type.
 */
typedef set<State> StateSet;

/**
 * \brief The transition/fromTo function type.
 */
typedef boost::function<void(FSM*)> TransitionFunctor;
class TransitionFunction: public ReferenceCount
{
public:
	TransitionFunction(FSM* fsm, TransitionFunctor functor)
	{
		mFsm = fsm;
		mFunctor = functor;
	}
	void operator()()
	{
		mFunctor(mFsm);
	}
private:
	FSM* mFsm;
	TransitionFunctor mFunctor;
};

/**
 * \brief The filter function type.
 */
typedef boost::function<const State&(FSM*, const std::string&)> FilterFunctor;
class FilterFunction: public ReferenceCount
{
public:
	FilterFunction(FSM* fsm, const std::string& str, FilterFunctor functor)
	{
		mFsm = fsm;
		mFunctor = functor;
	}
	const State& operator()(const std::string& state)
	{
		return mFunctor(mFsm, state);
	}
private:
	FSM* mFsm;
	FilterFunctor mFunctor;
};

/**
 * \brief The (deferred) FSM method type.
 */
typedef boost::function<void(void)> FSMFunctor;
class FSMFunction: public ReferenceCount
{
public:
	FSMFunction(FSMFunctor functor)
	{
		mFunctor = functor;
	}
	void operator()()
	{
		mFunctor();
	}
private:
	FSMFunctor mFunctor;
};

///@{
typedef std::map<State, PT(TransitionFunction)> TransitionFunctionTable;
typedef std::map<State, PT(FilterFunction)> FilterFunctionTable;
typedef std::map<std::pair<State, State>, PT(TransitionFunction)> FromToFunctionTable;
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
 * particular State, define StateFunction(s) for enter to and/or
 * exit from the State.
 * Both functions can access the previous state name as
 * mOldState, and the new state name we are transitioning to as
 * mNewState.  (Of course, in enter "Red" State, mNewState will
 * always be "Red", and the in exit "Red" State, mOldState will
 * always be "Red".)
 * Both functions are optional.  If either function is omitted, the
 * state is still defined, but nothing is done when transitioning
 * into (or out of) the state.
 * Additionally, you may define a filter State StateFunction for each
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
 * functions, current state contains the value NullState (you are
 * not really in any state during the transition).  However, during a
 * transition you *can* query the outgoing and incoming states,
 * respectively, via oldState() and newState().  At other times,
 * currentState contains the name of the current state.
 * Initially, the FSM is in state OffState. It does not call
 * enter OffState at construction time; it is simply in OffState already
 * by convention.
 * If you need to call code in enter OffState to initialize your FSM
 * properly, call it explicitly in the constructor.  Similarly, when
 * cleanup() is called or the FSM is destructed, the FSM transitions
 * back to OffState by convention.  (It does call enter OffState at this
 * point, but does not call exit OffState.)
 * To implement nested hierarchical FSM's, simply create a nested FSM
 * and store it on the class within the appropriate enter State
 * function, and clean it up within the corresponding exit State
 * function.
 * There is a way to define specialized transition behavior between
 * two particular states.  This is done by defining a from X To Y
 * function, where X is the old state and Y is the new state.  If this
 * is defined, it will be run in place of the exit X and enter Y
 * functions, so if you want that behavior, you'll have to call them
 * specifically.  Otherwise, you can completely replace that transition's
 * behavior.
 *
 */
class FSM
{
protected:
	//Protected Interface
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
	virtual void defaultFilter(void);

	/**
	 * \brief From the off state, we can always go directly to any other
	 * state.
	 *
	 * @param state The destination state.
	 */
	void filterOffState(void);

	/**
	 * \brief Internal function to change unconditionally to the indicated
	 * state.
	 *
	 * @param state The destination state.
	 */
	void setState(const State& state);
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

public:
	//Public Interface
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
	 * by transitioning to the OffState.
	 */
	void cleanup();

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
	 * @return True if request is accepted, false otherwise.
	 */
	bool request(const State& state);

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

	/**
	 * \name FSM construction functions.
	 *
	 * The return values indicate if the actions ware successful or not.
	 */
	///@{
	bool addState(const State& state, StateFunction* enterFunc = NULL,
			StateFunction* exitFunc = NULL);
	bool removeState(const State& state);
	bool addEnterFunc(const State& state, StateFunction* filterFunc);
	bool removeEnterFunc(const State& state);
	bool addExitFunc(const State& state, StateFunction* filterFunc);
	bool removeExitFunc(const State& state);
	bool addFilter(const State& state, StateFunction* filterFunc);
	bool removeFilterFunc(const State& state);
	bool addFromToFunc(const State& stateFrom, const State& stateTo,
			StateFunction* fromToFunc);
	bool removeFromToFunc(const State& stateFrom, const State& stateTo);
	///@}

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

	///This member records transition requests made by demand() or
	///forceTransition() while the FSM is in transition between
	///states.
	std::queue<PT(FSMFunction)> mRequestQueue;

	///The (reentrant) mutex associated with this fsm.
	ReMutex mMutex;
};
////////////////////////////////////////////////////////////////////////
State FSM::NullState = std::string("NullState");
State FSM::OffState = std::string("OffState");

FSM::FSM(const std::string& name)
{
	//reset all data
	mEnterFunctions.clear();
	mExitFunctions.clear();
	mFilterFunctions.clear();
	mFromToFunctions.clear();
	mStates.clear();
	//set name
	mName = name;
	//set serial number
	mSerialNum = FSM::getSerialNum();
	//Set up the NullState
	addState(NullState, NULL, NULL);
	addFilter(NullState, NULL);
	//Set up the OffState
	addState(OffState, NULL, NULL);
	StateFunction* filterOff = new StateFunction(this,
			StateFunctor(boost::bind(&FSM::filterOffState, this)));
	addFilter(OffState, filterOff);
	mState = OffState;
	mOldState = NullState;
	mNewState = NullState;
	//set broadcast state chage
	mBroadcastStateChanges = false;
	//
}

int FSM::getSerialNum()
{
	//lock (guard) the mutex
	ReMutexHolder guard(mMutex);

	return ++SerialNum;
}

void FSM::cleanup()
{
	ReMutexHolder guard(mMutex);

	if (mState != OffState)
	{
		setState(OffState);
	}
}

void FSM::setBroadcastStateChanges(bool doBroadcast)
{
	mBroadcastStateChanges = doBroadcast;
}

Event FSM::getStateChangeEvent()
{
	std::ostringstream stateChange;
	stateChange << "FSM-" << mSerialNum << "-" << mName << "-stateChange";
	return Event(stateChange.str());
}

StateFunction* FSM::getCurrentFilter()
{
	return mFilterFunctions[mState];
}

const State& FSM::getCurrentOrNextState()
{
	ReMutexHolder guard(mMutex);

	if (mState != NullState)
	{
		return mState;
	}
	return mNewState;
}

bool FSM::getCurrentStateOrTransition(State& currState, State& toState)
{
	ReMutexHolder guard(mMutex);

	if (mState != NullState)
	{
		currState = mState;
		return true;
	}
	currState = mOldState;
	toState = mNewState;
	return false;
}

bool FSM::isInTransition()
{
	ReMutexHolder guard(mMutex);

	return mState == NullState;
}

void FSM::forceTransition(const State& state)
{
	ReMutexHolder guard(mMutex);

	if (mState == NullState)
	{
		// Queue up the request.
		FSMFunctor fsmFunctor = boost::bind(&FSM::forceTransition,
				boost::ref(*this), state);
		FSMFunction* forceTransFunc = new FSMFunction(fsmFunctor);
		mRequestQueue.push(forceTransFunc);
		return;
	}
	setState(state);
}

void FSM::demand(const State& state)
{
	ReMutexHolder guard(mMutex);

	if (mState == NullState)
	{
		// Queue up the request.
		FSMFunctor fsmFunctor = boost::bind(&FSM::demand, boost::ref(*this),
				state);
		FSMFunction* demandFunc = new FSMFunction(fsmFunctor);
		mRequestQueue.push(demandFunc);
		return;
	}
	//
	if (request(state) == NullState)
	{
		throw GameException(
				"FSM::demand: Request denied from'" + mState + "' to '" + state
						+ "'");
	}
}

bool FSM::request(const State& state)
{
	ReMutexHolder guard(mMutex);


}

void FSM::setStateArray(const StateSet& states)
{
}

void FSM::requestNext()
{
}

void FSM::requestPrev()
{
}

bool FSM::addState(const State& state, StateFunction* enterFunc,
		StateFunction* exitFunc)
{
}

bool FSM::removeState(const State& state)
{
}

bool FSM::addEnterFunc(const State& state, StateFunction* filterFunc)
{
}

bool FSM::removeEnterFunc(const State& state)
{
}

bool FSM::addExitFunc(const State& state, StateFunction* filterFunc)
{
}

bool FSM::removeExitFunc(const State& state)
{
}

bool FSM::addFilter(const State& state, StateFunction* filterFunc)
{
}

bool FSM::removeFilterFunc(const State& state)
{
}

bool FSM::addFromToFunc(const State& stateFrom, const State& stateTo,
		StateFunction* fromToFunc)
{
}

bool FSM::removeFromToFunc(const State& stateFrom, const State& stateTo)
{
}

int FSM::getSerialNum()
{
}

void FSM::defaultEnter()
{
}

void FSM::defaultExit()
{
}

void FSM::defaultFilter(void)
{
}

void FSM::filterOffState(void)
{
}

void FSM::setState(const State& state)
{
}

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

