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
#include <set>
#include <pointerTo.h>
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
 * each family type.
 * Each component can be updated directly or indirectly through a
 * Panda task.
 * Each component can respond to Panda events (stimuli) by registering
 * or unregistering them with it. For any event called "<EVENT>" a
 * global std::string variable named "<EVENT>_<FAMILYTYPE>_<OBJECTID>"
 * is loaded at runtime from a dynamic linked library (referenced by the
 * macro HANDLERS_SO) and its value is the name of the function handler
 * of the same event (contained in the same library too). If this variable
 * doesn't exist or if any error occurs the default handler (referenced by
 * the macro DEFAULT_HANDLER_NAME) is used.
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
	void setOwnerObject(Object* ownerObject);
	Object* getOwnerObject() const;
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
	ComponentTemplate* mTmpl;
	///Unique identifier for this component.
	ComponentId mComponentId;
	///The object this component is a member of.
	Object* mOwnerObject;

	/**
	 * \name Events management.
	 */
	///@{
	///Set of events this component should respond to.
	std::set<std::string> mEventSet;
	///@{
	///Handles, typedefs, functions for managing event handlers libraries.
	LIB_HANDLE mHandlerLib;
	bool mHandlerLibLoaded;
	typedef EventHandler::EventCallbackFunction* PHANDLER;
	typedef std::string* PHANDLERNAME;
	///Table of handlers keyed by event names.
	std::map<std::string,PHANDLER> mHandlerTable;

	void loadEventHandlers();
	void unloadEventHandlers();
	///@}
	/**
	 * \brief Helper functions to setup register/unregister events this
	 * component should respond to.
	 *
	 * A handler routine called "<EVENT>_<OBJECTID>_<FAMILYTYPE>"
	 * should exist in a dynamic linked library referenced by HANDLERS_SO.
	 * If this handler doesn't exist or if any error occurs the default
	 * handler (contained in HANDLERS_DEFAULT_SO library) is used.
	 * These routines should be used only after the component has
	 * been added to n object.
	 */
	///@{
	void setupEvents();
	void registerEvents();
	void unregisterEvents();
	///@}
	///@}

	///The (reentrant) mutex associated with this component.
	ReMutex mMutex;

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
