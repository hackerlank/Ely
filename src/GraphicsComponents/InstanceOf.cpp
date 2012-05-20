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
 * \file /Ely/src/GraphicsComponents/InstanceOf.cpp
 *
 * \date 20/mag/2012 (09:42:38)
 * \author marco
 */

#include "GraphicsComponents/InstanceOf.h"
#include "GraphicsComponents/InstanceOfTemplate.h"

InstanceOf::InstanceOf()
{
	// TODO Auto-generated constructor stub

}

InstanceOf::InstanceOf(InstanceOfTemplate* tmpl) :
		mTmpl(tmpl)
{
}

InstanceOf::~InstanceOf()
{
	mNodePath.remove_node();
}

const ComponentFamilyType InstanceOf::familyType() const
{
	return ComponentFamilyType("Graphics");
}

const ComponentType InstanceOf::componentType() const
{
	return ComponentType("InstanceOf");
}

void InstanceOf::update()
{
}

bool InstanceOf::initialize()
{
	//setup initial state
	mNodePath = NodePath(mComponentId);
	mNodePath.set_scale(mTmpl->initScaling());
	mNodePath.set_pos(mTmpl->initPosition());
	mNodePath.set_hpr(mTmpl->initOrientation());
	return true;
}

NodePath& InstanceOf::nodePath()
{
	return mNodePath;
}

InstanceOf::operator NodePath()
{
	return mNodePath;
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOf::_type_handle;
