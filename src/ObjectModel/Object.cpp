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

const ObjectId& Object::objectId() const
{
	return mObjectId;
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

SMARTPTR(Component)Object::getComponent(const ComponentFamilyType& familyID)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ComponentTable::iterator it = mComponents.find(familyID);
	if (it == mComponents.end())
	{
		return NULL;
	}
	return (*it).second;
}

SMARTPTR(Component)Object::addComponent(SMARTPTR(Component) newComponent)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not newComponent)
	{
		throw GameException("Object::addComponent: NULL new Component");
	}
	SMARTPTR(Component) previousComp;
	previousComp.clear();
	ComponentFamilyType familyId = newComponent.p()->familyType();
	ComponentTable::iterator it = mComponents.find(familyId);
	if (it != mComponents.end())
	{
		// a previous component of that family already existed
		previousComp = (*it).second;
		mComponents.erase(it);
	}
	//set the component owner
	newComponent->setOwnerObject(this);
	//insert the new component into the table
	mComponents[familyId] = newComponent;
	//on addition to object component setup
	mComponents[familyId].p()->onAddToObjectSetup();
	return previousComp;
}

unsigned int Object::numComponents()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return static_cast<unsigned int>(mComponents.size());
}

NodePath Object::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void Object::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

Object::operator NodePath()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void Object::sceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//Parent (by default none)
	ObjectId parentId = ObjectId(mTmpl->parameter(std::string("parent")));
	//Is static (by default false)
	mIsStatic = (
			mTmpl->parameter(std::string("is_static")) == std::string("true") ?
					true : false);
	//find parent into the created objects
	SMARTPTR(Object)createdObject = ObjectTemplateManager::GetSingleton().getCreatedObject(parentId);
	if (createdObject != NULL)
	{
		//reparent to parent
		mNodePath.reparent_to(createdObject->getNodePath());
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
	//give components a chance to set up
	ComponentTable::iterator iterComp;
	for (iterComp = mComponents.begin(); iterComp != mComponents.end();
			++iterComp)
	{
		iterComp->second.p()->onAddToSceneSetup();
	}
	//load initialization functions library.
	loadInitializationFunctions();
	//execute the initialization function (if any)
	if (mInitializationsLoaded)
	{
		const char* dlsymError;
		std::string functionName;

		//load initialization function (if any): <OBJECTID>_initialization
		// reset errors
		dlerror();
		functionName = std::string(mObjectId) + "_initialization";
		PINITIALIZATION pInitializationFunction = (PINITIALIZATION) dlsym(
				mInitializationLib, functionName.c_str());
		dlsymError = dlerror();
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

#ifdef WIN32
void Object::loadInitializationFunctions()
{
}

void Activity::unloadInitializationFunctions()
{
}
#else
void Object::loadInitializationFunctions()
{
	//if initializations loaded do nothing
	if (mInitializationsLoaded)
	{
		return;
	}
	mInitializationLib = NULL;
	//load the initialization functions library
	mInitializationLib = dlopen(INITIALIZATIONS_SO, RTLD_LAZY);
	if (not mInitializationLib)
	{
		std::cerr << "Error loading library: " << dlerror() << std::endl;
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
	if (dlclose(mInitializationLib) != 0)
	{
		std::cerr << "Error closing library: " << INITIALIZATIONS_SO
				<< std::endl;
	}
	//initializations unloaded
	mInitializationsLoaded = false;
}
#endif

SMARTPTR(ObjectTemplate)const Object::objectTmpl() const
{
	return mTmpl;
}

bool Object::isStatic()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsStatic;
}

void Object::setStatic(bool value)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mIsStatic = value;
}

ReMutex& Object::getMutex()
{
	return mMutex;
}

//TypedObject semantics: hardcoded
TypeHandle Object::_type_handle;

