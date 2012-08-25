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
 * \file /Ely/src/ObjectModel/ComponentTemplateManager.cpp
 *
 * \date 11/mag/2012 (17:48:53)
 * \author marco
 */

#include "ObjectModel/ComponentTemplateManager.h"

ComponentTemplateManager::ComponentTemplateManager()
{
}

ComponentTemplateManager::~ComponentTemplateManager()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//remove component templates
	while (mComponentTemplates.size() > 0)
	{
		ComponentTemplateTable::iterator iter = mComponentTemplates.begin();
		ComponentType compType = iter->first;
		PRINT(
				"Removing component template for type '" << std::string(compType) << "'");
		mComponentTemplates.erase(iter);
	}
	std::cout << std::endl;
}

PT(ComponentTemplate)ComponentTemplateManager::addComponentTemplate(
		ComponentTemplate* componentTmpl)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not componentTmpl)
	{
		throw GameException(
				"ComponentTemplateManager::addComponentTemplate: NULL Component template");
	}
	PT(ComponentTemplate) previousCompTmpl;
	previousCompTmpl.clear();
	ComponentType componentId = componentTmpl->componentType();
	ComponentTemplateTable::iterator it = mComponentTemplates.find(componentId);
	if (it != mComponentTemplates.end())
	{
		// a previous component template for that component already existed
		previousCompTmpl = (*it).second;
		mComponentTemplates.erase(it);
	}
	//insert the new component template
	mComponentTemplates[componentId] = PT(ComponentTemplate)(componentTmpl);
	return previousCompTmpl;
}

bool ComponentTemplateManager::removeComponentTemplate(
		ComponentType componentID)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ComponentTemplateTable::iterator it = mComponentTemplates.find(componentID);
	if (it == mComponentTemplates.end())
	{
		return false;
	}
	mComponentTemplates.erase(it);
	return true;
}

ComponentTemplate* ComponentTemplateManager::getComponentTemplate(
		ComponentType componentType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ComponentTemplateTable::iterator it = mComponentTemplates.find(
			componentType);
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	return (*it).second;
}

Component* ComponentTemplateManager::createComponent(
		ComponentType componentType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ComponentTemplateTable::iterator it = mComponentTemplates.find(
			componentType);
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	//new unique id
	ComponentId newCompId = ComponentId(componentType) + ComponentId(getId());
	//create component
	Component* newComp = (*it).second->makeComponent(newCompId);
	return newComp;
}

void ComponentTemplateManager::resetComponentTemplatesParams()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ComponentTemplateTable::iterator iter;
	for (iter = mComponentTemplates.begin(); iter != mComponentTemplates.end();
			++iter)
	{
		iter->second->setParametersDefaults();
	}
}

ReMutex& ComponentTemplateManager::getMutex()
{
	return mMutex;
}

IdType ComponentTemplateManager::getId()
{
	return ++id;
}
