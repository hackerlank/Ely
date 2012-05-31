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
 * \file /Ely/src/InputComponents/ControlByEvent.cpp
 *
 * \date 31/mag/2012 (16:42:14)
 * \author marco
 */

#include "InputComponents/ControlByEvent.h"

ControlByEvent::ControlByEvent()
{
	// TODO Auto-generated constructor stub

}

ControlByEvent::ControlByEvent(ControlByEventTemplate* tmpl)
{
}

ControlByEvent::~ControlByEvent()
{
	// TODO Auto-generated destructor stub
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEvent::_type_handle;

const ComponentFamilyType ControlByEvent::familyType() const
{
	return ComponentFamilyType("Input");
}

const ComponentType ControlByEvent::componentType() const
{
	return ComponentType("ControlByEvent");
}

void ControlByEvent::update()
{
}

bool ControlByEvent::initialize()
{
	return true;
}

void ControlByEvent::onAddSetup()
{
}

