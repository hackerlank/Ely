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
	setParametersDefaults();
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

void InstanceOfTemplate::setParametersDefaults()
{
	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("scale_x", "1.0"));
	mParameterTable.insert(ParameterNameValue("scale_y", "1.0"));
	mParameterTable.insert(ParameterNameValue("scale_z", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOfTemplate::_type_handle;

