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
 * \file /Ely/src/SceneComponents/InstanceOf.cpp
 *
 * \date 20/mag/2012 (09:42:38)
 * \author marco
 */

#include "SceneComponents/InstanceOf.h"
#include "SceneComponents/InstanceOfTemplate.h"

InstanceOf::InstanceOf()
{
	// TODO Auto-generated constructor stub
}

InstanceOf::InstanceOf(SMARTPTR(InstanceOfTemplate)tmpl)
{
	CHECKEXISTENCE(GameSceneManager::GetSingletonPtr(),
			"InstanceOf::InstanceOf: invalid GameSceneManager")
	mTmpl = tmpl;
}

InstanceOf::~InstanceOf()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath.remove_node();
}

const ComponentFamilyType InstanceOf::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType InstanceOf::componentType() const
{
	return mTmpl->componentType();
}

bool InstanceOf::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//setup initial state
	mNodePath = NodePath(mComponentId);
	//Scaling (default: (1.0,1.0,1.0))
	float scaleX = strtof(mTmpl->parameter(std::string("scale_x")).c_str(), NULL);
	float scaleY = strtof(mTmpl->parameter(std::string("scale_y")).c_str(), NULL);
	float scaleZ = strtof(mTmpl->parameter(std::string("scale_z")).c_str(), NULL);
	mNodePath.set_sx((scaleX != 0.0 ? scaleX : 1.0));
	mNodePath.set_sy((scaleY != 0.0 ? scaleY : 1.0));
	mNodePath.set_sz((scaleZ != 0.0 ? scaleZ : 1.0));
	//
	return result;
}

void InstanceOf::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set the node path of the object to the
	//node path of this instance of
	mOwnerObject->setNodePath(mNodePath);

	//get that object this component is an instance of;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not this component is instance of nothing.
	ObjectId instanceOfId = ObjectId(
			mTmpl->parameter(std::string("instance_of")));
	mInstancedObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			instanceOfId);
	if (mInstancedObject != NULL)
	{
		SMARTPTR(Component) component =
				mInstancedObject->getComponent(ComponentFamilyType("Scene"));
		if (component->is_of_type(Model::get_class_type()))
		{
			//an instanceable object should have a model component
			SMARTPTR(Model)model = DCAST(Model, component);
			if (model)
			{
				model->getNodePath().instance_to(
						mOwnerObject->getNodePath());
			}
		}
	}
	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

NodePath InstanceOf::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void InstanceOf::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

SMARTPTR(Object)InstanceOf::getInstancedObject() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mInstancedObject;
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOf::_type_handle;
