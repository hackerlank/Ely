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
#include "ObjectModel/ObjectTemplateManager.h"
#include "SceneComponents/Model.h"
#include "Game/GameSceneManager.h"

namespace ely
{

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

bool InstanceOf::initialize()
{
	bool result = true;
	//instance of object id
	mInstanceOfId = ObjectId(mTmpl->parameter(std::string("instance_of")));
	//
	std::string param;
	unsigned int idx, valueNum;
	std::vector<std::string> paramValuesStr;
	float value;
	//scale
	param = mTmpl->parameter(std::string("scale"));
	paramValuesStr = parseCompoundString(param, ',');
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
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mScale[idx] = (value >= 0.0 ? value : -value);
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

	//set scaling
	mNodePath.set_scale(mScale);

	//get that object this component is an instance of;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not this component is instance of nothing.
	mInstancedObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			mInstanceOfId);
	if (mInstancedObject)
	{
		SMARTPTR(Component)sceneComponent =
				mInstancedObject->getComponent(ComponentFamilyType("Scene"));
		//an instanceable object should have a model component
		if (sceneComponent and
				(sceneComponent->componentType() == ComponentType("Model")))
		{
			DCAST(Model, sceneComponent)->getNodePath().instance_to(mNodePath);
		}
	}
	//reparent this InstanceOf node path to the object node path
	mNodePath.reparent_to(mOwnerObject->getNodePath());
}

void InstanceOf::onRemoveFromObjectCleanup()
{
	//detach the first child of this instance of node path (if any)
	if (mInstancedObject and (mNodePath.get_num_children() > 0))
	{
		// \see NodePath::instance_to() documentation.
		mNodePath.get_child(0).detach_node();
	}

	//Remove node path
	mNodePath.remove_node();
	//
	reset();
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOf::_type_handle;

///Template

InstanceOfTemplate::InstanceOfTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"InstanceOfTemplate::InstanceOfTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"InstanceOfTemplate::InstanceOfTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameSceneManager::GetSingletonPtr(),
			"InstanceOfTemplate::InstanceOfTemplate: invalid GameSceneManager")
	//
	setParametersDefaults();
}

InstanceOfTemplate::~InstanceOfTemplate()
{
	
}

ComponentType InstanceOfTemplate::componentType() const
{
	return ComponentType(InstanceOf::get_class_type().get_name());
}

ComponentFamilyType InstanceOfTemplate::componentFamilyType() const
{
	return ComponentFamilyType("Scene");
}

SMARTPTR(Component)InstanceOfTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(InstanceOf) newInstanceOf = new InstanceOf(this);
	newInstanceOf->setComponentId(compId);
	if (not newInstanceOf->initialize())
	{
		return NULL;
	}
	return newInstanceOf.p();
}

void InstanceOfTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("scale", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOfTemplate::_type_handle;

} // namespace ely
