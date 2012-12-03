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
 * \date 11/nov/2012 (09:45:00)
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
	CHECKEXISTENCE(GamePhysicsManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GamePhysicsManager")
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

	//check kinematic parameters
	if (mAbsLookAtDistance < 0.0)
	{
		mAbsLookAtDistance = abs(mDistance);
	}
	if ((mAbsMinDistance > abs(mDistance)) or (mAbsMinDistance < 0.0))
	{
		mAbsMinDistance = abs(mDistance);
	}
	if (mAbsMinHeight <= 0.0)
	{
		mAbsMinHeight = abs(mDistance);
	}
	//check if backward located
	float sign = (mBackward ? 1.0 : -1.0);
	//set chaser position (wrt chased node)
	mChaserPosition = LPoint3f(0.0, -mDistance * sign, mAbsMinHeight * 1.5);
	//set "look at" position (wrt chased node)
	mLookAtPosition = LPoint3f(0.0, mAbsLookAtDistance * sign, 0.0);

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

float Chaser::getAbsLookAtDistance() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAbsLookAtDistance;
}

void Chaser::setAbsLookAtDistance(float absLookAtDistance)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mAbsLookAtDistance = absLookAtDistance;
}

float Chaser::getAbsMinDistance() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAbsMinDistance;
}

void Chaser::setAbsMinDistance(float absMinDistance)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mAbsMinDistance = absMinDistance;
}

float Chaser::getAbsMinHeight() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAbsMinHeight;
}

void Chaser::setAbsMinHeight(float absMinHeight)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mAbsMinHeight = absMinHeight;
}

float Chaser::getDistance() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mDistance;
}

void Chaser::setDistance(float distance)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mDistance = distance;
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
	//look at distance settings
	mAbsLookAtDistance = (float) atof(
			mTmpl->parameter(std::string("abs_lookat_distance")).c_str());
	//distance settings
	mDistance = (float) atof(mTmpl->parameter(std::string("distance")).c_str());
	//min distance settings
	mAbsMinDistance = (float) atof(
			mTmpl->parameter(std::string("abs_min_distance")).c_str());
	//min height settings
	mAbsMinHeight = (float) atof(
			mTmpl->parameter(std::string("abs_min_height")).c_str());
	//friction' settings
	mFriction = (float) atof(mTmpl->parameter(std::string("friction")).c_str());
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
	//setup event callbacks if any
	setupEvents();
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
	LPoint3f actualChaserPos = mReferenceNodePath.get_relative_point(
			mOwnerObject->getNodePath(), LPoint3f::zero());
	LPoint3f newPos = getChaserPos(desiredChaserPos, actualChaserPos, dt);
	//correct chaser height (not in OgreBulletDemos)
	LPoint3f downTo = LPoint3f(newPos.get_x(), newPos.get_y(), -1000000.0);
	BulletClosestHitRayResult result =
			GamePhysicsManager::GetSingleton().bulletWorld()->ray_test_closest(
					newPos, downTo);
	if (newPos.get_z() - result.get_hit_pos().get_z() < mAbsMinHeight)
	{
		newPos.set_z(result.get_hit_pos().get_z() + mAbsMinHeight);
	}
	mOwnerObject->getNodePath().set_pos(mReferenceNodePath, newPos);
	//orientation
	mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
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
	//calculate new position
	LPoint3f newPos = desiredChaserPos + deltaPos;
	//correct min distance
	LPoint3f chasedPos = mReferenceNodePath.get_relative_point(mChasedNodePath,
			LPoint3f::zero());
	LVector3f newTargetDir = newPos - chasedPos;
	if (newTargetDir.length() < mAbsMinDistance)
	{
		newTargetDir.normalize();
		newPos = chasedPos + newTargetDir * mAbsMinDistance;
	}
	//
	return newPos;
}

//TypedObject semantics: hardcoded
TypeHandle Chaser::_type_handle;

