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
#include <nodePath.h>
#include <filename.h>
#include <typedObject.h>
#include "Support/FSM.h"
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

class ActivityTemplate;

/**
 * \brief Component representing the activity of an object.
 *
 * It is composed of a embedded fsm (= FSM<std::string>) representing
 * the object game states (i.e. strings).
 * A state transition can be request by delegating its embedded FSM
 * interface functions, like in this sample code:
 * \code
 * 	((fsm)activity).request("stateC", valueList);
 * \endcode
 * For any state called "State" three "transition" functions can be
 * defined with these signatures:
 * \li \c void EnterState(fsm*, Activity& act, const ValueList& vl);
 * \li \c void ExitState(fsm*, Activity& act);
 * \li \c ValueList FilterState(fsm*, Activity& act, const std::string& state, const ValueList& vl);
 * Furthermore for a pair of state "StateA", "StateB" a "transition" function
 * can be defined with this signature:
 * \li \c void FromStateAToStateB(fsm*, Activity& act, const ValueList& vl);
 * All these routines are loaded at runtime from a dynamic linked library
 * (referenced by the macro TRANSITIONS_SO).
 * Inside these routine the Activity* "act" argument passed refers to this
 * component.
 * For details see FSM.
 *
 * XML Param(s):
 * \li \c "states"  			|multiple|no default
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
	operator fsm();

private:
	///The embedded FSM.
	fsm mFSM;

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
