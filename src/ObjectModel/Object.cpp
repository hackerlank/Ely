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
 * \author consultit
 */

#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"

namespace ely
{

Object::Object(const ObjectId& objectId, SMARTPTR(ObjectTemplate)tmpl) :
mTmpl(tmpl), mInitializationsLoaded(false)
{
	mObjectId = objectId;
}

Object::~Object()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	unloadInitializationFunctions();
	//
	mNodePath.remove_node();
}

void Object::clearComponents()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//remove object components
	while (mComponents.size() > 0)
	{
		ComponentTable::iterator iter = mComponents.begin();
		ComponentFamilyType compFamilyType = iter->first;
		PRINT(
				"\tRemoving component of family type '" << std::string(compFamilyType) << "'");
		mComponents.erase(iter);
	}
	std::cout << std::endl;
}

SMARTPTR(Component)Object::getComponent(const ComponentFamilyType& familyID) const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ComponentTable::const_iterator it = mComponents.find(familyID);
	if (it == mComponents.end())
	{
		return NULL;
	}
	return (*it).second;
}

SMARTPTR(Component)Object::addComponent(SMARTPTR(Component) component,
		bool existingObject)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not component)
	{
		throw GameException("Object::addComponent: NULL new Component");
	}
	SMARTPTR(Component) previousComp;
	previousComp.clear();
	ComponentFamilyType familyId = component->familyType();
	ComponentTable::iterator it = mComponents.find(familyId);
	if (it != mComponents.end())
	{
		// a previous component of that family already existed
		previousComp = (*it).second;
		mComponents.erase(it);
	}
	//set the component owner
	component->setOwnerObject(this);
	//insert the new component into the table
	mComponents[familyId] = component;
	//on addition to object component setup
	mComponents[familyId]->onAddToObjectSetup();
	//if this object is an already existing object calls onAddToSceneSetup
	if (existingObject)
	{
		mComponents[familyId]->onAddToSceneSetup();
	}
	return previousComp;
}

bool Object::removeComponent(SMARTPTR(Component) component)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not component)
	{
		throw GameException("Object::addComponent: NULL new Component");
	}
	ComponentFamilyType familyId = component->familyType();
	ComponentTable::iterator it = mComponents.find(familyId);
	if (it->second != component)
	{
		return false;
	}
	//erase component
	mComponents.erase(it);
	//
	return true;
}

void Object::sceneSetup()
{
	//Called only by ObjectTemplateManager::createObject(). Thread safe.

	//Parent (by default none)
	ObjectId parentId = ObjectId(mTmpl->parameter(std::string("parent")));
	//Is static (by default false)
	mIsSteady = (
			mTmpl->parameter(std::string("is_steady")) == std::string("true") ?
					true : false);
	//find parent into the created objects
	SMARTPTR(Object)createdObject = ObjectTemplateManager::GetSingleton().getCreatedObject(parentId);
	if (createdObject != NULL)
	{
		//reparent to parent
		mNodePath.reparent_to(createdObject->getNodePath());
	}

	//Position (default: (0,0,0))
	std::vector<std::string> posStr = parseCompoundString(
			mTmpl->parameter(std::string("pos")), ',');
	float pos[3];
	pos[0] = pos[1] = pos[2] = 0.0;
	for (unsigned int i = 0; (i < 3) and (i < posStr.size()); ++i)
	{
		pos[i] = strtof(posStr[i].c_str(), NULL);
	}
	mNodePath.set_pos(pos[0], pos[1], pos[2]);
	//Rotation (default: (0,0,0))
	std::vector<std::string> rotStr = parseCompoundString(
			mTmpl->parameter(std::string("rot")), ',');
	float rot[3];
	rot[0] = rot[1] = rot[2] = 0.0;
	for (unsigned int i = 0; (i < 3) and (i < rotStr.size()); ++i)
	{
		rot[i] = strtof(rotStr[i].c_str(), NULL);
	}
	mNodePath.set_hpr(rot[0], rot[1], rot[2]);
	//give components a chance to set up
	ComponentTable::iterator iterComp;
	for (iterComp = mComponents.begin(); iterComp != mComponents.end();
			++iterComp)
	{
		iterComp->second->onAddToSceneSetup();
	}
}

void Object::worldSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//load initialization functions library.
	loadInitializationFunctions();
	//execute the initialization function (if any)
	if (mInitializationsLoaded)
	{
		const char* dlsymError;
		std::string functionName;

		//load initialization function (if any): <OBJECTID>_initialization
		// reset errors
		lt_dlerror();
		functionName = std::string(mObjectId) + "_initialization";
		PINITIALIZATION pInitializationFunction = (PINITIALIZATION) lt_dlsym(
				mInitializationLib, functionName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT(
					"No initialization function '" << functionName << "': " << dlsymError);
		}
		else
		{
			//call initialization function
			pInitializationFunction(this, mTmpl->getParameterTable(),
					mTmpl->pandaFramework(), mTmpl->windowFramework());
		}
	}
}

void Object::loadInitializationFunctions()
{
	//if initializations loaded do nothing
	if (mInitializationsLoaded)
	{
		return;
	}
	mInitializationLib = NULL;
	// reset errors
	lt_dlerror();
	//load the initialization functions library
	mInitializationLib = lt_dlopen(INITIALIZATIONS_LA);
	if (mInitializationLib == NULL)
	{
		std::cerr << "Error loading library: " << INITIALIZATIONS_LA << ": "
				<< lt_dlerror() << std::endl;
		return;
	}
	//initializations loaded
	mInitializationsLoaded = true;
}

void Object::unloadInitializationFunctions()
{
	//if initializations not loaded do nothing
	if (not mInitializationsLoaded)
	{
		return;
	}
	//Close the initialization functions library
	// reset errors
	lt_dlerror();
	if (lt_dlclose(mInitializationLib) != 0)
	{
		std::cerr << "Error closing library: " << INITIALIZATIONS_LA << ": "
				<< lt_dlerror() << std::endl;
	}
	//initializations unloaded
	mInitializationsLoaded = false;
}

//TypedObject semantics: hardcoded
TypeHandle Object::_type_handle;

} // namespace ely
