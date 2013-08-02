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
 * \author consultit
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "Utilities/Tools.h"
#include <typedWritableReferenceCount.h>
#include <genericAsyncTask.h>
#include <reMutex.h>

namespace ely
{
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
 * <COMPONENTTYPE> of an object of type <OBJECTTYPE>, there exists a
 * global std::string variable named "<EVENTTYPE>_<COMPONENTTYPE>_<OBJECTTYPE>"
 * (see note) which is loaded at runtime from a dynamic linked library
 * (referenced by the macro CALLBACKS_LA) and whose value is the name of
 * the callback function (contained in the same library too) for the same event
 * type.\n
 * If this variable doesn't exist or if any error occurs the default
 * callback (referenced by the macro DEFAULT_CALLBACK_NAME) is used.\n
 * To check if a "name" is an allowed event type call:
 * \code
 * 	ComponentTemplate::isEventType("name");
 * \endcode
 * \note In the various substrings composing the name:
 * - hyphens ("-") are replaced with underscores ("_")\n
 *
 * XML Param(s):
 * - "event_types" 		|multiple|no default
 * - "events"			|multiple|no default (each specified as "event_type@event")
 * \note "event_types" parameters are specified into the object
 * template definition.
 */
class Component: public TypedWritableReferenceCount
{
protected:
	friend class Object;
	friend class ComponentTemplate;

	/**
	 * \brief Allows a component to be initialized.
	 *
	 * This can be done after creation but "before" insertion into an object.\n
	 * This method for all derived classes are called only by methods of the
	 * respective ComponentTemplate derived class.
	 */
	virtual bool initialize() = 0;

	/**
	 * \brief Sets the owner object.
	 *
	 * This method for all derived classes are called only by object methods.
	 * \param ownerObject The owner object.
	 */
	void setOwnerObject(SMARTPTR(Object)ownerObject);

	/**
	 * \brief Sets the component unique identifier.
	 * \param componentId The component unique identifier.
	 */
	void setComponentId(const ComponentId& componentId);

	/**
	 * \brief On addition to object setup.
	 *
	 * Gives a component the ability to do some setup just "after" this
	 * component has been added to an object. Optional.\n
	 * This method for all derived classes are called only by object methods.
	 */
	virtual void onAddToObjectSetup();

	/**
	 * \brief On object addition to scene setup.
	 *
	 * Gives a component the ability to do some setup just "after" the
	 * object, this component belongs to, has been added to the scene
	 * and set up. Optional.\n
	 * This method for all derived classes are called only by object methods.
	 */
	virtual void onAddToSceneSetup();

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
	 * \brief Gets the owner object.
	 * \return The owner object.
	 */
	SMARTPTR(Object) getOwnerObject() const;

	/**
	 * \brief Gets the component unique identifier.
	 * \return The component unique identifier.
	 */
	ComponentId getComponentId() const;

	/**
	 * \brief Return the type of an event.
	 * @param event The event.
	 * @return The type of the event.
	 */
	std::string getEventType(const std::string& event);

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
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
	 * All these functions can be called multiple time in a safe way.\n
	 * \note Because component's events are shared by all object of the same type,
	 * and because their types are stored into the object template, these
	 * functions should be called only after the component's owner object
	 * has been set, for example into onAddToObjectSetup component method.
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
	lt_dlhandle mCallbackLib;
	typedef EventHandler::EventCallbackFunction* PCALLBACK;
	typedef std::string* PCALLBACKNAME;
	///@}

	///Table of events keyed by event types.
	std::map<std::string, std::string> mEventTable;
	///Table of event types keyed by events.
	std::map<std::string, std::string> mEventTypeTable;
	///Table of callbacks keyed by event type names.
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

///inline definitions

inline void Component::setComponentId(const ComponentId& componentId)
{
	mComponentId = componentId;
}

inline ComponentId Component::getComponentId() const
{
	return mComponentId;
}

inline ReMutex& Component::getMutex()
{
	return mMutex;
}

}  // namespace ely

#endif /* COMPONENT_H_ */
