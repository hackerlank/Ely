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
 * \file /Ely/include/Game/ObjectTemplateManager.h
 *
 * \date 13/mag/2012 (10:16:20)
 * \author marco
 */

#ifndef OBJECTTEMPLATEMANAGER_H_
#define OBJECTTEMPLATEMANAGER_H_

#include <map>
#include <string>
#include <sstream>
#include <nodePath.h>
#include <pointerTo.h>
#include "Utilitiy.h"
#include "ObjectModel/ObjectTemplate.h"
#include "ObjectModel/Object.h"
#include "ComponentTemplateManager.h"

/**
 * \brief Type of the generated object counter.
 */
struct IdType
{
	unsigned long int i;
	IdType() :
			i(0)
	{
	}
	IdType& operator ++()
	{
		++i;
		return *this;
	}
	operator std::string()
	{
		std::ostringstream oStringI;
		oStringI << i;
		return oStringI.str();
	}
};

/**
 * \brief Singleton template manager that stores all the object templates.
 *
 * Not multi-threaded.
 */
class ObjectTemplateManager: public Singleton<ObjectTemplateManager>
{
public:
	/**
	 * \brief Read the object templates definitions,suitably formatted (xml), from file.
	 * @param filename The name of the file.
	 * @return True if all ok, false otherwise.
	 */
	bool readObjectTemplates(const std::string& filename);

	/**
	 * \brief Add an object template for a given object type it can create.
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
	 * \brief Remove the object template given the object type it can create.
	 * @param objectID The object type.
	 * @return True if the object template existed, false otherwise.
	 */
	bool removeObjectTemplate(ObjectTemplateId objectType);

	/**
	 * \brief Get the object template given the object type it can create.
	 * @param objectID The object type.
	 * @return The object template.
	 */
	ObjectTemplate* getObjectTemplate(ObjectTemplateId objectType);

	/**
	 * \brief Create a object given its type and a NodePath.
	 *
	 * The type is needed to select the correct template.
	 * @param objectID The object type.
	 * @return The just created object, or NULL if the object cannot be created.
	 */
	Object* createObject(ObjectTemplateId objectType);

	/**
	 * \brief Return an unique id for created objects.
	 * @return
	 */
	IdType getObjectId();

private:
	///Table of object templates indexed by their name.
	typedef std::map<const ObjectTemplateId, PT(ObjectTemplate)> ObjectTemplateTable;
	ObjectTemplateTable mObjectTemplates;
	///The unique id for created objects.
	IdType id;
};

#endif /* OBJECTTEMPLATEMANAGER_H_ */
