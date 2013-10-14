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
 * \author consultit
 */

#include "SceneComponents/InstanceOf.h"
#include "SceneComponents/InstanceOfTemplate.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "SceneComponents/Model.h"
#include "Game/GameSceneManager.h"

namespace ely
{

InstanceOf::InstanceOf()
{
	// TODO Auto-generated constructor stub
}

InstanceOf::InstanceOf(SMARTPTR(InstanceOfTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameSceneManager::GetSingletonPtr(),
			"InstanceOf::InstanceOf: invalid GameSceneManager")

	mTmpl = tmpl;
	reset();
}

InstanceOf::~InstanceOf()
{
}

ComponentFamilyType InstanceOf::familyType() const
{
	return mTmpl->familyType();
}

ComponentType InstanceOf::componentType() const
{
	return mTmpl->componentType();
}

bool InstanceOf::initialize()
{
	bool result = true;
	//instance of object id
	mInstanceOfId = ObjectId(
			mTmpl->parameter(std::string("instance_of")));
	//
	std::string param;
	unsigned int idx, valueNum;
	std::vector<std::string> paramValuesStr;
	//scaling
	param = mTmpl->parameter(std::string("scale"));
	paramValuesStr = parseCompoundString(param , ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < 3))
	{
		paramValuesStr.resize(3, paramValuesStr[0]);
	}
	else if (valueNum < 3)
	{
		paramValuesStr.resize(3, "1.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mScale[idx] = strtof(paramValuesStr[idx].c_str(), NULL);
	}
	//
	return result;
}

void InstanceOf::onAddToObjectSetup()
{
	//setup initial state
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
	mNodePath = NodePath(name);

	//set scaling (default: (1.0,1.0,1.0))
	mNodePath.set_scale(mScale[0], mScale[1], mScale[2]);

	//get that object this component is an instance of;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not this component is instance of nothing.
	mInstancedObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			mInstanceOfId);
	if (mInstancedObject != NULL)
	{
		SMARTPTR(Component) component =
				mInstancedObject->getComponent(ComponentFamilyType("Scene"));
		//an instanceable object should have a model component
		if (component and component->is_of_type(Model::get_class_type()))
		{
			DCAST(Model, component)->getNodePath().instance_to(mNodePath);
		}
	}
	//set the object node path to this instance of node path
	mOldObjectNodePath = mOwnerObject->getNodePath();
	mOwnerObject->setNodePath(mNodePath);
}

void InstanceOf::onRemoveFromObjectCleanup()
{
	//set the object node path to the old one
	mOwnerObject->setNodePath(mOldObjectNodePath);

	//detach the first child of this instance of node path (if any)
	if (mInstancedObject and (mNodePath.get_num_children() > 0))
	{
		/// \see NodePath::instance_to() documentation.
		mNodePath.get_child(0).detach_node();
	}

	//Remove node path
	mNodePath.remove_node();
	//
	reset();
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOf::_type_handle;

} // namespace ely
