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

#include <string>

#include <pointerTo.h>
#include <typedWritableReferenceCount.h>
#include <genericAsyncTask.h>

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

/**
 * \brief Base abstract class to provide a common interface for
 * all object components.
 *
 * Components are organized into class hierarchies called families.
 * Each component belongs to a family and is derived from a base
 * family component. Any object can have only one component of
 * each family type.
 */
class Component: public TypedWritableReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	Component();

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
	 * \brief Gets a reference to the owner object.
	 * \return The owner object.
	 */
	Object*& ownerObject();
	/**
	 * \brief Gets a reference to the component unique identifier.
	 * \return The component unique identifier.
	 */
	ComponentId& componentId();

protected:
	///Unique identifier for this component.
	ComponentId mComponentId;
	/// The object this component is a member of.
	Object* mOwnerObject;

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
