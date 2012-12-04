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
 * \file /Ely/src/AIComponents/Steering.cpp
 *
 * \date 03/dic/2012 (13:39:59)
 * \author marco
 */

#include "AIComponents/Steering.h"
#include "AIComponents/SteeringTemplate.h"

Steering::Steering()
{
	// TODO Auto-generated constructor stub
}

Steering::Steering(SMARTPTR(SteeringTemplate)tmpl)
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"Steering::Steering: invalid GameAIManager")
	mTmpl = tmpl;
	mAICharacter = NULL;
	mUpdatePtr = NULL;
}

Steering::~Steering()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	delete mAICharacter;
}

const ComponentFamilyType Steering::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Steering::componentType() const
{
	return mTmpl->componentType();
}

bool Steering::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//set AICharacter parameters
	//type
	std::string type = mTmpl->parameter(std::string("type"));
	///TODO
	if (type == std::string("nodepath"))
	{
	}
	else
	{
		//controller
	}
	//behavior
	mBehavior = mTmpl->parameter(std::string("behavior"));
	//
	float floatParam;
	//mass
	floatParam = (float) atof(mTmpl->parameter(std::string("mass")).c_str());
	floatParam > 0.0 ? mMass = floatParam : mMass = 1.0;
	//movt_force
	floatParam = (float) atof(
			mTmpl->parameter(std::string("movt_force")).c_str());
	floatParam > 0.0 ? mMovtForce = floatParam : mMovtForce = 1.0;
	//max_force
	floatParam = (float) atof(
			mTmpl->parameter(std::string("max_force")).c_str());
	floatParam > 0.0 ? mMaxForce = floatParam : mMaxForce = 1.0;
	//
	return result;
}

void Steering::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//create the AICharacter...
	mAICharacter = new AICharacter(std::string(mComponentId),
			mOwnerObject->getNodePath(), mMass, mMovtForce, mMaxForce);
	//...add it to the AIWorld
	GameAIManager::GetSingletonPtr()->aiWorld()->add_ai_char(mAICharacter);
	//get a reference to its AIBehaviors
	AIBehaviors* behaviors = mAICharacter->get_ai_behaviors();
	//switch to the indicated behavior

	///TODO
	if (mBehavior == std::string("flee"))
	{
	}
	else if (mBehavior == std::string("pursue"))
	{
	}
	else if (mBehavior == std::string("evade"))
	{
	}
	else if (mBehavior == std::string("arrival"))
	{
	}
	else if (mBehavior == std::string("wander"))
	{
	}
	else if (mBehavior == std::string("flock"))
	{
	}
	else if (mBehavior == std::string("obstacle_avoidance"))
	{
	}
	else if (mBehavior == std::string("path_follow"))
	{
	}
	else
	{
		//seek

	}
	//Add to the AI manager update
	GameAIManager::GetSingletonPtr()->addToAIUpdate(this);
	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void Steering::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//call the designated update member
	((*this).*mUpdatePtr)(dt);
}

void Steering::updateController(float dt)
{
}

void Steering::updateNodePath(float dt)
{
}

//TypedObject semantics: hardcoded
TypeHandle Steering::_type_handle;
