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
#include <pandaFramework.h>
#include <typedWritableReferenceCount.h>
#include <genericAsyncTask.h>
#include <pmutex.h>
#include <conditionVar.h>
#include <list>

namespace ely
{
/**
 * \brief Component type.
 */
typedef std::string ComponentType;
/**
 * \brief Component family type.
 */
typedef std::string ComponentFamilyType;
/**
 * \brief Component instance identifier type (by default the name
 * of the NodePath Component).
 */
typedef std::string ComponentId;

class Object;
class ComponentTemplate;

/**
 * \brief Base abstract class to provide a common interface for
 * all Components.
 *
 * Components are organized into sets of similar behavior called families.
 * Each Component has type and belongs to a family and is derived from this base
 * class. Any Object can have only one Component of each family type.\n
 * Each Component can be updated directly at each game loop tick (or frame).\n
 * Each Component can respond to Panda events (stimuli) by registering
 * or unregistering callbacks for them with the global EventHandler.\n
 * An Object can respond to events only through its owned Components.
 * For any owned Component type, an Object type can optionally declare, the set
 * of event types which that Component type has to respond to.
 * This means that each event type, declared for a Component type,
 * is defined  on "per ObjectTemplate (i.e. Object type) basis".
 * Panda events are specified as values of these event types on a "per
 * Object basis".\n
 * By default, when an event type is defined, any event specified
 * as value of the event type, is associated to a callback function
 * with this signature:
 * \code
 * void <EVENTTYPE>_<COMPONENTTYPE>_<OBJECTTYPE>(const Event* event, void* data);
 * \endcode
 * This means that, by default, Components of the same type that
 * belong to any Object of a given type, will respond to these events
 * with the same callback function.\n
 * Any event can be ignored on a "per Object basis" by not specifying it in
 * parameters. Moreover, both the event type values and the callback function
 * can be overridden by parameters on a "per Object basis".\n
 * The callback functions are loaded at runtime from a dynamic linked library
 * (\see GameManager::GameDataInfo::CALLBACKS).\n
 * If a callback function doesn't exist or if any error occurs, the default
 * callback (referenced by the macro DEFAULT_CALLBACK_NAME) is used.\n
 * To check if a (name,value) pair is a declared legitimate event type call:
 * \code
 * 	ObjectTemplate::isComponentParameterValue();
 * \endcode
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *event_types* 		|multiple| - | each specified as "evType1[:evType2:...:evTypeN]" into ObjectTemplate definition
 * | *events*			|multiple| - | each specified as "evType1@evValue1[:evType2@evValue2:...:evTypeN@evValueN]$[callbackName]" into Object definition
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
	 * \brief Resets all Component data members.
	 */
	virtual void reset() = 0;

	/**
	 * \brief Allows a Component to be initialized.
	 *
	 * This can be done after creation but "before" insertion into an Object.\n
	 * This method for all derived classes are called only by methods of the
	 * respective ComponentTemplate derived class.\n
	 * This method for all derived classes are called (indirectly) only
	 * by ObjectTemplateManager methods during its creation and before
	 * it is publicly available to other threads.\n
	 */
	virtual bool initialize() = 0;

	/**
	 * \brief Sets the Component unique identifier.
	 *
	 * This method for all derived classes are called only by methods of the
	 * respective ComponentTemplate derived class.\n
	 * This method for all derived classes are called (indirectly) only
	 * by ObjectTemplateManager methods during its creation and before
	 * it is publicly available to other threads.\n
	 * \param componentId The Component unique identifier.
	 */
	void setComponentId(const ComponentId& componentId);

	/**
	 * \brief On addition to Object setup.
	 *
	 * Gives a Component the ability to do some setup just "after" this
	 * Component has been added to an Object. Optional.\n
	 * This method for all derived classes are called only by ObjectTemplateManager
	 * methods during its creation and before it is publicly available to
	 * other threads.\n
	 * addToObjectSetup() is the setup common to all Component's types, while
	 * onAddToObjectSetup() is the specialized setup implemented by a derived
	 * Component type.
	 */
	///@{
	void addToObjectSetup();
	virtual void onAddToObjectSetup() = 0;
	///@}

	/**
	 * \brief On remove from Object cleanup.
	 *
	 * Gives a Component the ability to do some cleanup just "before" this
	 * Component will be removed from an Object. Optional.\n
	 * removeFromObjectCleanup() is the cleanup common to all Component's types, while
	 * onRemoveFromObjectCleanup() is the specialized cleanup implemented by a derived
	 * Component type.
	 */
	///@{
	void removeFromObjectCleanup();
	virtual void onRemoveFromObjectCleanup() = 0;
	///@}

	/**
	 * \brief On Object addition to scene setup.
	 *
	 * Gives a Component the ability to do some setup just "after" the
	 * Object, this Component belongs to, has been added to the scene
	 * and set up. Optional.\n
	 * This method for all derived classes are called only by ObjectTemplateManager
	 * methods during its creation and before it is publicly available to
	 * other threads.\n
	 * A possible request registration to any "Game*Manager" for updating
	 * "must" be done in this method as the last thing.\n
	 * addToSceneSetup() is the setup common to all Component types, while
	 * onAddToSceneSetup() is the specialized setup implemented by a derived
	 * Component type.
	 */
	///@{
	void addToSceneSetup();
	virtual void onAddToSceneSetup() = 0;
	///@}

	/**
	 * \brief On Object removal from scene cleanup.
	 *
	 * Gives a Component the ability to do some cleanup just "before" this
	 * Component will be removed from the scene. Optional.\n
	 * A possible request for cancellation from any "Game*Manager" for updating
	 * "must" be done in this method as the first thing (before locking the mutex).\n
	 * removeFromSceneCleanup() is the cleanup common to all Component's types, while
	 * onRemoveFromSceneCleanup() is the specialized cleanup implemented by a derived
	 * Component type.
	 */
	///@{
	void removeFromSceneCleanup();
	virtual void onRemoveFromSceneCleanup() = 0;
	///@}

	/**
	 * \brief Sets the owner Object.
	 *
	 * This method for all derived classes are called only by Object methods.
	 * \param ownerObject The owner Object.
	 */
	void setOwnerObject(SMARTPTR(Object)ownerObject);

public:

	/**
	 * \brief Destructor.
	 */
	virtual ~Component();

	/**
	 * \brief Gets the type of this Component.
	 * @return The id of this Component.
	 */
	ComponentType componentType() const;

	/**
	 * \brief Gets the family type of this Component.
	 * @return The family type of this Component.
	 */
	ComponentFamilyType familyType() const;

	/**
	 * \brief The result type a Component method can return to signal
	 * the status of result of the operation or of the Component.
	 *
	 * Derived Component can derive from this to add other result status,
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
	 * \brief Updates the state of the Component.
	 *
	 * @param data Generic data.
	 */
	virtual void update(void* data);

	/**
	 * \brief Gets the owner Object.
	 * \return The owner Object.
	 */
	SMARTPTR(Object) getOwnerObject() const;

	/**
	 * \brief Gets the Component unique identifier.
	 * \return The Component unique identifier.
	 */
	ComponentId getComponentId() const;

	/**
	 * \brief Return the type of an event.
	 * @param event The event.
	 * @return The type of the event.
	 */
	std::string getEventType(const std::string& event);

	/**
	 * \brief Set the Component as free.
	 * @param free The free flag.
	 */
	void setFreeFlag(bool freeComponent);

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:
	///The Template used to construct this Component (read only after creation).
	SMARTPTR(ComponentTemplate) mTmpl;
	///Unique identifier for this Component (read only after creation).
	ComponentId mComponentId;
	///The Object this Component is a member of (read only after Component creation).
	SMARTPTR(Object) mOwnerObject;
	///Free Component flag.
	bool mFreeComponent;
#ifdef ELY_THREAD
	///Signals this Component is going to be destroyed.
	bool mDestroying;
#endif

#ifdef ELY_THREAD
	///The mutex associated with this Component.
	ReMutex mMutex;
#endif

	/**
	 * \name Helper functions to register/unregister events' callbacks.
	 *
	 * This interface can be called by the derived Components to setup
	 * and register/unregister callbacks for events this Component
	 * should respond to.\n
	 * Functions registerEventCallbacks/unregisterEventCallbacks can
	 * be used, after the Component has been added to Object, to
	 * add/remove callbacks to/from the global EventHandler.\n
	 * All these functions can be called multiple time in a safe way.\n
	 * \note Because Component's events are shared by all Object of the same type,
	 * and because their types are stored into the Object Template, these
	 * functions should be called only after the Component's owner Object
	 * has been set, for example into addToObjectSetup Component method.
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
		TypedWritableReferenceCount::init_type();
		register_type(_type_handle, "Component",
				TypedWritableReferenceCount::get_class_type());
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

inline void Component::setFreeFlag(bool freeComponent)
{
	mFreeComponent = freeComponent;
}

#ifdef ELY_THREAD
inline ReMutex& Component::getMutex()
{
	return mMutex;
}
#endif

///Template
/**
 * \brief Abstract base class of Component templates used to create Components.
 */
class ComponentTemplate: public TypedWritableReferenceCount
{
protected:
	friend class ComponentTemplateManager;

	/**
	 * \brief Creates the current Component of that family.
	 * @param compId The Component identifier.
	 * @return The Component just created, NULL if Component cannot be created.
	 */
	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId) = 0;

public:

	/**
	 * \brief Constructor.
	 */
	ComponentTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);

	/**
	 * \brief Destructor.
	 */
	virtual ~ComponentTemplate();

	/**
	 * \brief Gets the type id of the Component created.
	 * @return The type id of the Component created.
	 */
	virtual ComponentType componentType() const = 0;
	/**
	 * \brief Gets the family id of the Component created.
	 * @return The family id of the Component created.
	 */
	virtual ComponentFamilyType familyType() const = 0;

	/**
	 * \brief For the Component this Template is designed to create,
	 * this function sets the parameters to their default values.
	 */
	virtual void setParametersDefaults() = 0;

	/**
	 * \name Parameters management.
	 * \brief Sets the parameters of the Component, this Template is
	 * designed to create, to custom values.
	 *
	 * These parameters overwrite (and/or are added to) the parameters defaults
	 * set by setParametersDefaults.
	 * @param parameterTable The table of (parameter,value).
	 */
	void setParameters(const ParameterTable& parameterTable);

	/**
	 * \brief Gets the parameter value associated to the Component.
	 * @param paramName The name of the parameter.
	 * @return The value of the parameter, empty string if none exists.
	 */
	std::string parameter(const std::string& paramName) const;

	/**
	 * \brief Gets the parameter multi-values associated to the Component.
	 * @param paramName The name of the parameter.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	std::list<std::string> parameterList(const std::string& paramName);

	/**
	 * \brief Gets the entire parameter table.
	 * @return The parameter table.
	 */
	ParameterTable getParameterTable() const;

	/**
	 * \brief Gets/sets the PandaFramework.
	 * @return A reference to the PandaFramework.
	 */
	PandaFramework* const pandaFramework() const;

	/**
	 * \brief Gets/sets the WindowFramework.
	 * @return A reference to the WindowFramework.
	 */
	WindowFramework* const windowFramework() const;

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:
	///Parameter table.
	ParameterTable mParameterTable;
	///The PandaFramework .
	PandaFramework* mPandaFramework;
	///The WindowFramework .
	WindowFramework* mWindowFramework;

#ifdef ELY_THREAD
	///The mutex associated with this Template.
	ReMutex mMutex;
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedWritableReferenceCount::init_type();
		register_type(_type_handle, "ComponentTemplate",
				TypedWritableReferenceCount::get_class_type());
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

inline PandaFramework* const ComponentTemplate::pandaFramework() const
{
	return mPandaFramework;
}

inline WindowFramework* const ComponentTemplate::windowFramework() const
{
	return mWindowFramework;
}

inline ParameterTable ComponentTemplate::getParameterTable() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mParameterTable;
}

#ifdef ELY_THREAD
inline ReMutex& ComponentTemplate::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* COMPONENT_H_ */
