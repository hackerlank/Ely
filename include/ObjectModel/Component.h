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

#include <referenceCount.h>
#include <pointerTo.h>
#include <typedObject.h>

#include <string>

/**
 * \brief Component identifier type.
 */
typedef std::string ComponentId;
/**
 * \brief Component family identifier type.
 */
typedef std::string ComponentFamilyId;

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
class Component: public TypedObject, public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	Component();

	/**
	 * \brief Get the id of this component.
	 * @return The id of this component.
	 */
	virtual const ComponentId componentID() const = 0;
	/**
	 * \brief Get the family id of this component.
	 * @return The family id of this component.
	 */
	virtual const ComponentFamilyId familyID() const = 0;

	/**
	 * \brief Updates the state of the component.
	 */
	virtual void update();

	/**
	 * \brief Allow a component to be initialized.
	 *
	 * This can be done after creation but "before" insertion into an object.
	 */
	virtual void initialize();

	/**
	 * \brief Get a reference to the owner object.
	 * \return The owner object.
	 */
	Object*& ownerObject();

protected:
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