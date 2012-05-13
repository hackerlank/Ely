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
 * \file /Ely/src/Game/ComponentTemplateManager.cpp
 *
 * \date 11/mag/2012 (17:48:53)
 * \author marco
 */

#include "Game/ComponentTemplateManager.h"

PT(ComponentTemplate) ComponentTemplateManager::addComponentTemplate(
		ComponentTemplate* componentTmpl)
{
	if (not componentTmpl)
	{
		throw GameException(
				"ComponentTemplateManager::addComponentTemplate: NULL Component template");
	}
	PT(ComponentTemplate) previousCompTmpl(NULL);
	ComponentId componentId = componentTmpl->componentID();
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

bool ComponentTemplateManager::removeComponentTemplate(ComponentId componentID)
{
	ComponentTemplateTable::iterator it = mComponentTemplates.find(componentID);
	if (it == mComponentTemplates.end())
	{
		return false;
	}
	mComponentTemplates.erase(it);
	return true;
}

ComponentTemplate* ComponentTemplateManager::getComponentTemplate(
		ComponentId componentID)
{
	ComponentTemplateTable::iterator it = mComponentTemplates.find(componentID);
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	return (*it).second;
}

Component* ComponentTemplateManager::createComponent(ComponentId componentID)
{
	ComponentTemplateTable::iterator it = mComponentTemplates.find(componentID);
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	return (*it).second->makeComponent();
}
