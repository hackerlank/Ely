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
 * \file /Ely/include/BehaviorComponents/Activity.h
 *
 * \date 17/ago/2012 (09:28:59)
 * \author consultit
 */

#ifndef ACTIVITY_H_
#define ACTIVITY_H_

#include <boost/bind.hpp>
#include "Support/FSM.h"
#include "ObjectModel/Component.h"

namespace ely
{
class ActivityTemplate;

/**
 * \brief Component representing the activity of an object.
 *
 * It is composed of a embedded fsm (i.e. FSM<std::string>) representing
 * the object game states (i.e. strings) and a "transition table" which
 * specify transitions from state to state upon event (types) reception.\n
 * Moreover it gives an object instance the chance to do custom update through a
 * function loaded at runtime from a dynamic linked library
 * (\see GameManager::GameDataInfo::INSTANCEUPDATES).
 * .
 * All objects of the same type share the same activity component's states.\n
 * A state transition can be request by delegating its embedded FSM
 * interface functions, like in this sample code:
 * \code
 * 	((fsm&)activity).request("stateC", valueList);
 * \endcode
 * Given an object with type <OBJECTYPE>, for any state <STATE> defined
 * three names of transition functions are defined by default
 * on a "per ObjectTemplate (i.e. Object type)" basis:
 * - <tt>Enter_<STATE>_<OBJECTYPE></tt>
 * - <tt>Exit_<STATE>_<OBJECTYPE></tt>
 * - <tt>Filter_<STATE>_<OBJECTYPE></tt>
 * These names can be overridden by parameters on a "per Object" basis.\n
 * Furthermore for a pair of state <STATEA>, <STATEB> a "FromTo" transition
 * function name can be defined, by default, on a "per ObjectTemplate
 * (i.e. Object type)" basis:
 * -<tt><STATEA>_FromTo_<STATEB>_<OBJECTYPE></tt>
 * This name can be overridden by parameters on a "per Object"
 * basis.\n
 * The transition functions are loaded at runtime from a dynamic linked library
 * (\see GameManager::GameDataInfo::TRANSITIONS).\n
 * Inside these routine the SMARTPTR(Activity) "activity" argument passed refers to this
 * component.\n
 * \see FSM for details.
 * .
 * The transition table is queried for the <NEXT STATE> given the pair
 * (<CURRENT STATE>,<EVENT TYPE>), so any of its elements is specified as\n
 * <CURRENT STATE>,<EVENT TYPE>@<NEXT STATE>\n
 * which means that when this component is in <CURRENT STATE>, and receive
 * an event of type <EVENT TYPE>, will make a transition on <NEXT STATE>.\n
 * So a common programming pattern, is:
 * - specify the states
 * - specify the event types that cause state transitions
 * - specify the transition table elements (triples)
 * - in the event's callback query the fsm for the current state and, given
 * the type of this event, ask the transition table for the next state to go.\n
 * Transition table elements, besides for states and event types, are defined
 * on a "per ObjectTemplate (i.e. Object type)" basis.\n
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *states*  				|multiple| - | each one specified as "state1[:state2:...:stateN]" into ObjectTemplate definition
 * | *from_to*				|multiple| - | each one specified as "state11@state21[:state12@state22:...:state1N@state2N]" into ObjectTemplate definition
 * | *states_transition* 	|multiple| - | each one specified as "state1[:state2:...:stateN]$enterName,exitName,filterName" into Object definition
 * | *from_to_transition*	|multiple| - | each one specified as "state11@state21[:state12@state22:...:state1N@state2N]$fromToName" into Object definition
 * | *transition_table*		|multiple| - | each one specified as "current_state1,event_type1@next_state1[:current_state2,event_type2@next_state2:...:current_stateN,event_typeN@next_stateN]" into ObjectTemplate definition
 * | *instance_update* 		|single| - | -
 *
 * \note in "states_transition" and "from_to_transition" any of
 * enterName, exitName, filterName, fromToName could be empty (meaning
 * that we want the defaults). And inside the strings representing these
 * predefined names, any "-" will be replaced by "_".
 *
 * \note parts inside [] are optional.\n
 */
class Activity: public Component
{
protected:
	friend class ActivityTemplate;

	Activity(SMARTPTR(ActivityTemplate)tmpl);
	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	virtual ~Activity();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \name FSM reference getter & conversion functions.
	 */
	///@{
	fsm& getFSM();
	operator fsm&();
	///@}

	/**
	 * \brief Updates this component.
	 *
	 * Will be called automatically by a behavior manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \name Transition table type declarations and getter & conversion functions.
	 */
	///@{
	typedef std::pair<std::string, std::string> StateEventType;
	typedef std::string NextState;
	typedef std::pair<StateEventType, NextState> TransitionTableItem;
	typedef std::map<StateEventType, NextState> TransitionTable;
	TransitionTable& getTransitionTable();
	operator TransitionTable&();
	///@}

private:
	///The underlying FSM (read-only after creation & before destruction).
	fsm mFSM;
	//Transition functions library management.
	///State transitions.
	std::list<std::string> mStateTransitionListParam;
	///FromTo transitions.
	std::list<std::string> mFromToTransitionListParam;
	/**
	 * \name Temporary helper data/functions.
	 */
	///@{
	///Transition functions' names keyed by state.
	struct TransitionNameTriple
	{
		TransitionNameTriple():
		mEnter(""),mExit(""),mFilter("")
		{}
		std::string mEnter;
		std::string mExit;
		std::string mFilter;
	};
	std::map<std::string, TransitionNameTriple> mStateTransitionTable;
	///FromTo transition functions' names keyed by state.
	typedef std::pair<std::string, std::string> StatePair;
	std::map<StatePair, std::string> mStatePairFromToTable;
	///Setup FSM helper data.
	void doSetupFSMData();
	///@}
	/**
	 * \name Handles, typedefs, for managing library of transition functions.
	 */
	///@{
	lt_dlhandle mTransitionLib;
	typedef void (*PENTER)(fsm*, Activity&, const ValueList&);
	typedef void (*PEXIT)(fsm*, Activity&);
	typedef ValueList (*PFILTER)(fsm*, Activity&, const std::string&,
			const ValueList&);
	typedef void (*PFROMTO)(fsm*, Activity&, const ValueList&);
	///@}
	///Helper flag.
	bool mTransitionsLoaded;
	/**
	 * \name Helper functions to load/unload transition functions.
	 */
	///@{
	void doLoadTransitionFunctions();
	void doUnloadTransitionFunctions();
	///@}

	//Transition table management.
	///@{
	TransitionTable mTransitionTable;
	///Setup Transition Table helper data.
	void doSetupTransitionTableData();
	///@}

	//Update management data types, variables and functions.
	/**
	 * \name Handles, typedefs, for managing library of instances' update functions.
	 */
	///@{
	lt_dlhandle mInstanceUpdateLib;
	typedef void* (*PINSTANCEUPDATE)(float dt, Activity& activity);
	///@}
	///Instance update function.
	PINSTANCEUPDATE mInstanceUpdate;
	///Instance update function name.
	std::string mInstanceUpdateName;
	/**
	 * \name Helper functions to load/unload instance update function.
	 */
	///@{
	void doLoadInstanceUpdate();
	void doUnloadInstanceUpdate();
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Activity", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

///inline definitions

inline void Activity::reset()
{
	mFSM.cleanup();
	mStateTransitionListParam.clear();
	mFromToTransitionListParam.clear();
	mTransitionTable.clear();
}

inline fsm& Activity::getFSM()
{
	return mFSM;
}

inline Activity::operator fsm&()
{
	return mFSM;
}

inline Activity::TransitionTable& Activity::getTransitionTable()
{
	return mTransitionTable;
}

inline Activity::operator Activity::TransitionTable&()
{
	return mTransitionTable;
}

///Template

class ActivityTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	ActivityTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ActivityTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType familyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "ActivityTemplate",
				ComponentTemplate::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};
}  // namespace ely

#endif /* ACTIVITY_H_ */
