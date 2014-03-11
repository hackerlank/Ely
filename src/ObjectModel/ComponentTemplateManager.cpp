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
 * \author consultit
 */

#include "ObjectModel/ComponentTemplateManager.h"

namespace ely
{

ComponentTemplateManager::ComponentTemplateManager()
{
}

ComponentTemplateManager::~ComponentTemplateManager()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//remove component templates
	while (mComponentTemplates.size() > 0)
	{
		ComponentTemplateTable::iterator iter = mComponentTemplates.begin();
		ComponentType compType = iter->first;
		removeComponentTemplate(compType);
	}
	std::cout << std::endl;
}

SMARTPTR(ComponentTemplate) ComponentTemplateManager::addComponentTemplate(
		SMARTPTR(ComponentTemplate) componentTmpl)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (not componentTmpl)
	{
		throw GameException(
				"ComponentTemplateManager::addComponentTemplate: NULL Component template");
	}
	SMARTPTR(ComponentTemplate) previousCompTmpl;
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
	mComponentTemplates[componentId] = componentTmpl;
	return previousCompTmpl;
}

bool ComponentTemplateManager::removeComponentTemplate(
		ComponentType componentType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ComponentTemplateTable::iterator it = mComponentTemplates.find(componentType);
	if (it == mComponentTemplates.end())
	{
		return false;
	}
	PRINT_DEBUG(
			"Removing component template for type '" << componentType << "'");
	mComponentTemplates.erase(it);
	return true;
}

SMARTPTR(ComponentTemplate) ComponentTemplateManager::getComponentTemplate(
		ComponentType componentType) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ComponentTemplateTable::const_iterator it = mComponentTemplates.find(
			componentType);
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	return (*it).second;
}

SMARTPTR(Component) ComponentTemplateManager::doCreateComponent(
		ComponentType componentType, bool freeComponent)
{
	ComponentTemplateTable::iterator it = mComponentTemplates.find(
			componentType);
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	//new unique id
	ComponentId newCompId = ComponentId(componentType) + ComponentId(getId());
	//create component
	SMARTPTR(Component) newComp = (*it).second->makeComponent(newCompId);
	newComp->setFreeFlag(freeComponent);
	return newComp;
}

void ComponentTemplateManager::resetComponentTemplateParams(ComponentType componentID)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ComponentTemplateTable::const_iterator iter = mComponentTemplates.find(
			componentID);
	if(iter != mComponentTemplates.end())
	{
		iter->second->setParametersDefaults();
	}
}

void ComponentTemplateManager::resetComponentTemplatesParams()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ComponentTemplateTable::iterator iter;
	for (iter = mComponentTemplates.begin(); iter != mComponentTemplates.end();
			++iter)
	{
		iter->second->setParametersDefaults();
	}
}

} // namespace ely
