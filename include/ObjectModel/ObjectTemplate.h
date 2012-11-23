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
 * \file /Ely/include/ObjectModel/ObjectTemplate.h
 *
 * \date 12/mag/2012 (19:34:28)
 * \author marco
 */

#ifndef OBJECTTEMPLATE_H_
#define OBJECTTEMPLATE_H_

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <utility>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <typedWritableReferenceCount.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "ComponentTemplate.h"
#include "Component.h"
#include "Utilities/Tools.h"

class ObjectTemplateManager;

/**
 * \brief Object type.
 *
 * This type identifies the name of template that create these type of objects.
 */
typedef std::string ObjectType;

/**
 * \brief Class storing all of objects templates used to create an object.
 */
class ObjectTemplate: public TypedWritableReferenceCount
{
public:
	/**
	 * \brief Type used for the ordered list of the component templates.
	 */
	typedef std::vector<SMARTPTR(ComponentTemplate)> ComponentTemplateList;

	/**
	 * \brief Constructor.
	 * @param name The name of this template.
	 * @param objectTmplMgr The ObjectTemplateManager.
	 * @param pandaFramework The PandaFramework.
	 * @param windowFramework The WindowFramework.
	 */
	ObjectTemplate(const ObjectType& name, ObjectTemplateManager* objectTmplMgr,
			PandaFramework* pandaFramework, WindowFramework* windowFramework);

	/**
	 * \brief Destructor.
	 */
	virtual ~ObjectTemplate();

	/**
	 * \brief Clears the table of all component templates of this
	 * object template.
	 */
	void clearComponentTemplates();

	/**
	 * \brief Gets a reference to the name (i.e. the object type) of
	 * this object template.
	 * @return The name of this object template.
	 */
	const ObjectType& name() const;

	/**
	 * \brief Gets the component template list.
	 * @return The component template list.
	 */
	ComponentTemplateList getComponentTemplates();

	/**
	 * \brief Adds a component template.
	 * @param componentTmpl The component template.
	 */
	void addComponentTemplate(SMARTPTR(ComponentTemplate) componentTmpl);

	/**
	 * \brief Gets a component template given the component type it can create.
	 * @param componentType The component type.
	 * @return The component template, NULL if it doesn't exist.
	 */
	SMARTPTR(ComponentTemplate) getComponentTemplate(const ComponentType&componentType);

	/**
	 * \name Parameters management.
	 * \brief Sets the parameters of the object, this template is
	 * designed to create, to custom values.
	 *
	 * These parameters overwrite (and/or are added to) the parameters defaults
	 * set by setParametersDefaults.
	 * @param parameterTable The table of (parameter,value).
	 */
	void setParameters(const ParameterTable& parameterTable);

	/**
	 * \brief For the object this template is designed to create,
	 * this function sets the (mandatory) parameters to their default values.
	 */
	void setParametersDefaults();

	/**
	 * \brief Gets the parameter value associated to the object.
	 * @param paramName The name of the parameter.
	 * @return The value of the parameter, empty string if none exists.
	 */
	std::string parameter(const std::string& paramName);
	/**
	 * \brief Gets the parameter multi-values associated to the object.
	 * @param paramName The name of the parameter.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	std::list<std::string> parameterList(const std::string& paramName);

	/**
	 * \brief Gets the entire parameter table.
	 * @return The parameter table.
	 */
	ParameterTable getParameterTable();

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

	/**
	 * \brief Adds a common attribute for a given component type of
	 * this object.
	 * @param parameterName The parameter name.
	 * @param parameterValue The parameter value.
	 * @param componentType The component type the parameter is related to.
	 */
	void addComponentParameter(const std::string& parameterName,
			const std::string& parameterValue, ComponentType compType);

	/**
	 * \brief Checks if a name/value pair is an allowed parameter/value
	 * for a given component type of this object.
	 * @param name The name to check.
	 * @param value The value to check.
	 * @param componentType The component type.
	 * @return True if the name/value pair match an allowed parameter/value
	 * for a given component, false otherwise.
	 */
	bool isComponentParameter(const std::string& name, const std::string& value,
			ComponentType compType);

	/**
	 * \brief Gets the parameter multi-values associated with the parameter
	 * associated to the component type of the object.
	 * @param paramName The name of the parameter.
	 * @param componentType The component type.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	std::list<std::string> componentParameterList(const std::string& paramName,
			ComponentType compType);

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

private:
	///Name identifying this object template.
	ObjectType mName;
	///Ordered list of all component templates.
	ComponentTemplateList mComponentTemplates;
	///The ObjectTemplateManager.
	ObjectTemplateManager* const mObjectTmplMgr;
	///Parameter table
	ParameterTable mParameterTable;
	///The PandaFramework.
	PandaFramework* mPandaFramework;
	///The WindowFramework.
	WindowFramework* mWindowFramework;

	///The table of ParameterTables indexed by component type.\n
	///These represent the allowed common attributes shared by each
	///component of a given type, belonging to any object of a given type.
	std::map<ComponentType, ParameterTable> mComponentParameterTables;

	///The (reentrant) mutex associated with this template.
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
		register_type(_type_handle, "ObjectTemplate",
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

struct idIsEqualTo
{
	idIsEqualTo(const ComponentType& compType) :
			mComponentType(compType)
	{
	}
	ComponentType mComponentType;
	bool operator()(const SMARTPTR(ComponentTemplate)componentTmpl)
	{
		return componentTmpl.p()->componentType() == mComponentType;
	}
};

#endif /* OBJECTTEMPLATE_H_ */
