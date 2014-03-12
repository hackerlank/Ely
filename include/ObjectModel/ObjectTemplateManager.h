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
#include "ObjectTemplate.h"
#include "Object.h"

namespace ely
{
/**
 * \brief Singleton template manager that stores all the object templates.
 *
 * This manager takes responsibility to create game objects and maintains
 * a table of (pointers to) created objects, indexed by ObjectId.\n
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
	 * \brief Adds an object template for a given object type it can create.
	 *
	 * It will add the object template to the internal table and if a
	 * template for that object type already existed it'll be replaced
	 * by this new template (and its ownership released by the manager).
	 * @param objectTmpl The object template to add.
	 * @return SMARTPTR(NULL) if there wasn't a template for that object, otherwise
	 * the previous template.
	 */
	SMARTPTR(ObjectTemplate)addObjectTemplate(SMARTPTR(ObjectTemplate) objectTmpl);

	/**
	 * \brief Removes the object template given the object type it can create.
	 * @param objectType The object type.
	 * @return True if the object template existed, false otherwise.
	 */
	bool removeObjectTemplate(ObjectType objectType);

	/**
	 * \brief Gets the object template given the object type it can create.
	 * @param objectType The object type.
	 * @return The object template.
	 */
	SMARTPTR(ObjectTemplate) getObjectTemplate(ObjectType objectType) const;

	/**
	 * \brief Creates a object given its type and a NodePath.
	 *
	 * The type is needed to select the correct template.\n
	 * The object creation can be made "ONLY" by calling this function.
	 *
	 * @param objectType The object type.
	 * @param objectId The object id.
	 * @param objTmplParams Object template parameter table used to setup the
	 * object in the scene.
	 * @param compTmplParams Map of component  templates' parameter tables,
	 * indexed by component type, used to initialize the object components.
	 * @param storeParams If to store object and component templates'
	 * parameters into the created object  and before it will be
	 * added to the scene (and initialized).
	 * @return The just created object, or NULL if the object cannot be created.
	 */
	SMARTPTR(Object) createObject(ObjectType objectType, ObjectId objectId = ObjectId(""),
			const ParameterTable& objTmplParams = ParameterTable(),
			const ParameterTableMap& compTmplParams = ParameterTableMap(),
			bool storeParams = false, SMARTPTR(Object) owner = NULL);

	/**
	 * \brief Adds a "free" component of the given type to an existing object with
	 * the given object identifier.\n
	 * \note you can add any component of any given type, even if the object template
	 * doesn't contain it.\n
	 * \note the new component will replace a component of the same family if any.
	 * \note event types of this new component need not to be the same as the
	 * declared ones in Object template if any.
	 * @param objectId The given object identifier.
	 * @param componentType The given component type.
	 * @param compTmplParams Map of component  templates' parameter tables,
	 * indexed by component type, used to initialize the object components.
	 * @return True if successfully added, false otherwise.
	 */
	bool addComponentToObject(ObjectId objectId, ComponentType componentType,
			const ParameterTable& compTmplParams = ParameterTable());

	/**
	 * \brief Object templates and object tables typedefs.
	 */
	///@{
	typedef std::map<const ObjectType, SMARTPTR(ObjectTemplate)> ObjectTemplateTable;
	typedef std::map<ObjectId, SMARTPTR(Object)> ObjectTable;
	///@}

	/**
	 * \brief Gets a created object give its object id.
	 * @return A pointer to the created object (NULL on error).
	 */
	SMARTPTR(Object) getCreatedObject(const ObjectId& objectId) const;

	/**
	 * \brief Gets a list of all created objects.
	 * @return A list of pointers to each created object.
	 */
	std::list<SMARTPTR(Object)> getCreatedObjects() const;

	/**
	 * \brief Destroys a created object give its object id.
	 * @return True if successful, false otherwise.
	 */
	bool destroyObject(const ObjectId& objectId);

	/**
	 * \brief Destroys all created objects.
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
	///Table of object templates indexed by their name.
	ObjectTemplateTable mObjectTemplates;
	/// The table of created game objects.
	ObjectTable mCreatedObjects;

	///The unique id for created objects.
	IdType id;
	/**
	 * \brief Return an unique id for created objects.
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
