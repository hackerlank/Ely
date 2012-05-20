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
#include "ComponentTemplate.h"
#include "Component.h"

/**
 * \brief Object template identifier type (this is
 * the type of the object the template can create).
 */
typedef std::string ObjectTemplateId;

/**
 * \brief Class storing all of objects templates used to create an object.
 */
class ObjectTemplate: public ReferenceCount
{
public:
	/**
	 * \brief Type used for the list of the component templates.
	 */
	typedef std::list<ComponentTemplate*> ComponentTemplateList;
	/**
	 * \brief Constructor.
	 * @param templateId The name of this template.
	 */
	ObjectTemplate(const ObjectTemplateId& name);
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
	 * \brief Get a reference to the name of this object template.
	 * @return The name of this object template.
	 */
	ObjectTemplateId& name();

	/**
	 * \brief Get the component template list.
	 * @return The component template list.
	 */
	ComponentTemplateList& getComponentTemplates();

	/**
	 * \brief Add a component template.
	 * @param componentTmpl The component template.
	 */
	void addComponentTemplate(ComponentTemplate* componentTmpl);
	/**
	 * \brief Get a component template given the component type it can create.
	 * @param componentId The component type.
	 * @return The component template, NULL if it doesn't exist.
	 */
	ComponentTemplate* getComponentTemplate(
			const ComponentType& componentId);

private:
	///Name identifying this object template.
	ObjectTemplateId mName;
	///List of all component templates.
	ComponentTemplateList mComponentTemplates;
};

struct idIsEqualTo
{
	idIsEqualTo(const ComponentType& compId) :
			mComponentID(compId)
	{
	}
	ComponentType mComponentID;
	bool operator()(const ComponentTemplate* componentTmpl)
	{
		return componentTmpl->componentType() == mComponentID;
	}
};

#endif /* OBJECTTEMPLATE_H_ */
