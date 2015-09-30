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
#include "ObjectModel/Object.h"
#include "Game/GameSceneManager.h"

namespace ely
{

NodePathWrapper::NodePathWrapper(SMARTPTR(NodePathWrapperTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameSceneManager::GetSingletonPtr(),
			"NodePathWrapper::NodePathWrapper: invalid GameSceneManager")

	mTmpl = tmpl;
	reset();
}

NodePathWrapper::~NodePathWrapper()
{
}

bool NodePathWrapper::initialize()
{
	bool result = true;
	//setup the wrapped NodePath
	mWrappedNodePathParam = mTmpl->parameter(std::string("nodepath"));
	//
	return result;
}

void NodePathWrapper::onAddToObjectSetup()
{
	//setup the wrapped NodePath
	if (mWrappedNodePathParam == std::string("render"))
	{
		mNodePath = mTmpl->windowFramework()->get_render();
	}
	else if (mWrappedNodePathParam == std::string("render2d"))
	{
		mNodePath = mTmpl->windowFramework()->get_render_2d();
	}
	else if (mWrappedNodePathParam == std::string("aspect2d"))
	{
		mNodePath = mTmpl->windowFramework()->get_aspect_2d();
	}
	else if (mWrappedNodePathParam == std::string("camera"))
	{
		mNodePath = mTmpl->windowFramework()->get_camera_group();
	}
	else
	{
		//default is render
		mNodePath = mTmpl->windowFramework()->get_render();
	}

	//set the object node path to this NodePathWrapper of node path
	mOldObjectNodePath = mOwnerObject->getNodePath();
	mOwnerObject->setNodePath(mNodePath);

	//clear all no more needed "Param" variables
	mWrappedNodePathParam.clear();
}

void NodePathWrapper::onRemoveFromObjectCleanup()
{
	//set the object node path to the old one
	mOwnerObject->setNodePath(mOldObjectNodePath);
	//
	reset();
}

//TypedObject semantics: hardcoded
TypeHandle NodePathWrapper::_type_handle;

///Template

NodePathWrapperTemplate::NodePathWrapperTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"NodePathWrapperTemplate::NodePathWrapperTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"NodePathWrapperTemplate::NodePathWrapperTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameSceneManager::GetSingletonPtr(),
			"NodePathWrapperTemplate::NodePathWrapperTemplate: invalid GameSceneManager")
	//
	setParametersDefaults();

}

NodePathWrapperTemplate::~NodePathWrapperTemplate()
{
	
}

ComponentType NodePathWrapperTemplate::componentType() const
{
	return ComponentType(NodePathWrapper::get_class_type().get_name());
}

ComponentFamilyType NodePathWrapperTemplate::familyType() const
{
	return ComponentFamilyType("Scene");
}

SMARTPTR(Component)NodePathWrapperTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(NodePathWrapper) newNodePathWrapper = new NodePathWrapper(this);
	newNodePathWrapper->setComponentId(compId);
	if (not newNodePathWrapper->initialize())
	{
		return NULL;
	}
	return newNodePathWrapper.p();
}

void NodePathWrapperTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	//no mandatory parameters
}

//TypedObject semantics: hardcoded
TypeHandle NodePathWrapperTemplate::_type_handle;

} // namespace ely
