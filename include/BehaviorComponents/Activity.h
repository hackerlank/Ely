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
 * \author marco
 */

#ifndef ACTIVITY_H_
#define ACTIVITY_H_

#include <string>
#include <list>
#include <set>
#include <nodePath.h>
#include <filename.h>
#include <typedObject.h>
#include <boost/bind.hpp>
#include "Support/FSM.h"
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

class ActivityTemplate;

/**
 * \brief Component representing the activity of an object.
 *
 * It is composed of a embedded fsm (i.e. FSM<std::string>) representing
 * the object game states (i.e. strings).\n
 * A state transition can be request by delegating its embedded FSM
 * interface functions, like in this sample code:
 * \code
 * 	((fsm&)activity).request("stateC", valueList);
 * \endcode
 * Given an object with id <OBJECTID>, for any state called <STATE> and
 * three "transition" functions can be defined with these signatures:
 * - <tt>void Enter_<STATE>_<OBJECTID>(fsm*, Activity& activity, const ValueList& valueList);</tt>
 * - <tt>void Exit_<STATE>_<OBJECTID>(fsm*, Activity& activity);</tt>
 * - <tt>ValueList Filter_<STATE>_<OBJECTID>(fsm*, Activity& activity, const std::string& state, const ValueList& valueList);</tt>
 * Furthermore for a pair of state <STATEA>, <STATEB> a "transition" function
 * can be defined with this signature:
 * - <tt> void <STATEA>_FromTo_<STATEB>_<OBJECTID>(fsm*, Activity& activity, const ValueList& valueList);</tt>
 *
 * All these routines are loaded at runtime from a dynamic linked library
 * (referenced by the macro TRANSITIONS_SO).\n
 * Inside these routine the Activity* "activity" argument passed refers to this
 * component.\n
 * \see FSM for details.
 *
 * XML Param(s):
 * - "states"  			|multiple|no default
 * - "from_to"			|multiple|no default
 *
 * \note given stateA and stateB the value of a parameter "from_to" would be
 * "stateA_FromTo_stateB".
 * \note each state name should be a valid C++ identifier, moreover, due to
 * the previous note, it should not contain the "_FromTo_" substring.
 */
class Activity: public Component
{
public:
	Activity();
	Activity(ActivityTemplate* tmpl);
	virtual ~Activity();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

	/**
	 * \brief fsm conversion function.
	 */
	operator fsm&();

private:
	///The embedded FSM.
	fsm mFSM;

	//Transition functions library management.

	///Set of "FromTo" transition functions.
	std::set<std::string> mFromToFunctionSet;

	/**
	 * \name Handles, typedefs, for managing library of transition functions.
	 */
	///@{
	LIB_HANDLE mTransitionLib;
	typedef void (*PENTER)(fsm*, Activity&, const ValueList&);
	typedef void (*PEXIT)(fsm*, Activity&);
	typedef ValueList (*PFILTER)(fsm*, Activity&, const std::string&,
			const ValueList&);
	typedef void (*PFROMTO)(fsm*, Activity&, const ValueList&);
	///@}

	///Helper flag.
	bool mTransitionsLoaded;

	/**
	 * \name Helper functions to load/unload event callbacks.
	 */
	///@{
	void loadTransitionFunctions();
	void unloadTransitionFunctions();
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

#endif /* ACTIVITY_H_ */
