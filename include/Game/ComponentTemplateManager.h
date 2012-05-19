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
 * \file /Ely/include/Game/ComponentTemplateManager.h
 *
 * \date 11/mag/2012 (17:48:53)
 * \author marco
 */

#ifndef COMPONENTTEMPLATEMANAGER_H_
#define COMPONENTTEMPLATEMANAGER_H_

#include <map>
#include <pointerTo.h>
#include "Utilities/Tools.h"
#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Component.h"

/**
 * \brief Singleton template manager that stores all the component templates.
 *
 * Not multi-threaded.
 */
class ComponentTemplateManager: public Singleton<ComponentTemplateManager>
{
public:
	/**
	 * \brief Add a component template for a given the component type it can create.
	 *
	 * It will add the component template to the internal table and if a
	 * template for that component type already existed it'll be replaced
	 * by this new template (and its ownership released by the manager).
	 * @param componentTmpl The component template to add.
	 * @return PT(NULL) if there wasn't a template for that component, otherwise
	 * the previous template.
	 */
	PT(ComponentTemplate) addComponentTemplate(ComponentTemplate* componentTmpl);

	/**
	 * \brief Remove the component template given the component type it can create.
	 * @param componentID The component type.
	 * @return True if the component template existed, false otherwise.
	 */
	bool removeComponentTemplate(ComponentId componentID);

	/**
	 * \brief Get the component template given the component type it can create.
	 * @param componentID The component type.
	 * @return The component template.
	 */
	ComponentTemplate* getComponentTemplate(ComponentId componentID);

	/**
	 * \brief Create a component given its type.
	 * @param componentID The component type.
	 * @return The just created component, or NULL on failure (for any reason).
	 */
	Component* createComponent(ComponentId componentID);

private:
	///Table of component templates indexed by component type.
	typedef std::map<const ComponentId, PT(ComponentTemplate)> ComponentTemplateTable;
	ComponentTemplateTable mComponentTemplates;

};

#endif /* COMPONENTTEMPLATEMANAGER_H_ */
