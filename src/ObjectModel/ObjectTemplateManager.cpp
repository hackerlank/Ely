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
	//remove object templates
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
		// a previous component template for that component already existed
		previousObjTmpl = (*it).second;
		mObjectTemplates.erase(it);
	}
	//insert the new component template
	mObjectTemplates[objectTemplId] = objectTmpl;
	return previousObjTmpl;

}

bool ObjectTemplateManager::removeObjectTemplate(ObjectType objectType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectType);
	if (it == mObjectTemplates.end())
	{
		return false;
	}
	PRINT_DEBUG( "Removing object template for type '" << objectType << "'");
	mObjectTemplates.erase(it);
	return true;
}

SMARTPTR(ObjectTemplate)ObjectTemplateManager::getObjectTemplate(ObjectType objectType) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ObjectTemplateTable::const_iterator it = mObjectTemplates.find(objectType);
	if (it == mObjectTemplates.end())
	{
		return NULL;
	}
	return (*it).second;
}

SMARTPTR(Object)ObjectTemplateManager::createObject(ObjectType objectType,
		ObjectId objectId,
		const ParameterTable& objTmplParams,
		const ParameterTableMap& compTmplParams,
		bool storeParams,
		SMARTPTR(Object)owner)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//check if it is an already created object
	SMARTPTR(Object) oldObject = getCreatedObject(objectId);
	if(oldObject)
	{
		return oldObject;
	}
	//retrieve the ObjectTemplate
	ObjectTemplateTable::iterator it1 = mObjectTemplates.find(objectType);
	if (it1 == mObjectTemplates.end())
	{
		return NULL;
	}
	SMARTPTR(ObjectTemplate) objectTmpl = (*it1).second;
	//create the new object
	ObjectId newId;
	if (objectId == ObjectId(""))
	{
		newId = ObjectId(objectType) + ObjectId(doGetId());
	}
	else
	{
		newId = objectId;
	}
	//create the new object
	SMARTPTR(Object) newObj = new Object(newId, objectTmpl);
	//set the owner if any
	newObj->setOwner(owner);
	//get the component template ordered list
	ObjectTemplate::ComponentTemplateList compTmplList =
	objectTmpl->getComponentTemplates();
	//iterate in order over the ordered list and add components in the same order
	for (unsigned int idx2 = 0; idx2 < compTmplList.size(); ++idx2)
	{
		//use ComponentTemplateManager to create component
		ComponentType compType = compTmplList[idx2]->componentType();
		//we have to initialize parameters of this component template
		//set component' parameters to their default values
		ComponentTemplateManager::GetSingleton().resetComponentTemplateParams(compType);
		//set parameters for this component template...
		ParameterTableMap::const_iterator it3;
		it3 = compTmplParams.find(std::string(compType));
		if (it3 != compTmplParams.end())
		{
			//...if not empty
			compTmplList[idx2]->setParameters(it3->second);
		}
		//create the component
		SMARTPTR(Component) newComp =
		ComponentTemplateManager::GetSingleton().doCreateComponent(
				compType);
		//add the component into the object
		if (not newComp)
		{
			return NULL;
		}
		//add the new component
		ComponentFamilyType familyId = newComp->familyType();
		newObj->doAddComponent(newComp, familyId);
		//set the component owner object
		newComp->setOwnerObject(newObj);
		//give a chance to component to setup itself when being added to object.
		newComp->addToObjectSetup();
	}
	//we have to initialize parameters of this object template
	//initialize parameters' object template to defaults
	objectTmpl->setParametersDefaults();
	//set parameters for this object template...
	if(not objTmplParams.empty())
	{
		//...if not empty
		objectTmpl->setParameters(objTmplParams);
	}
	//store object and component templates' parameters into
	//the created object and before it will be added to the
	//scene (and initialized).
	bool storeParamsParam = (
			objectTmpl->parameter(std::string("store_params")) ==
			std::string("true") ? true : false);
	if (storeParams or storeParamsParam)
	{
		newObj->storeParameters(objTmplParams, compTmplParams);
	}
	//give a chance to object to setup itself when being added to scene.
	newObj->onAddToSceneSetup();
	Object::ComponentOrderedList::const_iterator compIter;

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

	//Now the object is completely existent so insert
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
	if (objectIter == mCreatedObjects.end())
	{
		return false;
	}
	PRINT_DEBUG( "Removing object '" << std::string(objectId) << "'");
	SMARTPTR(Object) object = objectIter->second;
	Object::ComponentOrderedList::const_reverse_iterator compRIter;
	//get a copy of object components because original will be modified
	//and to avoid dropping of reference count
	Object::ComponentOrderedList objectComponents = object->doGetComponents();
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

		//lock (guard) the object mutex
		HOLD_REMUTEX(object->getMutex())

		//on removal from scene object cleanup
		object->onRemoveFromSceneCleanup();
		//remove components in reverse order of insertion
		for (compRIter = objectComponents.rbegin();
				compRIter != objectComponents.rend(); ++compRIter)
		{
			//on removal from object component cleanup
			compRIter->second->removeFromObjectCleanup();
			//set the old component owner to NULL
			compRIter->second->setOwnerObject(NULL);
			//remove old component from object
			object->doRemoveComponent(compRIter->second, compRIter->first);
		}
		//on object removal cleanup
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

	//remove object from the table of created objects.
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
			//object has no owner: insert id
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
		ComponentType componentType, const ParameterTable& compTmplParams)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//check if it is effectively an already created object
	SMARTPTR(Object) object = getCreatedObject(objectId);
	if(not object)
	{
		return false;
	}
	//get the component template
	SMARTPTR(ComponentTemplate) componentTemplate =
	ComponentTemplateManager::GetSingleton().getComponentTemplate(componentType);
	//we have to initialize parameters of this component template
	//set component' parameters to their default values
	componentTemplate->setParametersDefaults();
	//set parameters for this component template...
	if (not compTmplParams.empty())
	{
		//...if not empty
		componentTemplate->setParameters(compTmplParams);
	}
	//create the new component
	SMARTPTR(Component) newComp =
	ComponentTemplateManager::GetSingleton().doCreateComponent(componentType);
	if (not newComp)
	{
		return false;
	}
	ComponentFamilyType familyId = newComp->familyType();
	//check if there are an old component
	SMARTPTR(Component) oldComp = object->getComponent(familyId);
	if(oldComp)
	{
		//on removal from scene old component cleanup
		oldComp->removeFromSceneCleanup();

		//lock (guard) the old component mutex
		HOLD_REMUTEX(oldComp->getMutex())
		{
			//lock (guard) the new component mutex
			HOLD_REMUTEX(newComp->getMutex())
			{
				//lock (guard) the object mutex
				HOLD_REMUTEX(object->getMutex())
				{
					//on removal from object old component cleanup
					oldComp->removeFromObjectCleanup();
					//set the old component owner to NULL
					oldComp->setOwnerObject(NULL);

					//remove old component from object
					object->doRemoveComponent(oldComp, familyId);

					//add the new component to the object
					object->doAddComponent(newComp, familyId);
					//set the new component owner
					newComp->setOwnerObject(object);
					//on addition to object new component setup
					newComp->addToObjectSetup();
					//on addition to scene new component setup
					newComp->addToSceneSetup();
				}
			}
		}
		//
		return true;
	}
	{
		//lock (guard) the new component mutex
		HOLD_REMUTEX(newComp->getMutex())
		{
			//lock (guard) the object mutex
			HOLD_REMUTEX(object->getMutex())

			//add the new component to the object
			object->doAddComponent(newComp, familyId);
			//set the new component owner
			newComp->setOwnerObject(object);
			//on addition to object new component setup
			newComp->addToObjectSetup();
			//on addition to scene new component setup
			newComp->addToSceneSetup();
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
	if (iterObj == mCreatedObjects.end())
	{
		return NULL;
	}
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
