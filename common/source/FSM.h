/**
 * \file FSM.h
 *
 * \date 2016-10-09
 * \author consultit
 */
#ifndef FSM_H_
#define FSM_H_

#include "commonTools.h"
#include <queue>
#include <list>
#include <throw_event.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>

/**
 * \brief The any value list type.
 */
typedef list<boost::any> AnyValueList;

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
 * If any is omitted, the default_filter() method is called instead.
 * A standard implementation of default_filter() is provided.\n
 * The current state may be queried at any time other than
 * during the handling of the enter, exit, fromTo functions. During these
 * functions, current state contains the value InTransition (you are
 * not really in any state during the transition).\n
 * However, during a transition you *can* query the outgoing and incoming
 * states via get_current_state_or_transition() or get_current_or_next_state().
 * At other times, you *can* query the current State via
 * get_current_or_next_state().\n
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
 * - <em>Enter functions</em>: <tt> void f(FSM<Key>*, const AnyValueList&); </tt>
 * - <em>Exit functions</em>: <tt> void f(FSM<Key>*); </tt>
 * - <em>FromTo functions</em>: <tt> void f(FSM<Key>*, const AnyValueList&); </tt>
 * - <em>Filter functions</em>: <tt> AnyValueList f(const StateKey&, const StateKey&, const AnyValueList&); </tt>
 * \note the Filter functions must return a AnyValueList with the first element
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
	 * 	Enter/FromTo/Filter callbacks have a AnyValueList parameter representing
	 * 	custom data, which can be passed by the various request/Next/Prev,
	 * 	demand and force_transition FSM functions.
	 */
	///@{
	typedef boost::function<void(FSM<StateKey>*, const AnyValueList&)> EnterFuncPTR;
	typedef boost::function<void(FSM<StateKey>*)> ExitFuncPTR;
	typedef boost::function<void(FSM<StateKey>*, const AnyValueList&)> FromToFuncPTR;
	///For a filter the first element of the returned AnyValueList is a
	///state key corresponding to the state to transition to, or the
	///value pFSM->Null if a transition is denied (here pFSM is the
	///passed pointer to the FSM).
	typedef boost::function<
			AnyValueList(FSM<StateKey>*, const StateKey&, const AnyValueList&)> FilterFuncPTR;
	typedef set<StateKey> AllowedStateKeySet;
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
	static void default_enter(FSM<StateKey>* fsm, const AnyValueList& data);
	EnterFuncPTR default_enterPTR;
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
	static void default_exit(FSM<StateKey>* fsm);
	ExitFuncPTR default_exitPTR;
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
	 * \note: the Filter functions must return a AnyValueList with the
	 * first element set to a state key or Null if the transition
	 * is denied.
	 * @param fsm The current FSM.
	 * @param toStateKey The destination state.
	 * @param data The custom data passed to this callback.
	 */
	static AnyValueList default_filter(FSM<StateKey>* fsm,
			const StateKey& toStateKey, const AnyValueList& data);
	FilterFuncPTR default_filterPTR;
	///@}

	/**
	 * \name Off State filter function definitions.
	 */
	///@{
	/**
	 * This Off State filter function allows always to go directly into
	 * any other state.
	 * \note: the Filter functions must return a AnyValueList with the
	 * first element set to a state key or Null if the transition
	 * is denied.
	 *
	 * @param fsm The current FSM.
	 * @param toStateKey The destination state.
	 * @param data The custom data passed to this callback.
	 */
	static AnyValueList filter_off(FSM<StateKey>* fsm, const StateKey& toStateKey,
			const AnyValueList& data);
	FilterFuncPTR filter_offPTR;
	///@}

	/**
	 * \brief Return the current filter function.
	 * @return The current filter function.
	 */
	FilterFuncPTR get_current_filter();

	/**
	 * \brief Internal function to change unconditionally to the indicated
	 * state if possible.
	 *
	 * @param newStateKey The destination state.
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 * @return The current state of this FSM.
	 */
	StateKey set_state(const StateKey& newStateKey, const AnyValueList& data =
			AnyValueList());

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

	typedef StateKey StateType;

	/**
	 * \brief The State set type.
	 */
	typedef set<StateTmpl<StateKey> > StateSet;
	/**
	 * \brief The FromToFunctions' table type.
	 */
	typedef map<pair<StateKey, StateKey>, FromToFuncPTR> FromToFunctionTable;

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
	 * @param name The FSM's "name".
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
	StateKey get_current_or_next_state() const;

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
	bool get_current_state_or_transition(StateKey& currStateKey,
			StateKey& toStateKey) const;

	/**
	 * \brief Return if a transition is currently active.
	 * @return If a transition is currently active.
	 */
	bool is_in_transition() const;

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
	 * Nothing is returned, see set_state() when an error occurs.
	 * @param stateKey The destination state.
	 * @param data The data passed to Enter/FromTo/Filter callbacks.
	 */
	void force_transition(const StateKey& stateKey, const AnyValueList& data =
			AnyValueList());

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
	void demand(const StateKey& stateKey, const AnyValueList& data = AnyValueList());

	/**
	 * \brief Requests a state transition (or other behavior).
	 *
	 * The request may be denied by the FSM's filter function.
	 * If it is denied, the filter function return a Null key.
	 * The request parameter should the new state key.\n
	 * The request, along with any additional arguments, is passed to
	 * the current filter State function.  If filter State returns a
	 * state key, the FSM transitions to that state.\n
	 * The return value is the same as the return value of the set_state()
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
	StateKey request(const StateKey& newStateKey, const AnyValueList& data =
			AnyValueList());

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
	StateKey request_next(const AnyValueList& data = AnyValueList());

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
	StateKey request_prev(const AnyValueList& data = AnyValueList());

	/**
	 * \name State change broadcasting.
	 *
	 * If set_broadcast_state_changes(true), this event will be sent through
	 * the EventHandler on every state change.\n
	 * The new and old states are accessible as mOldState and mNewState,
	 * and the transition functions will already have been called.
	 */
	///@{
	/**
	 * @param doBroadcast Flag to enable/disable broadcast.
	 */
	void set_broadcast_state_changes(bool doBroadcast);
	string get_state_change_event() const;
	///@}

	/**
	 * \name FSM construction functions.
	 */
	///@{
	bool add_state(const StateKey& stateKey,
			const FSM<StateKey>::EnterFuncPTR& enterFunc,
			const FSM<StateKey>::ExitFuncPTR& exitFunc,
			const FSM<StateKey>::FilterFuncPTR& filterFunc,
			const FSM<StateKey>::AllowedStateKeySet& allowedStateKeys =
					AllowedStateKeySet());
	bool remove_state(const StateKey& stateKey);
	bool add_from_to_func(const StateKey& stateFrom, const StateKey& stateTo,
			const FSM<StateKey>::FromToFuncPTR& fromToFunc);
	bool remove_from_to_func(const StateKey& stateFrom, const StateKey& stateTo);
	///@}

	/**
	 * \brief Set the states' set of this FSM in one shot.
	 * @param stateSet The state set.
	 */
	void set_state_set(const StateSet& stateSet);

	/**
	 * \brief Return the set of state keys.
	 * @return The set of state keys.
	 */
	set<StateKey> get_key_state_set() const;

	/**
	 * \brief Return the number of the states belonging to this FSM
	 * other than the Off initial state.
	 * @return The number of the states (Off apart).
	 */
	unsigned int get_num_states() const;

protected:
	//Data section
	///The Global serial number.
	static int SerialNum;
	/**
	 * \brief Return a  unique global serial number.
	 * @return
	 */
	int get_serial_num();
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
	///force_transition() while the FSM is in transition between
	///states.
	queue<FSMMethodPTR> mRequestQueue;
};

///inline
#include "FSM.I"

#endif /* FSM_H_ */
