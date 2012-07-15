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

#include <list>
#include <set>
#include <string>
#include <utility>
#include <typedWritableReferenceCount.h>
#include "Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Abstract base class of component templates used to create components.
 */
class ComponentTemplate: public TypedWritableReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	ComponentTemplate();

	/**
	 * \brief Gets the id of the component created.
	 * @return The id of the component created.
	 */
	virtual const ComponentType componentType() const = 0;
	/**
	 * \brief Gets the family id of the component created.
	 * @return The family id of the component created.
	 */
	virtual const ComponentFamilyType familyType() const = 0;

	/**
	 * \brief Creates the actual component of that family.
	 * @return The component just created, NULL if component cannot be created.
	 */
	virtual Component* makeComponent(const ComponentId& compId) = 0;

	/**
	 * \name Parameters management.
	 * \brief Sets the parameters of the component, this template is
	 * designed to create, to custom values.
	 *
	 * These parameters overwrite (and/or are added to) the parameters defaults
	 * set by setParametersDefaults.
	 * @param parameterTable The table of (parameter,value).
	 */
	virtual void setParameters(ParameterTable& parameterTable);

	/**
	 * \brief For the component this template is designed to create,
	 * this function sets the (mandatory) parameters to their default values.
	 */
	virtual void setParametersDefaults() = 0;

	/**
	 * \brief Gets the parameter value associated to the object.
	 * @param The name of the parameter.
	 * @return The value of the parameter, empty string if none exists.
	 */
	virtual std::string parameter(const std::string& paramName);
	/**
	 * \brief Gets the parameter multi-values associated to the object.
	 * @param The name of the parameter.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	virtual std::list<std::string> parameterList(const std::string& paramName);

protected:
	///Parameter table
	ParameterTable mParameterTable;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedObject::init_type();
		register_type(_type_handle, "ComponentTemplate",
				TypedObject::get_class_type());
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
