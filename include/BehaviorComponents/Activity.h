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
 * the object game states (i.e. strings).\n
 * All objects of the same type share the same activity component's states.\n
 * A state transition can be request by delegating its embedded FSM
 * interface functions, like in this sample code:
 * \code
 * 	((fsm&)activity).request("stateC", valueList);
 * \endcode
 * Given an object with type <OBJECTYPE>, for any state called <STATE> and
 * three "transition" functions can be defined as parameter or, by default,
 * with these signatures:
 * - <tt>void Enter_<STATE>_<OBJECTYPE>(fsm*, Activity& activity, const ValueList& valueList);</tt>
 * - <tt>void Exit_<STATE>_<OBJECTYPE>(fsm*, Activity& activity);</tt>
 * - <tt>ValueList Filter_<STATE>_<OBJECTYPE>(fsm*, Activity& activity, const std::string& state, const ValueList& valueList);</tt>
 * Furthermore for a pair of state <STATEA>, <STATEB> a "transition" function
 * can be defined as parameter or, by default, with this signature:
 * - <tt> void <STATEA>_FromTo_<STATEB>_<OBJECTYPE>(fsm*, Activity& activity, const ValueList& valueList);</tt>
 *
 * All these routines are loaded at runtime from a dynamic linked library
 * (referenced by the macro TRANSITIONS_LA).\n
 * Inside these routine the SMARTPTR(Activity) "activity" argument passed refers to this
 * component.\n
 * \see FSM for details.
 *
 * XML Param(s):
 * - "states"  			|multiple|no default (each one specified as
 * "state1:state2:...:stateN$enterName,exitName,filterName",
 * with second part after "$" optional)
 * - "from_to"			|multiple|no default (each one specified as
 * "state1@state2$fromToName", with second part after "$" optional)
 *
 * \note "states" and "from_to" parameters are specified into the object
 * template definition.
 * \note each "-" in any computed string will be replaced by "_".
 */
class Activity: public Component
{
protected:
	friend class ActivityTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Activity();
	Activity(SMARTPTR(ActivityTemplate)tmpl);
	virtual ~Activity();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \name FSM reference getter & conversion function.
	 */
	///@{
	fsm& getFSM();
	operator fsm&();
	///@}

private:
	///The underlying FSM (read-only after creation & before destruction).
	fsm mFSM;

	//Transition functions library management.

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
	///Setup helper data.
	void doSetupHelperData();
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
	mTransitionLib = NULL;
	mTransitionsLoaded = false;
}

inline void Activity::onAddToSceneSetup()
{
}

inline void Activity::onRemoveFromSceneCleanup()
{
}

inline fsm& Activity::getFSM()
{
	return mFSM;
}

inline Activity::operator fsm&()
{
	return mFSM;
}

}  // namespace ely

#endif /* ACTIVITY_H_ */
