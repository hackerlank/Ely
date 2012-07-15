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
 * \file /Ely/src/SceneComponents/NodePathWrapper.cpp
 *
 * \date 28/giu/2012 (20:16:04)
 * \author marco
 */

#include "SceneComponents/NodePathWrapper.h"

NodePathWrapper::NodePathWrapper()
{
	// TODO Auto-generated constructor stub
}

NodePathWrapper::NodePathWrapper(NodePathWrapperTemplate* tmpl) :
		mTmpl(tmpl)
{
}

NodePathWrapper::~NodePathWrapper()
{
	// TODO Auto-generated destructor stub
}

const ComponentFamilyType NodePathWrapper::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType NodePathWrapper::componentType() const
{
	return mTmpl->componentType();
}

bool NodePathWrapper::initialize()
{
	bool result = true;
	//setup the wrapped NodePath
	std::string wrappedNodePath = mTmpl->parameter(std::string("nodepath"));
	if (wrappedNodePath == std::string("render"))
	{
		mNodePath = mTmpl->windowFramework()->get_render();
	}
	else if (wrappedNodePath == std::string("render2d"))
	{
		mNodePath = mTmpl->windowFramework()->get_render_2d();
	}
	else if (wrappedNodePath == std::string("aspect2d"))
	{
		mNodePath = mTmpl->windowFramework()->get_aspect_2d();
	}
	else if (wrappedNodePath == std::string("camera"))
	{
		mNodePath = mTmpl->windowFramework()->get_camera_group();
	}
	else
	{
		result = false;
	}
	return result;
}

void NodePathWrapper::onAddToObjectSetup()
{
	//set the node path of the object to the
	//node path of this NodePathWrapper
	mOwnerObject->nodePath() = mNodePath;
}

NodePath& NodePathWrapper::nodePath()
{
	return mNodePath;
}

NodePathWrapper::operator NodePath()
{
	return mNodePath;
}

//TypedObject semantics: hardcoded
TypeHandle NodePathWrapper::_type_handle;

