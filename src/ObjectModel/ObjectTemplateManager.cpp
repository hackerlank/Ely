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
 * \author marco
 */

#include "ObjectModel/ObjectTemplateManager.h"

ObjectTemplateManager::ObjectTemplateManager()
{
}

ObjectTemplateManager::~ObjectTemplateManager()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//remove created objects
	while (mCreatedObjects.size() > 0)
	{
		ObjectTable::iterator iter = mCreatedObjects.begin();
		ObjectId objId = iter->first;
		PRINT( "Removing object '" << std::string(objId) << "'");
		//remove first object components because they
		//have back references to their owner object
		iter->second->clearComponents();
		//
		mCreatedObjects.erase(iter);
	}
	//remove object templates
	while (mObjectTemplates.size() > 0)
	{
		ObjectTemplateTable::iterator iter = mObjectTemplates.begin();
		ObjectType objType = iter->first;
		PRINT(
				"Removing object template for type '" << std::string(objType) << "'");
		mObjectTemplates.erase(iter);
	}
	std::cout << std::endl;
}

SMARTPTR(ObjectTemplate)ObjectTemplateManager::addObjectTemplate(
		SMARTPTR(ObjectTemplate) objectTmpl)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not objectTmpl)
	{
		throw GameException(
				"ObjectTemplateManager::addObjectTemplate: NULL Component template");
	}
	SMARTPTR(ObjectTemplate) previousObjTmpl;
	previousObjTmpl.clear();
	ObjectType objectTemplId = objectTmpl->name();
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
	HOLDMUTEX(mMutex)

	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectType);
	if (it == mObjectTemplates.end())
	{
		return false;
	}
	mObjectTemplates.erase(it);
	return true;
}

SMARTPTR(ObjectTemplate)ObjectTemplateManager::getObjectTemplate(ObjectType objectType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectType);
	if (it == mObjectTemplates.end())
	{
		return NULL;
	}
	return (*it).second;
}

SMARTPTR(Object)ObjectTemplateManager::createObject(ObjectType objectType,
		ObjectId objectId,
		bool createWithParamTables,
		const ParameterTable& objTmplParams,
		const ParameterTableMap& compTmplParams)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
		newId = ObjectId(objectType) + ObjectId(getId());
	}
	else
	{
		newId = objectId;
	}
	SMARTPTR(Object) newObj = new Object(newId, objectTmpl);
	//get the component template list
	ObjectTemplate::ComponentTemplateList compTmplList =
	objectTmpl->getComponentTemplates();
	//iterate in order over the ordered list and assign components
	for (unsigned int idx2 = 0; idx2 < compTmplList.size(); ++idx2)
	{
		//use ComponentTemplateManager to create component
		ComponentType compType = compTmplList[idx2].p()->componentType();
		//check if we have to initialize parameters of this component template
		if(createWithParamTables)
		{
			//set component' parameters to their default values
			compTmplList[idx2].p()->setParametersDefaults();
			//set parameters for this component template...
			ParameterTableMap::const_iterator it3;
			it3 = compTmplParams.find(std::string(compType));
			if (it3 != compTmplParams.end())
			{
				//...if not empty
				compTmplList[idx2].p()->setParameters(it3->second);
				PRINT( "    Initializing Component '" << std::string(compType) << "'");
			}
		}
		//
		SMARTPTR(Component) newComp =
		ComponentTemplateManager::GetSingleton().createComponent(
				compType);
		//add the component into the object
		if (not newComp)
		{
			return NULL;
		}
		newObj->addComponent(newComp);
	}
	//check if we have to initialize parameters of this object template
	if(createWithParamTables)
	{
		//initialize parameters' object template to defaults
		objectTmpl->setParametersDefaults();
		//set parameters for this object template...
		if(not objTmplParams.empty())
		{
			//...if not empty
			objectTmpl->setParameters(objTmplParams);
		}
		//give a chance to object (and its components) to customize
		//themselves when being added to scene.
		newObj->sceneSetup();
	}
	//Now the object is completely existent so insert
	//it in the table of created objects.
	mCreatedObjects[newId] = newObj;
	return newObj;
}

SMARTPTR(Object)ObjectTemplateManager::getCreatedObject(const ObjectId& objectId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ObjectTable::iterator iterObj;
	iterObj = mCreatedObjects.find(objectId);
	if (iterObj == mCreatedObjects.end())
	{
		return NULL;
	}
	return iterObj->second;
}

bool ObjectTemplateManager::removeCreatedObject(const ObjectId& objectId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ObjectTable::iterator iterObj;
	iterObj = mCreatedObjects.find(objectId);
	if (iterObj == mCreatedObjects.end())
	{
		return false;
	}
	PRINT( "Removing object '" << std::string(objectId) << "'");
	//remove first object components because they
	//have back references to their owner object
	iterObj->second->clearComponents();
	//
	mCreatedObjects.erase(iterObj);
	return true;
}

ReMutex& ObjectTemplateManager::getMutex()
{
	return mMutex;
}

IdType ObjectTemplateManager::getId()
{
	return ++id;
}
