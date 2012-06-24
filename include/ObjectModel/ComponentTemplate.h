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
#include <referenceCount.h>
#include <typedWritable.h>
#include "Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Abstract base class of component templates used to create components.
 */
class ComponentTemplate: public TypedWritable, public ReferenceCount
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
	virtual Component* makeComponent(ComponentId& compId) = 0;

	/**
	 * \name Parameters management.
	 * \brief Sets the parameters of the component, this template is
	 * designed to create, to custom values.
	 * @param parameterTable The table of (parameter,value).
	 */
	virtual void setParameters(ParameterTable& parameterTable);

	/**
	 * \brief (Re)sets the parameters of the component, this template is
	 * designed to create, to their default values.
	 */
	virtual void resetParameters();

	/**
	 * \brief Get/set the parameters associated to the object.
	 * @param The name of the parameter.
	 * @return The value of the parameter.
	 */
	///@{
	virtual std::string& parameter(const std::string& paramName);
	virtual std::list<std::string>& parameterList(const std::string& paramName);
	///@}

protected:
	///@{
	/// Set of allowed Parameters.
	std::string mUnknown;
	std::list<std::string> mUnknownList;
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
