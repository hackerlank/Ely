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

ControlByEvent::ControlByEvent(ControlByEventTemplate* tmpl) :
		mTmpl(tmpl)
{
}

ControlByEvent::~ControlByEvent()
{
}

const ComponentFamilyType ControlByEvent::familyType() const
{
	return ComponentFamilyType("Input");
}

const ComponentType ControlByEvent::componentType() const
{
	return ComponentType("ControlByEvent");
}

bool ControlByEvent::initialize()
{
	bool result = true;
	return result;
}

void ControlByEvent::onAddSetup()
{
}

NodePath& ControlByEvent::nodePath()
{
	return mNodePath;
}

ControlByEvent::operator NodePath()
{
	return mNodePath;
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEvent::_type_handle;
