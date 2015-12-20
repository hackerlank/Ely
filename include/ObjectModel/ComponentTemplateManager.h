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
 * \date 2012-05-11 
 * \author consultit
 */

#ifndef COMPONENTTEMPLATEMANAGER_H_
#define COMPONENTTEMPLATEMANAGER_H_

#include "Utilities/Tools.h"
#include "Component.h"

namespace ely
{
/**
 * \brief Singleton manager that handles all the ComponentTemplates.
 *
 * Thread-safe during utilization.
 */
class ComponentTemplateManager: public Singleton<ComponentTemplateManager>
{
private:
	friend class ObjectTemplateManager;

	/**
	 * \brief Creates a Component given its type.
	 * @param compType The Component type.
	 * @param freeComponent If this is a free Component or if it belongs
	 * to an Object Template, default is false.
	 * @return The just created Component, or NULL on failure (for any reason).
	 */
	SMARTPTR(Component) doCreateComponent(ComponentType compType,
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
	 * \brief Adds a ComponentTemplate for a the Component type it can create.
	 *
	 * It will add the Component Template to the internal table and if a
	 * Template for that Component type already existed it'll be replaced
	 * by this new Template (and its ownership released by the manager).
	 * @param componentTmpl The Component Template to add.
	 * @return NULL if there wasn't a Template for that Component, otherwise
	 * the previous Template.
	 */
	SMARTPTR(ComponentTemplate)addComponentTemplate(
			SMARTPTR(ComponentTemplate) componentTmpl);

	/**
	 * \brief Removes the ComponentTemplate given the Component type it can create.
	 * @param compType The Component type.
	 * @return True if the Component Template existed, false otherwise.
	 */
	bool removeComponentTemplate(ComponentType compType);

	/**
	 * \brief Gets the ComponentTemplate given the Component type it can create.
	 * @param compType The Component type.
	 * @return The Component Template.
	 */
	SMARTPTR(ComponentTemplate) getComponentTemplate(ComponentType compType) const;

	/**
	 * \brief Resets the parameters of the given Component type, to their default parameters.
	 *
	 * Component configuration parameters are handled by the corresponding ComponentTemplate.
	 * @param compType The Component type.
	 */
	void resetComponentTemplateParams(ComponentType compType);

	/**
	 * \brief Resets all ComponentTemplates to their default parameters.
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
