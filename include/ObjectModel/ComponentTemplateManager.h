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
 * \author consultit
 */

#ifndef COMPONENTTEMPLATEMANAGER_H_
#define COMPONENTTEMPLATEMANAGER_H_

#include "Utilities/Tools.h"
#include "Component.h"

namespace ely
{
/**
 * \brief Singleton Template manager that stores all the Component templates.
 *
 * Thread-safe during utilization.
 */
class ComponentTemplateManager: public Singleton<ComponentTemplateManager>
{
private:
	friend class ObjectTemplateManager;

	/**
	 * \brief Creates a Component given its type.
	 * @param componentType The Component type.
	 * @param freeComponent If this is a free Component or if it belongs
	 * to an Object Template, default is false.
	 * @return The just created Component, or NULL on failure (for any reason).
	 */
	SMARTPTR(Component) doCreateComponent(ComponentType componentType,
			bool freeComponent = false);

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
	 * \brief Adds a Component Template for a given the Component type it can create.
	 *
	 * It will add the Component Template to the internal table and if a
	 * Template for that Component type already existed it'll be replaced
	 * by this new Template (and its ownership released by the manager).
	 * @param componentTmpl The Component Template to add.
	 * @return SMARTPTR(NULL) if there wasn't a Template for that Component, otherwise
	 * the previous Template.
	 */
	SMARTPTR(ComponentTemplate)addComponentTemplate(
			SMARTPTR(ComponentTemplate) componentTmpl);

	/**
	 * \brief Removes the Component Template given the Component type it can create.
	 * @param componentType The Component type.
	 * @return True if the Component Template existed, false otherwise.
	 */
	bool removeComponentTemplate(ComponentType componentType);

	/**
	 * \brief Gets the Component Template given the Component type it can create.
	 * @param componentType The Component type.
	 * @return The Component Template.
	 */
	SMARTPTR(ComponentTemplate) getComponentTemplate(ComponentType componentType) const;

	/**
	 * \brief Resets the Component Template, of the given Component type,
	 * to its default parameters.
	 *
	 * @param componentType The Component type.
	 */
	void resetComponentTemplateParams(ComponentType componentType);

	/**
	 * \brief Resets all Component templates to their default parameters.
	 */
	void resetComponentTemplatesParams();

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

private:

	///Table of Component templates indexed by Component type.
	typedef std::map<const ComponentType, SMARTPTR(ComponentTemplate)> ComponentTemplateTable;
	ComponentTemplateTable mComponentTemplates;
	///The unique id for created components.
	IdType id;
	/**
	 * \brief Returns an unique id for created components.
	 * @return
	 */
	IdType getId();

#ifdef ELY_THREAD
	///The mutex associated with this manager.
	ReMutex mMutex;
#endif
};

///inline definitions

inline IdType ComponentTemplateManager::getId()
{
	return ++id;
}

#ifdef ELY_THREAD
inline ReMutex& ComponentTemplateManager::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* COMPONENTTEMPLATEMANAGER_H_ */
