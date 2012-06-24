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
		throw GameException("Object::setComponent: NULL new Component");
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
	//Parent (by default render)
	ObjectId parentId = ObjectId(mTmpl->parameter(std::string("parent")));
	//find parent into the created objects
	ObjectTemplateManager::ObjectTable& createdObjects =
			mTmpl->objectTmplMgr()->createdObjects();
	ObjectTemplateManager::ObjectTable::iterator iter;
	iter = createdObjects.find(parentId);
	if (iter != createdObjects.end())
	{
		//reparent to parent
		mNodePath.reparent_to(iter->second->nodePath());
	}
	else
	{
		//reparent to render
		mNodePath.reparent_to(mTmpl->windowFramework()->get_render());
	}
	//Position (default: (0,0,0))

	tag = node->FirstChildElement("Position");
	if (tag != NULL)
	{
		const char *coord;
		coord = tag->Attribute("x", NULL);
		if (coord != NULL)
		{
			objectNodePtr->nodePath().set_x((float) atof(coord));
		}
		coord = tag->Attribute("y", NULL);
		if (coord != NULL)
		{
			objectNodePtr->nodePath().set_y((float) atof(coord));
		}
		coord = tag->Attribute("z", NULL);
		if (coord != NULL)
		{
			objectNodePtr->nodePath().set_z((float) atof(coord));
		}
	}
	//Orientation (default: (0,0,0))
	tag = node->FirstChildElement("Orientation");
	if (tag != NULL)
	{
		const char *coord;
		coord = tag->Attribute("h", NULL);
		if (coord != NULL)
		{
			objectNodePtr->nodePath().set_h((float) atof(coord));
		}
		coord = tag->Attribute("p", NULL);
		if (coord != NULL)
		{
			objectNodePtr->nodePath().set_p((float) atof(coord));
		}
		coord = tag->Attribute("r", NULL);
		if (coord != NULL)
		{
			objectNodePtr->nodePath().set_r((float) atof(coord));
		}
	}
	//Scaling (default: (1.0,1.0,1.0))
	tag = node->FirstChildElement("Scaling");
	if (tag != NULL)
	{
		const char *coord;
		coord = tag->Attribute("x", NULL);
		if (coord != NULL)
		{
			float res = (float) atof(coord);
			objectNodePtr->nodePath().set_sx((res != 0.0 ? res : 1.0));
		}
		coord = tag->Attribute("y", NULL);
		if (coord != NULL)
		{
			float res = (float) atof(coord);
			objectNodePtr->nodePath().set_sy((res != 0.0 ? res : 1.0));
		}
		coord = tag->Attribute("z", NULL);
		if (coord != NULL)
		{
			float res = (float) atof(coord);
			objectNodePtr->nodePath().set_sz((res != 0.0 ? res : 1.0));
		}
	}
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

