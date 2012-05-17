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
 * \file /Ely/src/Game/ObjectTemplateManager.cpp
 *
 * \date 13/mag/2012 (10:16:20)
 * \author marco
 */

#include "Game/ObjectTemplateManager.h"

ObjectTemplateManager::ObjectTemplateManager(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ObjectTemplateManager::ObjectTemplateManager: invalid PandaFramework or WindowFramework");
	}
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
}

PT(ObjectTemplate) ObjectTemplateManager::addObjectTemplate(
		ObjectTemplate* objectTmpl)
{
	if (not objectTmpl)
	{
		throw GameException(
				"ObjectTemplateManager::addObjectTemplate: NULL Component template");
	}
	PT(ObjectTemplate) previousObjTmpl(NULL);
	ObjectTemplateId objectTemplId = objectTmpl->name();
	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectTemplId);
	if (it != mObjectTemplates.end())
	{
		// a previous component template for that component already existed
		previousObjTmpl = (*it).second;
		mObjectTemplates.erase(it);
	}
	//insert the new component template
	mObjectTemplates[objectTemplId] = PT(ObjectTemplate)(objectTmpl);
	return previousObjTmpl;

}

bool ObjectTemplateManager::removeObjectTemplate(ObjectTemplateId objectType)
{
	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectType);
	if (it == mObjectTemplates.end())
	{
		return false;
	}
	mObjectTemplates.erase(it);
	return true;
}

ObjectTemplate* ObjectTemplateManager::getObjectTemplate(
		ObjectTemplateId objectType)
{
	ObjectTemplateTable::iterator it = mObjectTemplates.find(objectType);
	if (it == mObjectTemplates.end())
	{
		return NULL;
	}
	return (*it).second;
}

bool ObjectTemplateManager::readObjectTemplates(const std::string& filename)
{
	return true;
}

Object* ObjectTemplateManager::createObject(ObjectTemplateId objectType)
{
	//retrieve the ObjectTemplate
	ObjectTemplateTable::iterator it1 = mObjectTemplates.find(objectType);
	if (it1 == mObjectTemplates.end())
	{
		return false;
	}
	ObjectTemplate* objectTmpl = (*it1).second.p();
	//create the new object
	ObjectId newId = ObjectId(objectType) + ObjectId(getObjectId());
	Object* newObj = new Object(newId, mPandaFramework, mWindowFramework);
	//get the component template list
	ObjectTemplate::ComponentTemplateList compTmplList =
			objectTmpl->getComponentTemplates();
	//iterate over the list and assign components
	ObjectTemplate::ComponentTemplateList::iterator it2;
	for (it2 = compTmplList.begin(); it2 != compTmplList.end(); ++it2)
	{
		Component* newComp = (*it2)->makeComponent();
		//set component owner
		newComp->ownerObject() = newObj;
		//set the component into the object
		newObj->setComponent(newComp);
	}
	//
	return newObj;
}

IdType ObjectTemplateManager::getObjectId()
{
	return ++id;
}

PandaFramework*& ObjectTemplateManager::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& ObjectTemplateManager::windowFramework()
{
	return mWindowFramework;
}
