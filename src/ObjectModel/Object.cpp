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
 * \file /Ely/src/ObjectModel/Object.cpp
 *
 * \date 07/mag/2012 (18:10:37)
 * \author marco
 */

#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"

Object::Object(const ObjectId& objectId, ObjectTemplate* tmpl) :
		mTmpl(tmpl), mIsStatic(false)
{
	mObjectId = objectId;
}

Object::~Object()
{
	mNodePath.remove_node();
}

ObjectId& Object::objectId()
{
	return mObjectId;
}

void Object::clearComponents()
{
	mComponents.clear();
}

Component* Object::getComponent(const ComponentFamilyType& familyID)
{
	ComponentTable::iterator it = mComponents.find(familyID);
	if (it == mComponents.end())
	{
		return NULL;
	}
	return (*it).second;
}

PT(Component) Object::addComponent(Component* newComponent)
{
	if (not newComponent)
	{
		throw GameException("Object::addComponent: NULL new Component");
	}
	PT(Component) previousComp = NULL;
	ComponentFamilyType familyId = newComponent->familyType();
	ComponentTable::iterator it = mComponents.find(familyId);
	if (it != mComponents.end())
	{
		// a previous component of that family already existed
		previousComp = (*it).second;
		mComponents.erase(it);
	}
	//set the component owner
	newComponent->ownerObject() = this;
	//insert the new component into the table
	mComponents[familyId] = PT(Component)(newComponent);
	//on addition to object component setup
	mComponents[familyId]->onAddToObjectSetup();
	return previousComp;
}

NodePath& Object::nodePath()
{
	return mNodePath;
}

unsigned int Object::numComponents()
{
	return static_cast<unsigned int>(mComponents.size());
}

Object::operator NodePath()
{
	return mNodePath;
}

void Object::sceneSetup()
{
	//Parent (by default none)
	ObjectId parentId = ObjectId(mTmpl->parameter(std::string("parent")));
	//find parent into the created objects
	ObjectTemplateManager::ObjectTable& createdObjects =
			mTmpl->objectTmplMgr()->createdObjects();
	ObjectTemplateManager::ObjectTable::iterator iterObj;
	iterObj = createdObjects.find(parentId);
	if (iterObj != createdObjects.end())
	{
		//reparent to parent
		mNodePath.reparent_to(iterObj->second->nodePath());
	}
	//Position (default: (0,0,0))
	float posX = atof(mTmpl->parameter(std::string("pos_x")).c_str());
	float posY = atof(mTmpl->parameter(std::string("pos_y")).c_str());
	float posZ = atof(mTmpl->parameter(std::string("pos_z")).c_str());
	mNodePath.set_x(posX);
	mNodePath.set_y(posY);
	mNodePath.set_z(posZ);
	//Orientation (default: (0,0,0))
	float rotH = atof(mTmpl->parameter(std::string("rot_h")).c_str());
	float rotP = atof(mTmpl->parameter(std::string("rot_p")).c_str());
	float rotR = atof(mTmpl->parameter(std::string("rot_r")).c_str());
	mNodePath.set_h(rotH);
	mNodePath.set_p(rotP);
	mNodePath.set_r(rotR);
	//Scaling (default: (1.0,1.0,1.0))
	float scaleX = atof(mTmpl->parameter(std::string("scale_x")).c_str());
	float scaleY = atof(mTmpl->parameter(std::string("scale_y")).c_str());
	float scaleZ = atof(mTmpl->parameter(std::string("scale_z")).c_str());
	mNodePath.set_sx((scaleX != 0.0 ? scaleX : 1.0));
	mNodePath.set_sy((scaleY != 0.0 ? scaleY : 1.0));
	mNodePath.set_sz((scaleZ != 0.0 ? scaleZ : 1.0));
	//give components a chance to set up
	ComponentTable::iterator iterComp;
	for (iterComp = mComponents.begin(); iterComp != mComponents.end();
			++iterComp)
	{
		iterComp->second->onAddToSceneSetup();
	}
}

ObjectTemplate* Object::objectTmpl()
{
	return mTmpl;
}

bool& Object::isStatic()
{
	return mIsStatic;
}

//TypedObject semantics: hardcoded
TypeHandle Object::_type_handle;

