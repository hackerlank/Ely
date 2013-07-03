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
 * \author consultit
 */

#include "SceneComponents/NodePathWrapper.h"
#include "SceneComponents/NodePathWrapperTemplate.h"
#include "ObjectModel/Object.h"
#include "Game/GameSceneManager.h"

namespace ely
{

NodePathWrapper::NodePathWrapper()
{
	// TODO Auto-generated constructor stub
}

NodePathWrapper::NodePathWrapper(SMARTPTR(NodePathWrapperTemplate)tmpl)
{
	CHECKEXISTENCE(GameSceneManager::GetSingletonPtr(),
			"NodePathWrapper::NodePathWrapper: invalid GameSceneManager")
	mTmpl = tmpl;
}

NodePathWrapper::~NodePathWrapper()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath.remove_node();
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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//setup the wrapped NodePath
	mWrappedNodePath = mTmpl->parameter(std::string("nodepath"));
	//
	return result;
}

void NodePathWrapper::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//setup the wrapped NodePath
	if (mWrappedNodePath == std::string("render"))
	{
		mNodePath = mTmpl->windowFramework()->get_render();
	}
	else if (mWrappedNodePath == std::string("render2d"))
	{
		mNodePath = mTmpl->windowFramework()->get_render_2d();
	}
	else if (mWrappedNodePath == std::string("aspect2d"))
	{
		mNodePath = mTmpl->windowFramework()->get_aspect_2d();
	}
	else if (mWrappedNodePath == std::string("camera"))
	{
		mNodePath = mTmpl->windowFramework()->get_camera_group();
	}
	else
	{
		//default is render
		mNodePath = mTmpl->windowFramework()->get_render();
	}

	//set the node path of the object to the
	//node path of this NodePathWrapper
	mOwnerObject->setNodePath(mNodePath);
	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

NodePath NodePathWrapper::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void NodePathWrapper::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

//TypedObject semantics: hardcoded
TypeHandle NodePathWrapper::_type_handle;

} // namespace ely
