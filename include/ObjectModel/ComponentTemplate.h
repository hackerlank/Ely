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
 * \file /Ely/include/ObjectModel/ComponentTemplate.h
 *
 * \date 11/mag/2012 (13:09:41)
 * \author marco
 */

#ifndef COMPONENTTEMPLATE_H_
#define COMPONENTTEMPLATE_H_

#include <referenceCount.h>
#include <typedObject.h>
#include "Object.h"
#include "Component.h"

/**
 * \brief Abstract base class of component templates used to create components.
 */
class ComponentTemplate: public TypedObject, public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	ComponentTemplate();

	/**
	 * \brief Get the id of the component created.
	 * @return The id of the component created.
	 */
	virtual const ComponentType componentType() const = 0;
	/**
	 * \brief Get the family id of the component created.
	 * @return The family id of the component created.
	 */
	virtual const ComponentFamilyType familyType() const = 0;

	/**
	 * \brief Create the actual component of that family.
	 * @return The component just created, NULL if component cannot be created.
	 */
	virtual Component* makeComponent(ComponentId& compId) = 0;

	/**
	 * \brief (Re)set this component template's members to their default values.
	 */
	virtual void reset() = 0;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedObject::init_type();
		register_type(_type_handle, "ComponentTemplate", TypedObject::get_class_type());
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

#endif /* COMPONENTTEMPLATE_H_ */
