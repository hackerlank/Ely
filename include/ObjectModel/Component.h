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
#include <pmutex.h>
#include <conditionVar.h>

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
 * Each Object of a given type has, for a given Component, the same
 * event types to respond to. This means that each event type, for a
 * Component is defined  on "per ObjectTemplate (i.e. Object type)"
 * basis.\n
 * By default, when an event type is defined, it is assumed that an event
 * with the same value as the event type associated to a callback function
 * with this signature:
 * \code
 * void <EVENTTYPE>_<COMPONENTTYPE>_<OBJECTTYPE>(const Event* event, void* data);
 * \endcode
 * are defined: this means that, by default, all Components of the same type
 * (belonging to any Object of that type), will respond to the same event value
 * with the same callback function.\n
 * Both the event value and the callback function can be overridden by parameters
 * on a "per Object basis".\n
 * The callback functions are loaded at runtime from a dynamic linked library
 * (\see GameManager::GameDataInfo::CALLBACKS).\n
 * If a callback function doesn't exist or if any error occurs, the default
 * callback (referenced by the macro DEFAULT_CALLBACK_NAME) is used.\n
 * To check if a "name" is an allowed event type call:
 * \code
 * 	ObjectTemplateManager::isComponentParameter();
 * \endcode
 *
 * XML Param(s):
 * - "event_types" 		|multiple|no default (each specified as
 * "evType1:evType2:...:evTypeN" into ObjectTemplate definition)
 * - "events"			|multiple|no default (each specified as
 * "evType1@evValue1[:evType2@evValue2:...:evTypeN@evValueN]$callbackName"
 *  into Object definition)
 *
 * \note in "events" any of evValues or  callbackNamecan be empty
 * (meaning we want the defaults value).
 * \note Inside the strings representing the above mentioned predefined
 * callback function names, any "-" will be replaced by "_".
 * \note parts inside [] are optional.\n
 */
class Component: public TypedWritableReferenceCount
{
protected:
	friend class ComponentTemplate;
	friend class ObjectTemplateManager;

	/**
	 * \brief Constructor.
	 */
	Component();

	/**
	 * \brief Resets all component data members.
	 */
	virtual void reset() = 0;

	/**
	 * \brief Allows a component to be initialized.
	 *
	 * This can be done after creation but "before" insertion into an object.\n
	 * This method for all derived classes are called only by methods of the
	 * respective ComponentTemplate derived class.\n
	 * This method for all derived classes are called (indirectly) only
	 * by ObjectTemplateManager methods during its creation and before
	 * it is publicly available to other threads.\n
	 */
	virtual bool initialize() = 0;

	/**
	 * \brief Sets the component unique identifier.
	 *
	 * This method for all derived classes are called only by methods of the
	 * respective ComponentTemplate derived class.\n
	 * This method for all derived classes are called (indirectly) only
	 * by ObjectTemplateManager methods during its creation and before
	 * it is publicly available to other threads.\n
	 * \param componentId The component unique identifier.
	 */
	void setComponentId(const ComponentId& componentId);

	/**
	 * \brief On addition to object setup.
	 *
	 * Gives a component the ability to do some setup just "after" this
	 * component has been added to an object. Optional.\n
	 * This method for all derived classes are called only by ObjectTemplateManager
	 * methods during its creation and before it is publicly available to
	 * other threads.\n
	 */
	///@{
	void addToObjectSetup();
	virtual void onAddToObjectSetup() = 0;
	///@}

	/**
	 * \brief On remove from object cleanup.
	 *
	 * Gives a component the ability to do some cleanup just "before" this
	 * component will be removed from an object. Optional.\n
	 */
	///@{
	void removeFromObjectCleanup();
	virtual void onRemoveFromObjectCleanup() = 0;
	///@}

	/**
	 * \brief On object addition to scene setup.
	 *
	 * Gives a component the ability to do some setup just "after" the
	 * object, this component belongs to, has been added to the scene
	 * and set up. Optional.\n
	 * This method for all derived classes are called only by ObjectTemplateManager
	 * methods during its creation and before it is publicly available to
	 * other threads.\n
	 * A possible request registration to any "Game*Manager" for updating
	 * "must" be done in this method as the last thing.\n
	 */
	///@{
	void addToSceneSetup();
	virtual void onAddToSceneSetup() = 0;
	///@}

	/**
	 * \brief On object removal from scene cleanup.
	 *
	 * Gives a component the ability to do some cleanup just "before" this
	 * component will be removed from the scene. Optional.\n
	 * A possible request for cancellation from any "Game*Manager" for updating
	 * "must" be done in this method as the first thing (before locking the mutex).\n
	 */
	///@{
	void removeFromSceneCleanup();
	virtual void onRemoveFromSceneCleanup() = 0;
	///@}

	/**
	 * \brief Sets the owner object.
	 *
	 * This method for all derived classes are called only by object methods.
	 * \param ownerObject The owner object.
	 */
	void setOwnerObject(SMARTPTR(Object)ownerObject);

public:

	/**
	 * \brief Destructor.
	 */
	virtual ~Component();

	/**
	 * \brief Gets the type of this component.
	 * @return The id of this component.
	 */
	virtual ComponentType componentType() const = 0;

	/**
	 * \brief Gets the family type of this component.
	 * @return The family id of this component.
	 */
	virtual ComponentFamilyType familyType() const = 0;

	/**
	 * \brief The result type a component method can return to signal
	 * the status of result of the operation or of the component.
	 *
	 * Derived component can derive from this to add other result status,
	 * by starting the enum from COMPONENT_RESULT_END+1.\n
	 */
	struct Result
	{
		int mResult;
		enum
		{
			OK,
			ERROR,
			DESTROYING,
			COMPONENT_RESULT_END
		};
		Result(int value):mResult(value)
		{
		}
		operator int()
		{
			return mResult;
		}
	};

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

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:
	///The template used to construct this component (read only after creation).
	SMARTPTR(ComponentTemplate) mTmpl;
	///Unique identifier for this component (read only after creation).
	ComponentId mComponentId;
	///The object this component is a member of (read only after component creation).
	SMARTPTR(Object) mOwnerObject;
#ifdef ELY_THREAD
	///Signals this component is going to be destroyed.
	bool mDestroying;
#endif

#ifdef ELY_THREAD
	///The mutex associated with this component.
	ReMutex mMutex;
#endif

	/**
	 * \name Helper functions to register/unregister events' callbacks.
	 *
	 * This interface can be called by the derived components to setup
	 * and register/unregister callbacks for events this component
	 * should respond to.\n
	 * Functions registerEventCallbacks/unregisterEventCallbacks can
	 * be used, after the component has been added to object, to
	 * add/remove callbacks to/from the global EventHandler.\n
	 * All these functions can be called multiple time in a safe way.\n
	 * \note Because component's events are shared by all object of the same type,
	 * and because their types are stored into the object template, these
	 * functions should be called only after the component's owner object
	 * has been set, for example into addToObjectSetup component method.
	 */
	///@{
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
	///@}

	///Table of events keyed by event types.
	std::map<std::string, std::string> mEventTable;
	///Table of event types keyed by events.
	std::map<std::string, std::string> mEventTypeTable;
	///Table of <name,callback> pairs keyed by event type names.
	typedef std::pair<std::string, PCALLBACK> NameCallbackPair;
	std::map<std::string, NameCallbackPair> mCallbackTable;

	///Helper flags.
	bool mCallbacksLoaded, mCallbacksRegistered;

	/**
	 * \name Helper functions to setup/cleanup events' tables and
	 * to load/unload events' callbacks.
	 */
	///@{
	void doSetupEventTables();
	void doCleanupEventTables();
	void doLoadEventCallbacks();
	void doUnloadEventCallbacks();
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

inline void Component::addToSceneSetup()
{
	onAddToSceneSetup();
}

inline void Component::removeFromSceneCleanup()
{
#ifdef ELY_THREAD
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		mDestroying = true;
	}
#endif

	onRemoveFromSceneCleanup();
}

inline void Component::update(void* data)
{
}

#ifdef ELY_THREAD
inline ReMutex& Component::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* COMPONENT_H_ */
