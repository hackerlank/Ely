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
 * It is composed of a FSM (Finite State Machine) that represents the
 * game states of the object.
 * Practically it is a "tiny" wrapper around its embedded FSM (see
 * FSM).
 * For any state called "State" three boost::function can be defined
 * with these names:
 * \li \c "EnterState"
 * \li \c "ExitState"
 * \li \c "FilterState"
 * Furthermore for a pair of state "StateA", "StateB" a routine can
 * be defined with this name:
 * \li \c "FromStateAToStateB"
 * All these routines are loaded at runtime from a dynamic linked library
 * (referenced by the macro TRANSITIONS_SO).
 * For their signatures see FSM.
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

	///////////////////////

	std::string request(const std::string& newStateKey, const ValueList& data =
			ValueList())
	{
		ValueList newData = data;
		newData.push_front(this);
		mFSM.request(newStateKey, newData);
		return "";
	}

	/////////////////////////
	/**
	 * \brief Gets/sets the node path associated to this model.
	 * @return The node path associated to this model.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

private:
	///The NodePath associated to this model.
	NodePath mNodePath;
	///The embedded FSM.
	fsm mFSM;

//	static void Enter(fsm*, const ValueList&)
//	{
//	}
//	static void Exit(fsm*)
//	{
//	}
//	static void Filter(fsm*, const std::string&, const ValueList&)
//	{
//	}
//	static void FromTo(fsm*, const ValueList&)
//	{
//	}

///@{
///Data members
	std::string mCurrentState, mNewState;
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
