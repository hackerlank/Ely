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
 * \file /Ely/src/InputComponents/ControlByEventTemplate.cpp
 *
 * \date 31/mag/2012 (16:44:06)
 * \author marco
 */

#include "InputComponents/ControlByEventTemplate.h"

ControlByEventTemplate::ControlByEventTemplate()
{
	// TODO Auto-generated constructor stub

}

ControlByEventTemplate::~ControlByEventTemplate()
{
	// TODO Auto-generated destructor stub
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEventTemplate::_type_handle;

const ComponentType ControlByEventTemplate::componentType() const
{
	return ComponentType("ControlByEvent");
}

const ComponentFamilyType ControlByEventTemplate::familyType() const
{
	return ComponentFamilyType("Input");
}

Component* ControlByEventTemplate::makeComponent(ComponentId& compId)
{
	return NULL;
}

void ControlByEventTemplate::reset()
{
}

