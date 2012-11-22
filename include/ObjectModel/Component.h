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
 * \file /Ely/include/ObjectModel/Component.h
 *
 * \date 09/mag/2012 (16:09:17)
 * \author marco
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <cstring>
#include <typedWritableReferenceCount.h>
#include <genericAsyncTask.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "Utilities/Tools.h"

/**
 * \brief Component identifier type.
 */
typedef std::string ComponentType;
/**
 * \brief Component family identifier type.
 */
typedef std::string ComponentFamilyType;
/**
 * \brief Object instance identifier type (by default the name
 * of the NodePath component).
 */
typedef std::string ComponentId;

class Object;
class ComponentTemplate;

/**
 * \brief Base abstract class to provide a common interface for
 * all object components.
 *
 * Components are organized into class hierarchies called families.
 * Each component belongs to a family and is derived from a base
 * family component. Any object can have only one component of
 * each family type.\n
 * Each component can be updated directly or indirectly through a
 * Panda task.\n
 * Each component can respond to Panda events (stimuli) by registering
 * or unregistering callbacks for them with the global EventHandler.\n
 * Each event has a type, and a callback associated.\n
 * For any event of type "<EVENTTYPE>" associated with a component of type
 * <COMPONENTTYPE> of an object of type <OBJECTTYPE>, there exists a callback
 * named "<EVENTTYPE>_<COMPONENTTYPE>_<OBJECTTYPE>" (see note) which is
 * loaded at runtime from a dynamic linked library (referenced by the
 * macro CALLBACKS_SO). If this variable doesn't exist
 * or if any error occurs the default callback (referenced by the macro
 * DEFAULT_CALLBACK) is used.\n
 * The name of the callback for a given "event_type" can be queried with:
 * \code
 * 	ComponentTemplate::getEventTypeCallback("event_type");
 * \endcode
 * \note In the various substrings composing the name:
 * - hyphens ("-") are replaced with underscores ("_")
 */
class Component: public TypedWritableReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	Component();

	/**
	 * \brief Destructor.
	 */
	virtual ~Component();

	/**
	 * \brief Gets the type of this component.
	 * @return The id of this component.
	 */
	const virtual ComponentType componentType() const = 0;

	/**
	 * \brief Gets the family type of this component.
	 * @return The family id of this component.
	 */
	const virtual ComponentFamilyType familyType() const = 0;

	/**
	 * \brief Updates the state of the component.
	 *
	 * @param data Generic data.
	 */
	virtual void update(void* data);

	/**
	 * \brief Updates the state of the component.
	 *
	 * This overload allows to exploit the Panda framework task management.
	 * See TaskInterface.
	 * @param task The task object.
	 * @return The "done" status.
	 */
	virtual AsyncTask::DoneStatus update(GenericAsyncTask* task);

	/**
	 * \brief Allows a component to be initialized.
	 *
	 * This can be done after creation but "before" insertion into an object.
	 */
	virtual bool initialize() = 0;

	/**
	 * \brief On addition to object setup.
	 *
	 * Gives a component the ability to do some setup just "after" this
	 * component has been added to an object. Optional.
	 */
	virtual void onAddToObjectSetup();

	/**
	 * \brief On object addition to scene setup.
	 *
	 * Gives a component the ability to do some setup just "after" the
	 * object, this component belongs to, has been added to the scene
	 * and set up. Optional.
	 */
	virtual void onAddToSceneSetup();

	/**
	 * \brief Gets/sets the owner object.
	 * \return The owner object.
	 */
	///@{
	void setOwnerObject(SMARTPTR(Object) ownerObject);
	SMARTPTR(Object) getOwnerObject() const;
	///@}

	/**
	 * \brief Sets the component unique identifier.
	 * \return The component unique identifier.
	 */
	void setComponentId(const ComponentId& componentId);

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

protected:
	///The template used to construct this component.
	SMARTPTR(ComponentTemplate) mTmpl;
	///Unique identifier for this component.
	ComponentId mComponentId;
	///The object this component is a member of.
	SMARTPTR(Object) mOwnerObject;

	///The (reentrant) mutex associated with this component.
	ReMutex mMutex;

	/**
	 * \name Helper interface for event management.
	 *
	 * This interface can be called by the derived components to setup
	 * and register/unregister callbacks for events this component
	 * should respond to.\n
	 * Function setupEvents can be called to initialize the
	 * set of events if any.\n
	 * Functions registerEventCallbacks/unregisterEventCallbacks can
	 * be used, after the component has been added to object, to
	 * add/remove callbacks to/from the global EventHandler.\n
	 * All these functions can be called multiple time in a safe way.
	 */
	///@{
	void setupEvents();
	void registerEventCallbacks();
	void unregisterEventCallbacks();
	///@}

private:
	//Event management data types and variables.

	/**
	 * \name Handles, typedefs, for managing library of event callbacks.
	 */
	///@{
	LIB_HANDLE mCallbackLib;
	typedef EventHandler::EventCallbackFunction* PCALLBACK;
	typedef std::string* PCALLBACKNAME;
	///@}

	///Table of callbacks keyed by event names.
	std::map<std::string, PCALLBACK> mCallbackTable;

	///Helper flags.
	bool mCallbacksLoaded, mCallbacksRegistered;

	/**
	 * \name Helper functions to load/unload event callbacks.
	 */
	///@{
	void loadEventCallbacks();
	void unloadEventCallbacks();
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedObject::init_type();
		register_type(_type_handle, "Component", TypedObject::get_class_type());
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

#endif /* COMPONENT_H_ */
