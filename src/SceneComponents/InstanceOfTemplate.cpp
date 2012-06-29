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
 * \file /Ely/src/SceneComponents/InstanceOfTemplate.cpp
 *
 * \date 20/mag/2012 (09:40:59)
 * \author marco
 */

#include "SceneComponents/InstanceOfTemplate.h"

InstanceOfTemplate::InstanceOfTemplate()
{
	resetParameters();
}

InstanceOfTemplate::~InstanceOfTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType InstanceOfTemplate::componentType() const
{
	return ComponentType("InstanceOf");
}

const ComponentFamilyType InstanceOfTemplate::familyType() const
{
	return ComponentFamilyType("Scene");
}

Component* InstanceOfTemplate::makeComponent(const ComponentId& compId)
{
	InstanceOf* newInstanceOf = new InstanceOf(this);
	newInstanceOf->componentId() = compId;
	if (not newInstanceOf->initialize())
	{
		return NULL;
	}
	return newInstanceOf;
}

void InstanceOfTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTable::iterator iter;
	CASEITER(parameterTable,iter,"instance_of",mInstanceOf)
}

void InstanceOfTemplate::resetParameters()
{
	//set component parameters to their default values
	mInstanceOf = std::string("");
}

std::string& InstanceOfTemplate::parameter(const std::string& paramName)
{
	std::string* strPtr = &mUnknown;
	CASE(paramName,strPtr,"instance_of",mInstanceOf)
	//
	return *strPtr;
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOfTemplate::_type_handle;

