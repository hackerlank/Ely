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
 * \file /Ely/src/ControlComponents/Chaser.cpp
 *
 * \date 11/nov/2012 09:45:00
 * \author marco
 */

#include "ControlComponents/Chaser.h"
#include "ControlComponents/ChaserTemplate.h"

Chaser::Chaser()
{
	// TODO Auto-generated constructor stub
}

Chaser::Chaser(SMARTPTR(ChaserTemplate)tmpl):mIsEnabled(false)
{
	CHECKEXISTENCE(GameControlManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GameControlManager")
	mTmpl = tmpl;
}

Chaser::~Chaser()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	disable();
}

const ComponentFamilyType Chaser::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Chaser::componentType() const
{
	return mTmpl->componentType();
}

void Chaser::enable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mIsEnabled or (not mOwnerObject) or mChasedNodePath.is_empty())
	{
		return;
	}

	//check distances
	if (mDistance <= 0.0)
	{
		mDistance = 1.0;
	}
	if (mMinDistance <= 0.0)
	{
		mMinDistance = 1.0;
	}
	//check if backward located
	float sign = (mBackward ? 1.0 : -1.0);
	//set chaser position (wrt chased node)
	mChaserPosition = LPoint3f(0.0, -5.0 * mDistance * sign, mDistance);
	//set "look at" position (wrt chased node)
	mLookAtPosition = LPoint3f(0.0, mDistance * sign, 0.0);

	//add to the control manager update
	GameControlManager::GetSingletonPtr()->addToControlUpdate(this);
	//
	mIsEnabled = not mIsEnabled;
	//register event callbacks if any
	registerEventCallbacks();
}

void Chaser::disable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if ((not mIsEnabled) or (not mOwnerObject) or mChasedNodePath.is_empty())
	{
		return;
	}

	//check if control manager exists
	if (GameControlManager::GetSingletonPtr())
	{
		//remove from control manager update
		GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);
	}
	//
	mIsEnabled = not mIsEnabled;
	//unregister event callbacks if any
	unregisterEventCallbacks();
}

bool Chaser::isEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsEnabled;
}

bool Chaser::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//get settings from template
	//enabling setting
	mEnabled = (
			mTmpl->parameter(std::string("enabled")) == std::string("true") ?
					true : false);
	//backward setting
	mBackward = (
			mTmpl->parameter(std::string("backward")) == std::string("true") ?
					true : false);
	//distances' settings
	mDistance = (float) atof(mTmpl->parameter(std::string("distance")).c_str());
	mMinDistance = (float) atof(
			mTmpl->parameter(std::string("min_distance")).c_str());
	//friction' settings
	mFriction = (float) atof(mTmpl->parameter(std::string("friction")).c_str());
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void Chaser::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set mChasedNodePath as empty
	mChasedNodePath = NodePath();

	//set the (node path of) object chased by this component;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not, this component chases nothing.
	ObjectId chasedId = ObjectId(
			mTmpl->parameter(std::string("chased_object")));
	SMARTPTR(Object)chasedObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			chasedId);
	if (chasedObject != NULL)
	{
		mChasedNodePath = chasedObject->getNodePath();

		//set the (node path of) reference object;
		//that object is supposed to be already created,
		//set up and added to the created objects table;
		//if not, this will be the parent of the chased object.
		ObjectId referenceId = ObjectId(
				mTmpl->parameter(std::string("reference_object")));
		SMARTPTR(Object)referenceObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				referenceId);
		if (referenceObject != NULL)
		{
			mReferenceNodePath = referenceObject->getNodePath();
		}
		else
		{
			mReferenceNodePath = mChasedNodePath.get_parent();
		}
	}
	//enable the component
	if (mEnabled)
	{
		enable();
	}
}

void Chaser::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if ((not mEnabled) or (not mOwnerObject) or mChasedNodePath.is_empty())
	{
		return;
	}

	//set chaser initial position/orientation
	mOwnerObject->getNodePath().set_pos(mChasedNodePath, mChaserPosition);
	mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
			LVector3::up());
}

void Chaser::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

	//update chaser position and orientation (see OgreBulletDemos)
	//position
	LPoint3f desiredChaserPos = mReferenceNodePath.get_relative_point(
			mChasedNodePath, mChaserPosition);
	LPoint3f actualChaserPos = mOwnerObject->getNodePath().get_pos(
			mReferenceNodePath);
	mOwnerObject->getNodePath().set_pos(mReferenceNodePath,
			getChaserPos(desiredChaserPos, actualChaserPos, dt));
	//orientation
	LPoint3f desiredLooAtPos = mReferenceNodePath.get_relative_point(
			mChasedNodePath, mLookAtPosition);
	mOwnerObject->getNodePath().look_at(mReferenceNodePath, desiredLooAtPos,
			LVector3::up());
}

LPoint3f Chaser::getChaserPos(LPoint3f desiredChaserPos,
		LPoint3f actualChaserPos, float deltaTime)
{
	float kReductFactor = mFriction * deltaTime;
	//calculate difference between desiredChaserPos and actualChaserPos
	LVector3f deltaPos = actualChaserPos - desiredChaserPos;
	//converge deltaPos.lenght toward zero: proportionally to deltaPos.lenght
	if (deltaPos.length_squared() > 0.0)
	{
		deltaPos -= deltaPos * kReductFactor;
	}
	//move chaser to new position
	LPoint3f newPos = desiredChaserPos + deltaPos;
	//correct min distance
	LPoint3f chasedPos = mChasedNodePath.get_pos(mReferenceNodePath);
	LVector3f newTargetDir = newPos - chasedPos;
	if (newTargetDir.length() < mMinDistance)
	{
		newTargetDir.normalize();
		newPos = chasedPos + newTargetDir * mMinDistance;
	}
	//
	return newPos;
}

//TypedObject semantics: hardcoded
TypeHandle Chaser::_type_handle;

