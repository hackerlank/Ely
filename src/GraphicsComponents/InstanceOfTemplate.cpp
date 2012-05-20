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
 * \file /Ely/src/GraphicsComponents/InstanceOfTemplate.cpp
 *
 * \date 20/mag/2012 (09:40:59)
 * \author marco
 */

#include "GraphicsComponents/InstanceOfTemplate.h"
#include "GraphicsComponents/InstanceOf.h"

InstanceOfTemplate::InstanceOfTemplate()
{
	mInitPosition = LVecBase3(0.0, 0.0, 0.0);
	mInitOrientation = LVecBase3(0.0, 0.0, 0.0);
	mInitScaling = LVecBase3(1.0, 1.0, 1.0);
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
	return ComponentFamilyType("Graphics");
}

Component* InstanceOfTemplate::makeComponent(ComponentId& compId)
{
	InstanceOf* newInstanceOf = new InstanceOf(this);
	newInstanceOf->componentId() = compId;
	if (not newInstanceOf->initialize())
	{
		return NULL;
	}
	return newInstanceOf;
}

void InstanceOfTemplate::reset()
{
	mInitPosition = LVecBase3(0.0, 0.0, 0.0);
	mInitOrientation = LVecBase3(0.0, 0.0, 0.0);
	mInitScaling = LVecBase3(1.0, 1.0, 1.0);
}

LVecBase3& InstanceOfTemplate::initOrientation()
{
	return mInitOrientation;
}

LVecBase3& InstanceOfTemplate::initPosition()
{
	return mInitPosition;
}

LVecBase3& InstanceOfTemplate::initScaling()
{
	return mInitScaling;
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOfTemplate::_type_handle;

