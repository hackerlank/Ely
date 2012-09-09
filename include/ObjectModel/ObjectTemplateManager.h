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
	SMARTPTR(ObjectTemplate) getObjectTemplate(ObjectType objectType);

	/**
	 * \brief Creates a object given its type and a NodePath.
	 *
	 * The type is needed to select the correct template.\n
	 * The object creation process can be made in two different ways:
	 * -# single thread way:
	 * 	- the initial parameters' values of each component, are setup
	 * 	on each component template (through ComponentTemplate::setParameters())
	 * 	- the object and its components are created (through this function)
	 * 	- the initial parameters' values of the object, are setup
	 * 	on the object template (through ObjectTemplate::setParameters())
	 * 	- Object::sceneSetup() is called to give a chance to object 
	 * 	(and its components) to customize themselves when being added to scene.
	 * -# multi threads way:
	 * 	- all the operations shown in the single threads way are performed 
	 * 	through this function, by passing it object template and components'
	 * 	templates parameters directly.
	 * The third parameter indicates that parameters for the object template
	 * and for component templates (fourth and fifth parameters) are passed
	 * directly to this function, in fact choosing between the "single thread"
	 * or "multi-threads" object creation ways: to create an object in a
	 * "multi-threads" environment, this parameter should be set true and
	 * objTmplParams and compTmplParams should be set to initialization
	 * parameters of object and components respectively. By default the
	 * creation process is "single thread".
	 *
	 * \note this distinction between these two creation way is needed
	 * because both object templates and component templates are shared
	 * resource.
	 *
	 * @param objectType The object type.
	 * @param objectId The object id.
	 * @param createWithParams Indicates if parameters for object and
	 * component templates are passed directly.
	 * @param objTmplParams Object template parameter table used to setup the
	 * object in the scene.
	 * @param compTmplParams Map of component  templates' parameter tables,
	 * indexed by component type, used to initialize the object components.
	 * @return The just created object, or NULL if the object cannot be created.
	 */
	SMARTPTR(Object) createObject(ObjectType objectType, ObjectId objectId = ObjectId(""),
			bool createWithParams = false,
			const ParameterTable& objTmplParams = ParameterTable(),
			const ParameterTableMap& compTmplParams = ParameterTableMap());

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
	SMARTPTR(Object) getCreatedObject(const ObjectId& objectId);

	/**
	 * \brief Removes a created object give its object id.
	 * @return A pointer to the just removed object (NULL on error).
	 */
	bool removeCreatedObject(const ObjectId& objectId);

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
