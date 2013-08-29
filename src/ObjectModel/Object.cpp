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
		mTmpl(tmpl), mObjectId(objectId), mInitializationLib(NULL),
		mInitializationsLoaded(false), mInititializationFuncName(std::string(""))
{
	doReset();
}

Object::~Object()
{
}

SMARTPTR(Component)Object::getComponent(const ComponentFamilyType& familyId) const
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	ComponentOrderedList::const_iterator it =
			find_if(mComponents.begin(), mComponents.end(), IsFamily(familyId));
	if (it == mComponents.end())
	{
		return NULL;
	}
	return (*it).second;
}

bool Object::doAddComponent(SMARTPTR(Component) component,
		const ComponentFamilyType& familyId)
{
	if (not component)
	{
		throw GameException("Object::addComponent: NULL new Component");
	}
	ComponentOrderedList::iterator it =
			find_if(mComponents.begin(), mComponents.end(), IsFamily(familyId));
	if (it != mComponents.end())
	{
		return false;
	}
	//insert the new component into the list at the back end
	mComponents.push_back(FamilyTypeComponentPair(familyId, component));
	//
	return true;
}

bool Object::doRemoveComponent(SMARTPTR(Component) component,
		const ComponentFamilyType& familyId)
{
	if (not component)
	{
		throw GameException("Object::addComponent: NULL new Component");
	}
	ComponentOrderedList::iterator it =
			find_if(mComponents.begin(), mComponents.end(), IsFamily(familyId));
	if ((it == mComponents.end()) or (it->second != component))
	{
		return false;
	}
	//erase component
	mComponents.erase(it);
	//
	return true;
}

void Object::onRemoveObjectCleanup()
{
	//
	doReset();
	//unload initialization functions
	doUnloadInitializationFunctions();
}

void Object::onAddToSceneSetup()
{
	//Called only by ObjectTemplateManager::createObject(). Thread safe.

	//Parent (by default none)
	ObjectId parentId = ObjectId(mTmpl->parameter(std::string("parent")));
	//Is static (by default false)
	mIsSteady = (
			mTmpl->parameter(std::string("is_steady")) == std::string("true") ?
					true : false);
	//find parent into the created objects
	SMARTPTR(Object)createdObject =
			ObjectTemplateManager::GetSingleton().getCreatedObject(parentId);
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
	//initialization function name
	mInititializationFuncName = mTmpl->parameter(std::string("init_func"));
}

void Object::onRemoveFromSceneCleanup()
{
	//set default pos/hpr
	mNodePath.set_hpr(0.0, 0.0, 0.0);
	mNodePath.set_pos(0.0, 0.0, 0.0);
	//detach node path from its parent
	mNodePath.detach_node();
	//set steady to false
	mIsSteady = false;
}

void Object::worldSetup()
{
	//called (indirectly) only by the main thread: mutex lock not needed

	//load initialization functions library.
	doLoadInitializationFunctions();
	//execute the initialization function (if any)
	if (mInitializationsLoaded)
	{
		const char* dlsymError;
		std::string functionName;

		//load initialization function (if any): <OBJECTID>_initialization
		// doReset errors
		lt_dlerror();
		functionName = (
				not mInititializationFuncName.empty() ?
						mInititializationFuncName :
						std::string(mObjectId) + "_initialization");
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

void Object::doLoadInitializationFunctions()
{
	//if initializations loaded do nothing
	if (mInitializationsLoaded)
	{
		return;
	}
	// doReset errors
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

void Object::doUnloadInitializationFunctions()
{
	//if initializations not loaded do nothing
	if (not mInitializationsLoaded)
	{
		return;
	}
	//Close the initialization functions library
	// doReset errors
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
