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
#include <list>
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
#include <boost/any.hpp>

#include "Utilities/Tools.h"

/**
 * \brief The Value list type.
 */
typedef std::list<boost::any> ValueList;

class FSM;

/**
 * \name The Enter/Exit/FromTo boost::function types.
 */
///@{
typedef boost::function<void(FSM*, const ValueList&)> EnterFuncPTR;
typedef boost::function<void(FSM*)> ExitFuncPTR;
typedef boost::function<void(FSM*, const ValueList&)> FromToFuncPTR;
///@}

/**
 * \brief The Filter boost::function type.
 */
typedef boost::function<ValueList(FSM*, const ValueList&)> FilterFuncPTR;

/**
 * \brief The StateTmpl type.
 *
 * It can be used where a T is expected and viceversa.
 */
template<typename T> struct StateTmpl
{
	/**
	 * \brief Default Constructor.
	 */
	StateTmpl()
	{
	}
	/**
	 * \brief Copy Constructor.
	 */
	StateTmpl(const StateTmpl& other) :
			name(other.name), enter(other.enter), exit(other.exit), filter(
					other.filter)
	{
	}
	/**
	 * \brief Default Destructor.
	 */
	~StateTmpl()
	{
	}
	/**
	 * \brief Converting Constructor.
	 */
	StateTmpl(const T& _name, EnterFuncPTR _enter = EnterFuncPTR(),
			ExitFuncPTR _exit = ExitFuncPTR(), FilterFuncPTR _filter =
					FilterFuncPTR()) :
			name(_name), enter(_enter), exit(_exit), filter(_filter)
	{
	}
	/**
	 * \brief Assignment operator.
	 */
	StateTmpl& operator=(const StateTmpl& other)
	{
		name = other.name;
		enter = other.enter;
		exit = other.exit;
		filter = other.filter;
		return *this;
	}

	/**
	 * \name Strict orderings.
	 */
	///@{
	/// Wrt StateTmpl
	bool operator<(const StateTmpl& other) const
	{
		return name < other.name;
	}
	///Wrt std::string
	bool operator<(const std::string& otherName) const
	{
		return name < otherName;
	}
	///@}
	/**
	 * \name Equality operators.
	 */
	///@{
	///Wrt StateTmpl
	bool operator==(const StateTmpl& other) const
	{
		return name == other.name;
	}
	bool operator!=(const StateTmpl& other) const
	{
		return name != other.name;
	}
	///Wrt std::string
	bool operator==(const std::string& otherName) const
	{
		return name == otherName;
	}
	bool operator!=(const std::string& otherName) const
	{
		return name != otherName;
	}
	///@}
	///Converting function to std::string.
	operator std::string() const
	{
		return name;
	}

	/**
	 * \name Members.
	 */
	///@{
	T name;
	EnterFuncPTR enter;
	ExitFuncPTR exit;
	FilterFuncPTR filter;
	///@}
};

/**
 * \name Global operators: allow T @op@ StateTmpl comparison
 */
///@{
template<typename T> bool operator==(const string& lhs, const StateTmpl<T>& rhs)
{
	return lhs == rhs.name;
}
template<typename T> bool operator<(const string& lhs, const StateTmpl<T>& rhs)
{
	return lhs < rhs.name;
}
///@}

/**
 * \name Common typedefs.
 */
///@{
typedef StateTmpl<std::string> State;
typedef std::set<State> StateSet;
typedef std::map<std::pair<State, State>, FromToFuncPTR> FromToFunctionTable;
///@}

/**
 * \brief A Finite State Machine.
 *
 * This is intended to be the class of any number of specific machines,
 * which consist of a collection of states and transitions, and rules
 * to switch between states according to arbitrary input data.
 * The states of an FSM are defined implicitly.
 * Each state is identified by a string.
 * Also strings are passed as request that are not state names but
 * arbitrary input.
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
	static void defaultEnter(FSM* fsm, const ValueList& data);

	/**
	 * \brief This is the default function that is called if there is no
	 * exit State function for a particular state name.
	 */
	static void defaultExit(FSM* fsm);

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
	static ValueList defaultFilter(FSM* fsm, const ValueList& data);

	/**
	 * \brief From the off state, we can always go directly to any other
	 * state.
	 *
	 * @param state The destination state.
	 */
	static ValueList filterOff(FSM* fsm, const ValueList& data);

	/**
	 * \brief Internal function to change unconditionally to the indicated
	 * state.
	 *
	 * @param state The destination state.
	 */
	void setState(const State& newState, const ValueList& data);
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
	 * \brief The FSM method boost::function type.
	 */
	typedef boost::function<void(const State&, const ValueList&)> FSMMethodPTR;

	/**
	 * \brief The FSM "Null" State.
	 */
	static State Null;

	/**
	 * \brief The FSM "Off" State.
	 */
	static State Off;

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

	FilterFuncPTR getCurrentFilter();

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
	void forceTransition(const State& req, const ValueList& data);

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
	void demand(const State& req, const ValueList& data);

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
	const State& request(const State& req, const ValueList& data);

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
	const State& requestNext(const ValueList& data);

	/**
	 * \brief Request the 'previous' state in the predefined state array.
	 *
	 * \note The elements in a std::set are always sorted from lower to
	 * higher following a specific strict weak ordering criterion set
	 * on container construction
	 */
	const State& requestPrev(const ValueList& data);

	/**
	 * \name FSM construction functions.
	 *
	 * The return values indicate if the actions ware successful or not.
	 */
	///@{
	bool addState(const State& state);
	bool removeState(const State& state);
	bool addEnterFunc(const State& state, const EnterFuncPTR& enterFunc);
	bool removeEnterFunc(const State& state);
	bool addExitFunc(const State& state, const ExitFuncPTR& exitFunc);
	bool removeExitFunc(const State& state);
	bool addFilter(const State& state, const FilterFuncPTR& filterFunc);
	bool removeFilterFunc(const State& state);
	bool addFromToFunc(const State& stateFrom, const State& stateTo,
			const FromToFuncPTR& fromToFunc);
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
	StateSet mStateSet;
	///The FromToFunction table
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
	std::queue<FSMMethodPTR> mRequestQueue;

	///The (reentrant) mutex associated with this FSM.
	ReMutex mMutex;
};
////////////////////////////////////////////////////////////////////////
State FSM::Null = State("Null");
State FSM::Off = State("Off", NULL, NULL, FilterFuncPTR(&FSM::filterOff));

FSM::FSM(const std::string& name)
{
	//reset all data
	mStateSet.clear();
	mFromToFunctions.clear();
	//set name
	mName = name;
	//set serial number
	mSerialNum = FSM::getSerialNum();
	//Set initial states
	mState = FSM::Off;
	mOldState = FSM::Null;
	mNewState = FSM::Null;
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

	if (mState != FSM::Off)
	{
		setState (OffState);
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

FilterFuncPTR FSM::getCurrentFilter()
{
	if (mState == FSM::Null)
	{
		throw GameException(
				"FSM::getCurrentFilter: FSM cannot determine current filter while in transition ('"
						+ mOldState + "' -> '" + mNewState + "').");

	}
	return mState.filter;
}

const State& FSM::getCurrentOrNextState()
{
	ReMutexHolder guard(mMutex);

	if (mState != FSM::Null)
	{
		return mState;
	}
	return mNewState;
}

bool FSM::getCurrentStateOrTransition(State& currState, State& toState)
{
	ReMutexHolder guard(mMutex);

	if (mState != FSM::Null)
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

	return mState == FSM::Null;
}

void FSM::forceTransition(const std::string& req, void* data)
{
	ReMutexHolder guard(mMutex);

	if (mState == FSM::Null)
	{
		// Queue up the request for later calling.
		FSMMethodPTR forceTransitionPTR = boost::bind(&FSM::forceTransition,
				boost::ref(*this), req, data);
		mRequestQueue.push(forceTransitionPTR);
		return;
	}
	setState(State(req));
}

void FSM::demand(const std::string& req, void* data)
{
	ReMutexHolder guard(mMutex);

	if (mState == NullState)
	{
		// Queue up the request for later calling.
		FSMMethodPTR demandPTR = boost::bind(&FSM::demand, boost::ref(*this),
				req, data);
		mRequestQueue.push(demandPTR);
		return;
	}
	//
	if (request(req, data) == NullState)
	{
		throw GameException(
				"FSM::demand: Request denied from'" + mState + "' to '" + req
						+ "'");
	}
}

const State& FSM::request(const std::string& req, void* data)
{
//    self.fsmLock.acquire()
//    try:
//        assert isinstance(request, types.StringTypes)
//        self.notify.debug("%s.request(%s, %s" % (
//            self.name, request, str(args)[1:]))
//
//        filter = self.getCurrentFilter()
//        result = filter(request, args)
//        if result:
//            if isinstance(result, types.StringTypes):
//                # If the return val1ue is a string, it's just the name
//                # of the state.  Wrap it in a tuple for consistency.
//                result = (result,) + args
//
//            # Otherwise, assume it's a (name, *args) tuple
//            self.__setState(*result)
//
//        return result
//    finally:
//        self.fsmLock.release()

	ReMutexHolder guard(mMutex);

	FilterFuncPTR filter = getCurrentFilter();
	State result = filter(req, data);
	if (mStateSet.find(result) != mStateSet.end())
	{
		//the return value is a string, it's just the name
		//                # of the state.
	}
}

void FSM::setStateArray(const StateSet& states)
{
}

const State& FSM::requestNext(void* data)
{
}

const State& FSM::requestPrev(void* data)
{
}

void FSM::defaultEnter(FSM* fsm, void* data)
{
}

void FSM::defaultExit(FSM* fsm)
{
}

void FSM::defaultFilter(FSM* fsm, const std::string& req, void* data)
{
}

void FSM::filterOff(FSM* fsm, const std::string& req, void* data)
{
}

void FSM::setState(const State& state, void* data)
{
}

//
//////Bind the Model and the Animation
////// don't use PT or CPT with AnimControlCollection
////AnimControlCollection anim_collection;
////AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data);
////AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);
////
////int main(int argc, char **argv)
////{
////	///setup
////	// Load your configuration
////	load_prc_file("config.prc");
////	PandaFramework panda = PandaFramework();
////	panda.open_framework(argc, argv);
////	panda.set_window_title("animation training");
////	WindowFramework* window = panda.open_window();
////	if (window != (WindowFramework *) NULL)
////	{
////		std::cout << "Opened the window successfully!\n";
////		// common setup
////		window->enable_keyboard(); // Enable keyboard detection
////		window->setup_trackball(); // Enable default camera movement
////	}
////	//physics
////	PT(BulletWorld) physicsWorld = new BulletWorld();
////	//physics: advance the simulation state
//////	AsyncTask* task = new GenericAsyncTask("update physics", &update_physics,
//////			reinterpret_cast<void*>(&actualAnim));
//////	task->set_delay(3);
//////	panda.get_task_mgr().add(task);
////
////	//Load the Actor Model
////	NodePath Actor = window->load_model(window->get_render(),
////			"bvw-f2004--airbladepilot/pilot-model");
////	PT(Character) character =
////			DCAST(Character, Actor.find("**/+Character").node());
////	PT(PartBundle) pbundle = character->get_bundle(0);
////	//Load Animations
////	std::vector<std::string> animations;
////	animations.push_back(std::string("pilot-chargeshoot"));
////	animations.push_back(std::string("pilot-discloop"));
////	for (unsigned int i = 0; i < animations.size(); ++i)
////	{
////		window->load_model(Actor, "bvw-f2004--airbladepilot/" + animations[i]);
////	}
////	auto_bind(Actor.node(), anim_collection);
////	pbundle->set_anim_blend_flag(true);
////	pbundle->set_control_effect(anim_collection.get_anim(0), 0.5);
////	pbundle->set_control_effect(anim_collection.get_anim(1), 0.5);
////	int actualAnim = 0;
////	//switch among animations
////	AsyncTask* task = new GenericAsyncTask("check playing", &check_playing,
////			reinterpret_cast<void*>(&actualAnim));
////	task->set_delay(3);
////	panda.get_task_mgr().add(task);
////	//attach to scene
////	Actor.reparent_to(window->get_render());
////	Actor.set_pos(0.0, 100.0, -30.0);
////
////	// Do the main loop
////	panda.main_loop();
////	// close the framework
////	panda.close_framework();
////	return 0;
////}
////
////AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data)
////{
////	//Control the Animations
////	double time = ClockObject::get_global_clock()->get_real_time();
////	int *actualAnim = reinterpret_cast<int*>(data);
////	int num = *actualAnim % 3;
////	if (num == 0)
////	{
////		std::cout << time << " - Blending" << std::endl;
////		if (not anim_collection.get_anim(0)->is_playing())
////		{
////			anim_collection.get_anim(0)->play();
////		}
////		if (not anim_collection.get_anim(1)->is_playing())
////		{
////			anim_collection.get_anim(1)->play();
////		}
////	}
////	else if (num == 1)
////	{
////		std::cout << time << " - Playing: " << anim_collection.get_anim_name(0)
////				<< std::endl;
////		if (not anim_collection.get_anim(0)->is_playing())
////		{
////			anim_collection.get_anim(0)->play();
////		}
////		if (anim_collection.get_anim(1)->is_playing())
////		{
////			anim_collection.get_anim(1)->stop();
////		}
////	}
////	else
////	{
////		std::cout << time << " - Playing: " << anim_collection.get_anim_name(1)
////				<< std::endl;
////		anim_collection.get_anim(1)->play();
////		if (anim_collection.get_anim(0)->is_playing())
////		{
////			anim_collection.get_anim(0)->stop();
////		}
////		if (not anim_collection.get_anim(1)->is_playing())
////		{
////			anim_collection.get_anim(1)->play();
////		}
////	}
////	*actualAnim += 1;
////	return AsyncTask::DS_again;
////}
////

void f1(const State& sta)
{
	std::cout << (std::string) sta << std::endl;
}
void f2(const std::string& str)
{
	std::cout << str << std::endl;
}

int main(int argc, char **argv)
{
	State sa("a"), sb("b"), sc("a");
	std::string stra = "a", strb = "b", strc = "a";
	//comparison
	std::cout << (stra == sc) << std::endl;
	std::cout << (sb == strc) << std::endl;

	f1(stra);
	f2(sa);

	return 0;
}

