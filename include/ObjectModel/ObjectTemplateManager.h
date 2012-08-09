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
 * \author marco
 */

#ifndef OBJECTTEMPLATEMANAGER_H_
#define OBJECTTEMPLATEMANAGER_H_

#include <map>
#include <iostream>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "Utilities/Tools.h"
#include "ObjectTemplate.h"
#include "Object.h"
#include "ComponentTemplateManager.h"

/**
 * \brief Singleton template manager that stores all the object templates.
 *
 * This manager takes responsibility to create game objects and maintains
 * a table of (pointers to) created objects, indexed by ObjectId.
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
	 * @return PT(NULL) if there wasn't a template for that object, otherwise
	 * the previous template.
	 */
	PT(ObjectTemplate) addObjectTemplate(ObjectTemplate* objectTmpl);

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
	ObjectTemplate* getObjectTemplate(ObjectType objectType);

	/**
	 * \brief Creates a object given its type and a NodePath.
	 *
	 * The type is needed to select the correct template.
	 * @param objectType The object type.
	 * @return The just created object, or NULL if the object cannot be created.
	 */
	Object* createObject(ObjectType objectType, ObjectId objectId = ObjectId(""));

	/**
	 * \brief Object templates and object tables typedefs.
	 */
	///@{
	typedef std::map<const ObjectType, PT(ObjectTemplate)> ObjectTemplateTable;
	typedef std::map<ObjectId, PT(Object)> ObjectTable;
	///@}

	/**
	 * \brief Gets a created object give its object id.
	 * @return A pointer to the created object (NULL otherwise).
	 */
	Object* getCreatedObject(const ObjectId& objectId);

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

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
	IdType getId();

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* OBJECTTEMPLATEMANAGER_H_ */
