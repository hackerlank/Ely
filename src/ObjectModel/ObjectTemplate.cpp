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
 * \file /Ely/src/ObjectModel/ObjectTemplate.cpp
 *
 * \date 12/mag/2012 (19:34:28)
 * \author marco
 */

#include "ObjectModel/ObjectTemplate.h"

ObjectTemplate::ObjectTemplate(const ObjectType& name) :
		mName(name)
{
}

ObjectTemplate::~ObjectTemplate()
{
	// TODO Auto-generated destructor stub
}

ObjectType& ObjectTemplate::name()
{
	return mName;
}

void ObjectTemplate::clearComponentTemplates()
{
	mComponentTemplates.clear();
}

ObjectTemplate::ComponentTemplateList& ObjectTemplate::getComponentTemplates()
{
	return mComponentTemplates;
}

void ObjectTemplate::addComponentTemplate(ComponentTemplate* componentTmpl)
{
	if (not componentTmpl)
	{
		throw GameException(
				"ObjectTemplate::addComponentTemplate: NULL component template");
	}
	mComponentTemplates.push_back(componentTmpl);
}

ComponentTemplate* ObjectTemplate::getComponentTemplate(
		const ComponentType& componentId)
{
	ComponentTemplateList::iterator it;
	it = find_if(mComponentTemplates.begin(), mComponentTemplates.end(),
			idIsEqualTo(componentId));
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	return *it;
}

//TypedObject semantics: hardcoded
TypeHandle ObjectTemplate::_type_handle;
