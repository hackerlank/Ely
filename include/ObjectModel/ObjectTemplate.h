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
#include <list>
#include <algorithm>
#include <utility>
#include <referenceCount.h>
#include <typedWritable.h>
#include "ComponentTemplate.h"
#include "Component.h"
#include "ObjectTemplateManager.h"
#include "Utilities/Tools.h"

/**
 * \brief Object type.
 *
 * This type identifies the name of template that create these type of objects.
 */
typedef std::string ObjectType;

/**
 * \brief Class storing all of objects templates used to create an object.
 */
class ObjectTemplate: public TypedWritable, public ReferenceCount
{
public:
	/**
	 * \brief Type used for the list of the component templates.
	 */
	typedef std::list<ComponentTemplate*> ComponentTemplateList;

	/**
	 * \brief Constructor.
	 * @param name The name of this template.
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
	ObjectType& name();

	/**
	 * \brief Gets the component template list.
	 * @return The component template list.
	 */
	ComponentTemplateList& getComponentTemplates();

	/**
	 * \brief Adds a component template.
	 * @param componentTmpl The component template.
	 */
	void addComponentTemplate(ComponentTemplate* componentTmpl);

	/**
	 * \brief Gets a component template given the component type it can create.
	 * @param componentId The component type.
	 * @return The component template, NULL if it doesn't exist.
	 */
	ComponentTemplate* getComponentTemplate(const ComponentType& componentId);

	/**
	 * \brief Gets/sets the ObjectTemplateManager.
	 * @return A reference to the ObjectTemplateManager.
	 */
	ObjectTemplateManager*& objectTmplMgr();

	/**
	 * \brief Gets/sets the PandaFramework.
	 * @return A reference to the PandaFramework.
	 */
	PandaFramework*& pandaFramework();

	/**
	 * \brief Gets/sets the WindowFramework.
	 * @return A reference to the WindowFramework.
	 */
	WindowFramework*& windowFramework();

	/**
	 * \name Parameters management.
	 * \brief Sets the parameters of the object, this object is
	 * designed to create, to custom values.
	 * @param parameterTable The table of (parameter,value).
	 */
	void setParameters(ParameterTable& parameterTable);

	/**
	 * \brief (Re)sets the parameters of the object, this template is
	 * designed to create, to their default values.
	 */
	void resetParameters();

	/**
	 * \name Parameters getters/setters.
	 * \brief Get/set a single parameter associated to the object.
	 * @param The name of the parameter.
	 * @return The value of the parameter.
	 */
	///@{
	std::string& parameter(const std::string& paramName);
	std::list<std::string>& parameterList(const std::string& paramName);
	///@}

private:
	///Name identifying this object template.
	ObjectType mName;
	///List of all component templates.
	ComponentTemplateList mComponentTemplates;
	///The ObjectTemplateManager.
	ObjectTemplateManager* mObjectTmplMgr;
	///The PandaFramework.
	PandaFramework* mPandaFramework;
	///The WindowFramework.
	WindowFramework* mWindowFramework;
	///@{
	/// Set of allowed Parameters.
	std::string mParent, mIsStatic, mPosX, mPosY, mPosZ, mRotH, mRotP, mRotR,
			mScaleX, mScaleY, mScaleZ, mUnknown;
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
	bool operator()(const ComponentTemplate* componentTmpl)
	{
		return componentTmpl->componentType() == mComponentType;
	}
};

#endif /* OBJECTTEMPLATE_H_ */
