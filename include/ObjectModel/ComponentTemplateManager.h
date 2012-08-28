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
 * \file /Ely/include/ObjectModel/ComponentTemplateManager.h
 *
 * \date 11/mag/2012 (17:48:53)
 * \author marco
 */

#ifndef COMPONENTTEMPLATEMANAGER_H_
#define COMPONENTTEMPLATEMANAGER_H_

#include <map>
#include <pointerTo.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "Utilities/Tools.h"
#include "ComponentTemplate.h"
#include "Component.h"

/**
 * \brief Singleton template manager that stores all the component templates.
 *
 * Thread-safe during utilization.
 */
class ComponentTemplateManager: public Singleton<ComponentTemplateManager>
{
public:
	/**
	 * \brief Constructor.
	 */
	ComponentTemplateManager();
	/**
	 * \brief Destructor.
	 */
	~ComponentTemplateManager();

	/**
	 * \brief Adds a component template for a given the component type it can create.
	 *
	 * It will add the component template to the internal table and if a
	 * template for that component type already existed it'll be replaced
	 * by this new template (and its ownership released by the manager).
	 * @param componentTmpl The component template to add.
	 * @return SMARTPTR(NULL) if there wasn't a template for that component, otherwise
	 * the previous template.
	 */
	SMARTPTR(ComponentTemplate) addComponentTemplate(
			SMARTPTR(ComponentTemplate) componentTmpl);

	/**
	 * \brief Removes the component template given the component type it can create.
	 * @param componentID The component type.
	 * @return True if the component template existed, false otherwise.
	 */
	bool removeComponentTemplate(ComponentType componentID);

	/**
	 * \brief Gets the component template given the component type it can create.
	 * @param componentID The component type.
	 * @return The component template.
	 */
	SMARTPTR(ComponentTemplate) getComponentTemplate(ComponentType componentID);

	/**
	 * \brief Resets all component templates to their default parameters.
	 */
	void resetComponentTemplatesParams();

	/**
	 * \brief Creates a component given its type.
	 * @param componentID The component type.
	 * @return The just created component, or NULL on failure (for any reason).
	 */
	SMARTPTR(Component) createComponent(ComponentType componentID);

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

private:

	///Table of component templates indexed by component type.
	typedef std::map<const ComponentType, SMARTPTR(ComponentTemplate)> ComponentTemplateTable;
	ComponentTemplateTable mComponentTemplates;
	///The unique id for created components.
	IdType id;
	/**
	 * \brief Returns an unique id for created components.
	 * @return
	 */
	IdType getId();

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* COMPONENTTEMPLATEMANAGER_H_ */
