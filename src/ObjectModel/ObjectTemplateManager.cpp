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
 * \file /Ely/src/ObjectModel/ObjectTemplateManager.cpp
 *
 * \date 13/mag/2012 (10:16:20)
 * \author consultit
 */

#include "ObjectModel/ObjectTemplateManager.h"
#include "ObjectModel/ComponentTemplateManager.h"

namespace ely
{

ObjectTemplateManager::ObjectTemplateManager()
{
}

ObjectTemplateManager::~ObjectTemplateManager()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//remove created objects (if any)
	while (mCreatedObjects.size() > 0)
	{
		ObjectTable::iterator iter = mCreatedObjects.begin();
		ObjectId objId = iter->first;
		destroyObject(objId);
	}
	//remove Object templates
	while (mObjectTemplates.size() > 0)
	{
		ObjectTemplateTable::iterator iter = mObjectTemplates.begin();
		ObjectType objType = iter->first;
		removeObjectTemplate(objType);
	}
	std::cout << std::endl;
}

SMARTPTR(ObjectTemplate)ObjectTemplateManager::addObjectTemplate(
		SMARTPTR(ObjectTemplate) objectTmpl)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (not objectTmpl)
	{
		throw GameException(
				"ObjectTemplateManager::addObjectTemplate: NULL Component template");
	}
	SMARTPTR(ObjectTemplate) previousObjTmpl;
	previousObjTmpl.clear();
	ObjectType objectTemplId = objectTmpl->objectType();
	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectTemplId);
	if (it != mObjectTemplates.end())
	{
		// a previous Component template for that Component already existed
		previousObjTmpl = (*it).second;
		mObjectTemplates.erase(it);
	}
	//insert the new Component template
	mObjectTemplates[objectTemplId] = objectTmpl;
	return previousObjTmpl;

}

bool ObjectTemplateManager::removeObjectTemplate(ObjectType objectType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectType);
	RETURN_ON_COND(it == mObjectTemplates.end(), false)

	PRINT_DEBUG( "Removing object template for type '" << objectType << "'");
	mObjectTemplates.erase(it);
	return true;
}

SMARTPTR(ObjectTemplate)ObjectTemplateManager::getObjectTemplate(ObjectType objectType) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ObjectTemplateTable::const_iterator it = mObjectTemplates.find(objectType);
	RETURN_ON_COND(it == mObjectTemplates.end(), NULL)

	return (*it).second;
}

SMARTPTR(Object)ObjectTemplateManager::createObject(ObjectType objectType,
		ObjectId objectId,
		const ParameterTable& objectParams,
		const ParameterTableMap& componentsParams,
		bool storeParams,
		SMARTPTR(Object)owner)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//check if it is an already created Object
	SMARTPTR(Object) oldObject = getCreatedObject(objectId);
	RETURN_ON_COND(oldObject, oldObject)

	//retrieve the ObjectTemplate
	ObjectTemplateTable::iterator it1 = mObjectTemplates.find(objectType);
	RETURN_ON_COND(it1 == mObjectTemplates.end(), NULL)

	SMARTPTR(ObjectTemplate) objectTmpl = (*it1).second;
	//create the new Object
	ObjectId newId;
	if (objectId == ObjectId(""))
	{
		newId = ObjectId(objectType) + ObjectId(doGetId());
	}
	else
	{
		newId = objectId;
	}
	//create the new Object
	SMARTPTR(Object) newObj = new Object(newId, objectTmpl);
	//set the owner if any
	newObj->setOwner(owner);
	//get the ComponentTemplate ordered list
	ObjectTemplate::ComponentTemplateList compTmplList =
	objectTmpl->getComponentTemplates();
	//iterate in order over the ordered list and add Components in the same order
	for (unsigned int idx2 = 0; idx2 < compTmplList.size(); ++idx2)
	{
		//use ComponentTemplateManager to create Component
		ComponentType compType = compTmplList[idx2]->componentType();
		//we have to initialize parameters of this Component
		//set Component' parameters to their default values
		ComponentTemplateManager::GetSingleton().resetComponentTemplateParams(compType);
		//set parameters for this Component...
		ParameterTableMap::const_iterator it3;
		it3 = componentsParams.find(std::string(compType));
		if (it3 != componentsParams.end())
		{
			//...if not empty
			compTmplList[idx2]->setParameters(it3->second);
		}
		//create the Component
		SMARTPTR(Component) newComp =
		ComponentTemplateManager::GetSingleton().doCreateComponent(
				compType);
		//return NULL on error
		RETURN_ON_COND(not newComp, NULL)

		//add the new Component into the Object if there
		//isn't another one with the same family type
		if (not newObj->doAddComponent(newComp))
		{
			continue;
		}
		//set the Component owner Object
		newComp->setOwnerObject(newObj);
		//give a chance to Component to setup itself when being added to Object.
		newComp->addToObjectSetup();
	}
	//we have to initialize parameters of this Object template
	//initialize parameters' Object template to defaults
	objectTmpl->setParametersDefaults();
	//set parameters for this Object template...
	if(not objectParams.empty())
	{
		//...if not empty
		objectTmpl->setParameters(objectParams);
	}
	//store Object and Components' parameters into
	//the created Object and before it will be added to the
	//scene (and initialized).
	bool storeParamsParam = (
			objectTmpl->parameter(std::string("store_params")) ==
			std::string("true") ? true : false);
	if (storeParams or storeParamsParam)
	{
		newObj->doStoreParameters(objectParams, componentsParams);
	}
	//give a chance to Object to setup itself when being added to scene.
	newObj->onAddToSceneSetup();
	Object::FamilyTypeComponentList::const_iterator compIter;

#ifdef ELY_THREAD
	//Lock all components during this code execution.
	for (compIter = newObj->doGetComponents().begin(); compIter != newObj->doGetComponents().end();
			++compIter)
	{
		compIter->second->getMutex().acquire();
	}
	try
	{
#endif //ELY_THREAD

		//give a chance to components to setup themselves when being
		//added to scene in order of insertion. Thread safe.
		for (compIter = newObj->doGetComponents().begin(); compIter != newObj->doGetComponents().end();
				++compIter)
		{
			compIter->second->addToSceneSetup();
		}

#ifdef ELY_THREAD
	}
	catch (...)
	{
		//Unlock all components.
		for (compIter = newObj->doGetComponents().begin(); compIter != newObj->doGetComponents().end();
				++compIter)
		{
			compIter->second->getMutex().release();
		}
	}
	//Unlock all components.
	for (compIter = newObj->doGetComponents().begin(); compIter != newObj->doGetComponents().end();
			++compIter)
	{
		compIter->second->getMutex().release();
	}
#endif //ELY_THREAD

	//Now the Object is completely existent so insert
	//it in the table of created objects.
	mCreatedObjects[newId] = newObj;
	return newObj;
}

bool ObjectTemplateManager::destroyObject(const ObjectId& objectId)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ObjectTable::iterator objectIter;
	objectIter = mCreatedObjects.find(objectId);
	RETURN_ON_COND(objectIter == mCreatedObjects.end(), false)

	PRINT_DEBUG( "Removing object '" << std::string(objectId) << "'");
	SMARTPTR(Object) object = objectIter->second;
	Object::FamilyTypeComponentList::const_reverse_iterator compRIter;
	//get a copy of Object components because original will be modified
	//and to avoid dropping of reference count
	Object::FamilyTypeComponentList objectComponents = object->doGetComponents();
	//on removal from scene components cleanup in reverse order of insertion
	for (compRIter = objectComponents.rbegin();
			compRIter != objectComponents.rend(); ++compRIter)
	{
		compRIter->second->removeFromSceneCleanup();
	}

#ifdef ELY_THREAD
	//Lock all components during this code execution.
	for (compRIter = objectComponents.rbegin();
			compRIter != objectComponents.rend(); ++compRIter)
	{
		compRIter->second->getMutex().acquire();
	}
	try
	{
#endif //ELY_THREAD

		//lock (guard) the Object mutex
		HOLD_REMUTEX(object->getMutex())

		//on removal from scene Object cleanup
		object->onRemoveFromSceneCleanup();
		//remove components in reverse order of insertion
		for (compRIter = objectComponents.rbegin();
				compRIter != objectComponents.rend(); ++compRIter)
		{
			//on removal from Object Component cleanup
			compRIter->second->removeFromObjectCleanup();
			//set the old Component owner to NULL
			compRIter->second->setOwnerObject(NULL);
			//remove old Component from Object
			object->doRemoveComponent(compRIter->second);
		}
		//on Object removal cleanup
		object->onRemoveObjectCleanup();

#ifdef ELY_THREAD
	}
	catch (...)
	{
		//Unlock all components.
		for (compRIter = objectComponents.rbegin();
				compRIter != objectComponents.rend(); ++compRIter)
		{
			compRIter->second->getMutex().release();
		}
	}
	//Unlock all components.
	for (compRIter = objectComponents.rbegin();
			compRIter != objectComponents.rend(); ++compRIter)
	{
		compRIter->second->getMutex().release();
	}
#endif //ELY_THREAD

	//remove Object from the table of created objects.
	mCreatedObjects.erase(objectIter);
	return true;
}

void ObjectTemplateManager::destroyAllObjects()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//get not owned objects
	ObjectTable::iterator iter;
	std::vector<ObjectId> notOwnedObjectIds;
	for (iter = mCreatedObjects.begin(); iter != mCreatedObjects.end(); ++iter)
	{
		if (not iter->second->getOwner())
		{
			//Object has no owner: insert id
			notOwnedObjectIds.push_back(iter->first);
		}
	}
	//remove not owned objects
	for (unsigned int i = 0; i < notOwnedObjectIds.size(); ++i)
	{
		destroyObject(notOwnedObjectIds[i]);
	}
	//remove any remaining (zombie) objects: should be none
	while (mCreatedObjects.size() > 0)
	{
		ObjectTable::iterator iter = mCreatedObjects.begin();
		ObjectId objId = iter->first;
		destroyObject(objId);
	}
}

bool ObjectTemplateManager::addComponentToObject(ObjectId objectId,
		ComponentType compType, const ParameterTable& componentParams)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//check if it is effectively an already created Object
	SMARTPTR(Object)object = getCreatedObject(objectId);
	RETURN_ON_COND(not object, false)

	//get the ComponentTemplate
	SMARTPTR(ComponentTemplate)componentTemplate =
	ComponentTemplateManager::GetSingleton().getComponentTemplate(compType);
	//we have to initialize parameters of this Component
	//set Component' parameters to their default values
	componentTemplate->setParametersDefaults();
	//set parameters for this Component...
	if (not componentParams.empty())
	{
		//...if not empty
		componentTemplate->setParameters(componentParams);
	}
	//create the new free Component
	SMARTPTR(Component)newComp =
	ComponentTemplateManager::GetSingleton().doCreateComponent(compType, true);
	RETURN_ON_COND(not newComp, false)

	ComponentFamilyType compFamilyType = newComp->componentFamilyType();
	//check if there are an old Component
	SMARTPTR(Component)oldComp = object->getComponent(compFamilyType);
	if (oldComp)
	{
		//on removal from scene old Component cleanup
		oldComp->removeFromSceneCleanup();

		//lock (guard) the old Component mutex
		HOLD_REMUTEX(oldComp->getMutex())
		{
			//lock (guard) the new Component mutex
			HOLD_REMUTEX(newComp->getMutex())
			{
				//lock (guard) the Object mutex
				HOLD_REMUTEX(object->getMutex())
				//on removal from Object old Component cleanup
				oldComp->removeFromObjectCleanup();
				//set the old Component owner to NULL
				oldComp->setOwnerObject(NULL);

				//remove old Component from Object
				object->doRemoveComponent(oldComp);

				//add the new Component to the Object
				object->doAddComponent(newComp);
				//set the new Component owner
				newComp->setOwnerObject(object);
				//on addition to Object new Component setup
				newComp->addToObjectSetup();
				//on addition to scene new Component setup
				newComp->addToSceneSetup();
			}
		}
		//
		return true;
	}
	{
		//lock (guard) the new Component mutex
		HOLD_REMUTEX(newComp->getMutex())
		{
			//lock (guard) the Object mutex
			HOLD_REMUTEX(object->getMutex())

			//add the new Component to the Object
			object->doAddComponent(newComp);
			//set the new Component owner
			newComp->setOwnerObject(object);
			//on addition to Object new Component setup
			newComp->addToObjectSetup();
			//on addition to scene new Component setup
			newComp->addToSceneSetup();
		}
	}
	//
	return true;
}

bool ObjectTemplateManager::removeComponentFromObject(ObjectId objectId,
		ComponentType compType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//check if it is effectively an already created Object
	SMARTPTR(Object)object = getCreatedObject(objectId);
	RETURN_ON_COND(not object, false)

	//get the Component by type
	SMARTPTR(Component)oldComp = object->getComponent(compType);
	//return false if there's not such a Component
	RETURN_ON_COND(not oldComp, false)

	//return false if this Object is designed to have a Component of this family type
	ComponentFamilyType compFamilyType = oldComp->componentFamilyType();
	RETURN_ON_COND(object->objectTmpl()->getComponentTemplate(compFamilyType),
			false)

	//remove actually the Component:
	//on removal from scene old Component cleanup
	oldComp->removeFromSceneCleanup();
	{
		//lock (guard) the old Component mutex
		HOLD_REMUTEX(oldComp->getMutex())
		{
			//lock (guard) the Object mutex
			HOLD_REMUTEX(object->getMutex())

			//on removal from object old Component cleanup
			oldComp->removeFromObjectCleanup();
			//set the old Component owner to NULL
			oldComp->setOwnerObject(NULL);

			//remove old Component from Object
			object->doRemoveComponent(oldComp);
		}
	}
	//
	return true;
}

SMARTPTR(Object)ObjectTemplateManager::getCreatedObject(const ObjectId& objectId) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ObjectTable::const_iterator iterObj;
	iterObj = mCreatedObjects.find(objectId);
	RETURN_ON_COND(iterObj == mCreatedObjects.end(), NULL)

	return iterObj->second;
}

std::list<SMARTPTR(Object)> ObjectTemplateManager::getCreatedObjects() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(Object)> createdObjects;
	ObjectTable::const_iterator iterObj;
	for(iterObj = mCreatedObjects.begin();
			iterObj!= mCreatedObjects.end();++iterObj)
	{
		createdObjects.push_back(iterObj->second);
	}
	return createdObjects;
}

IdType ObjectTemplateManager::doGetId()
{
	return ++id;
}

} // namespace ely
