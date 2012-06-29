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
	return mTmpl->familyType();
}

const ComponentType InstanceOf::componentType() const
{
	return mTmpl->componentType();
}

bool InstanceOf::initialize()
{
	//setup initial state
	mNodePath = NodePath(mComponentId);
	return true;
}

NodePath& InstanceOf::nodePath()
{
	return mNodePath;
}

void InstanceOf::onAddToObjectSetup()
{
	//set the node path of the object to the
	//node path of this instance of
	mOwnerObject->nodePath() = mNodePath;

	//get that object this component is an instance of;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not this component is instance of nothing.
	ObjectId instanceOfId = ObjectId(
			mTmpl->parameter(std::string("instance_of")));
	ObjectTemplateManager::ObjectTable& createdObjects =
			mOwnerObject->objectTmpl()->objectTmplMgr()->createdObjects();
	ObjectTemplateManager::ObjectTable::iterator iter;
	iter = createdObjects.find(instanceOfId);
	if (iter != createdObjects.end())
	{
		iter->second->nodePath().instance_to(mOwnerObject->nodePath());
	}
}

InstanceOf::operator NodePath()
{
	return mNodePath;
}

//TypedObject semantics: hardcoded
TypeHandle InstanceOf::_type_handle;
