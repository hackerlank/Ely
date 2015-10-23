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
 * \file /Ely/include/ObjectModel/ObjectTemplateManager.h
 *
 * \date 13/mag/2012 (10:16:20)
 * \author consultit
 */

#ifndef OBJECTTEMPLATEMANAGER_H_
#define OBJECTTEMPLATEMANAGER_H_

#include "Utilities/Tools.h"
#include "Object.h"

namespace ely
{
/**
 * \brief Singleton manager that handles all the ObjectTemplates.
 *
 * This manager has the additional responsibility to create game Objects
 * and to maintain a table of (references to) all created Objects, indexed by ObjectId.\n
 * Thread-safe during utilization.
 */
class ObjectTemplateManager: public Singleton<ObjectTemplateManager>
{
public:
	/**
	 * \brief Constructor.
	 */
	ObjectTemplateManager();
	/**
	 * \brief Destructor.
	 */
	~ObjectTemplateManager();

	/**
	 * \brief Adds an ObjectTemplate for a given Object type it can create.
	 *
	 * It will add the ObjectTemplate to the internal table and, if a
	 * template for that Object type (i.e. having the same name) already existed,
	 * it'll be replaced by this new template (and its ownership released by the manager).
	 * @param objectTmpl The ObjectTemplate to add.
	 * @return NULL if there wasn't a template for that Object, otherwise
	 * the previous template.
	 */
	SMARTPTR(ObjectTemplate)addObjectTemplate(SMARTPTR(ObjectTemplate) objectTmpl);

	/**
	 * \brief Removes the ObjectTemplate given the Object type it can create.
	 * @param objectType The Object type.
	 * @return True if the ObjectTemplate existed, false otherwise.
	 */
	bool removeObjectTemplate(ObjectType objectType);

	/**
	 * \brief Gets the ObjectTemplate given the Object type it can create.
	 * @param objectType The Object type.
	 * @return The ObjectTemplate.
	 */
	SMARTPTR(ObjectTemplate) getObjectTemplate(ObjectType objectType) const;

	/**
	 * \brief Creates a game Object.
	 *
	 * The Object creation can be performed "ONLY" by calling this function.\n
	 * The type is needed to select the correct ObjectTemplate.\n
	 * @param objectType The Object type.
	 * @param objectId The Object identifier.
	 * @param objectParams Object parameter table used to setup the
	 * Object in the game scene.
	 * @param componentsParams Map of Components' parameter tables, indexed by
	 * Component type, used to initialize the owned Components of the Object.
	 * @param storeParams Whether to store Object and Components' parameters
	 * into the created Object and before it will be added to the game scene
	 * (and before being initialized).
	 * @param owner The owner Object
	 * @return The just created Object, or NULL if the Object cannot be created.
	 */
	SMARTPTR(Object) createObject(ObjectType objectType, ObjectId objectId = ObjectId(""),
			const ParameterTable& objectParams = ParameterTable(),
			const ParameterTableMap& componentsParams = ParameterTableMap(),
			bool storeParams = false, SMARTPTR(Object) owner = NULL);

	/**
	 * \brief Adds/replaces a Component of the given type to an existing Object with
	 * the given Object identifier.
	 *
	 * \note you can add any Component of any given type, even if the ObjectTemplate
	 * doesn't contain it.\n
	 * \note the new Component will replace a pre-existing Component of the same
	 * family type, if any.
	 * \note event types of this new Component need not to be the same as the
	 * declared ones in ObjectTemplate, if any.
	 * @param objectId The given Object identifier.
	 * @param componentType The given Component type.
	 * @param componentParams Parameters used to initialize the Component.
	 * @return True if successfully added, false otherwise.
	 */
	bool addComponentToObject(ObjectId objectId, ComponentType componentType,
			const ParameterTable& componentParams = ParameterTable());

	/**
	 * \brief Removes a Component, if any, with the given type, from an existing
	 * Object with the given identifier.
	 *
	 * \note the Component is removed only if the ObjectTemplate doesn't contain it.\n
	 * @param objectId The given Object identifier.
	 * @param componentType The given Component type.
	 * @return True if successfully removed, false otherwise.
	 */
	bool removeComponentFromObject(ObjectId objectId, ComponentType componentType);

	/**
	 * \brief ObjectTemplate and Object tables typedefs.
	 */
	///@{
	typedef std::map<const ObjectType, SMARTPTR(ObjectTemplate)> ObjectTemplateTable;
	typedef std::map<ObjectId, SMARTPTR(Object)> ObjectTable;
	///@}

	/**
	 * \brief Gets a created Object by its identifier.
	 * @return A reference to the created Object (NULL on error).
	 */
	SMARTPTR(Object) getCreatedObject(const ObjectId& objectId) const;

	/**
	 * \brief Gets a list of all created Objects.
	 * @return A list of references to the created Objects.
	 */
	std::list<SMARTPTR(Object)> getCreatedObjects() const;

	/**
	 * \brief Destroys a created Object by its identifier.
	 * @return True if successful, false otherwise.
	 */
	bool destroyObject(const ObjectId& objectId);

	/**
	 * \brief Destroys all created Objects.
	 */
	void destroyAllObjects();

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

private:
	///Table of ObjectTemplates indexed by their name.
	ObjectTemplateTable mObjectTemplates;
	/// The table of created game Objects.
	ObjectTable mCreatedObjects;

	///The unique identifier for created Objects.
	IdType id;
	/**
	 * \brief Return an unique identifier for created Objects.
	 * @return The unique id.
	 */
	IdType doGetId();

#ifdef ELY_THREAD
	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
#endif
};

///inline definitions

#ifdef ELY_THREAD
inline ReMutex& ObjectTemplateManager::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* OBJECTTEMPLATEMANAGER_H_ */
