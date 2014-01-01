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
#include "Game/GameManager.h"

namespace ely
{

Object::Object(const ObjectId& objectId, SMARTPTR(ObjectTemplate)tmpl) :
mTmpl(tmpl), mObjectId(objectId), mOwner(NULL),mInitializationLib(NULL),
mInitializationsLoaded(false), mInititializationFuncName(""),
mInitializationFunction(NULL)
{
	//by default set node path to not empty
	mNodePath = NodePath(mObjectId);
	doReset();
}

Object::~Object()
{
}

SMARTPTR(Component)Object::getComponent(const ComponentFamilyType& familyId) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ComponentOrderedList::const_iterator it =
	find_if(mComponents.begin(), mComponents.end(), IsFamily(familyId));
	if (it == mComponents.end())
	{
		return NULL;
	}
	return (*it).second;
}

bool Object::doAddComponent(SMARTPTR(Component)component,
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

bool Object::doRemoveComponent(SMARTPTR(Component)component,
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

	//Is static (by default false)
	mIsSteady = (
			mTmpl->parameter(std::string("is_steady")) == std::string("true") ?
					true : false);

	///set up into scene only if NodePath is not empty
	if (not mNodePath.is_empty())
	{
		//Parent (by default none)
		ObjectId parentId = ObjectId(mTmpl->parameter(std::string("parent")));
		//find parent into the created objects
		SMARTPTR(Object)createdObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(parentId);
		if (createdObject != NULL)
		{
			//reparent to parent
			mNodePath.reparent_to(createdObject->getNodePath());
		}

		//
		std::string param;
		unsigned int idx, valueNum;
		std::vector<std::string> paramValuesStr;
		//position
		param = mTmpl->parameter(std::string("pos"));
		paramValuesStr = parseCompoundString(param, ',');
		valueNum = paramValuesStr.size();
		if (valueNum < 3)
		{
			paramValuesStr.resize(3, "0.0");
		}
		LPoint3f pos;
		for (idx = 0; idx < 3; ++idx)
		{
			pos[idx] = strtof(paramValuesStr[idx].c_str(), NULL);
		}
		mNodePath.set_pos(pos);
		//rotation
		param = mTmpl->parameter(std::string("rot"));
		paramValuesStr = parseCompoundString(param, ',');
		valueNum = paramValuesStr.size();
		if (valueNum < 3)
		{
			paramValuesStr.resize(3, "0.0");
		}
		LVecBase3f rot;
		for (idx = 0; idx < 3; ++idx)
		{
			rot[idx] = strtof(paramValuesStr[idx].c_str(), NULL);
		}
		mNodePath.set_hpr(rot);
	}

	///set initialization function (if any)
	//get initialization function name
	mInititializationFuncName = mTmpl->parameter(std::string("init_func"));
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
		mInitializationFunction = (PINITIALIZATION) lt_dlsym(mInitializationLib,
				functionName.c_str());
		dlsymError = lt_dlerror();
		if (dlsymError)
		{
			PRINT_DEBUG(
					"No initialization function '" << functionName << "': " << dlsymError);
		}
	}
}

void Object::onRemoveFromSceneCleanup()
{
	///clean up into scene only if NodePath is not empty
	if (not mNodePath.is_empty())
	{
		//set default pos/hpr
		mNodePath.set_hpr(0.0, 0.0, 0.0);
		mNodePath.set_pos(0.0, 0.0, 0.0);
		//detach node path from its parent
		mNodePath.detach_node();
	}
	//set steady to false
	mIsSteady = false;
}

void Object::worldSetup()
{
	//this method is thread safe because
	//mInitializationFunction is read-only when called
	if (mInitializationFunction)
	{
		//call initialization function
		mInitializationFunction(this, mTmpl->getParameterTable(),
				mTmpl->pandaFramework(), mTmpl->windowFramework());
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
	mInitializationLib = lt_dlopen(
			GameManager::GetSingletonPtr()->getDataInfo(
					GameManager::INITIALIZATIONS).c_str());
	if (mInitializationLib == NULL)
	{
		std::cerr << "Error loading library: "
				<< GameManager::GetSingletonPtr()->getDataInfo(
						GameManager::INITIALIZATIONS) << ": " << lt_dlerror()
				<< std::endl;
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
		std::cerr << "Error closing library: "
				<< GameManager::GetSingletonPtr()->getDataInfo(
						GameManager::INITIALIZATIONS) << ": " << lt_dlerror()
				<< std::endl;
	}
	//initializations unloaded
	mInitializationsLoaded = false;
}

//TypedObject semantics: hardcoded
TypeHandle Object::_type_handle;

} // namespace ely
