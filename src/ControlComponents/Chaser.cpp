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
 * \author consultit
 */

#include "ControlComponents/Chaser.h"
#include "ControlComponents/ChaserTemplate.h"
#include "Game/GameControlManager.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

Chaser::Chaser()
{
	// TODO Auto-generated constructor stub
}

Chaser::Chaser(SMARTPTR(ChaserTemplate)tmpl)
{
	CHECK_EXISTENCE(GameControlManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GameControlManager")
	CHECK_EXISTENCE(GamePhysicsManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GamePhysicsManager")

	mTmpl = tmpl;
	reset();
}

Chaser::~Chaser()
{
}

ComponentFamilyType Chaser::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Chaser::componentType() const
{
	return mTmpl->componentType();
}

bool Chaser::initialize()
{
	bool result = true;
	//get settings from template
	//enabling setting
	mStartEnabled = (
			mTmpl->parameter(std::string("enabled")) == std::string("true") ?
					true : false);
	//backward setting
	mBackward = (
			mTmpl->parameter(std::string("backward")) == std::string("true") ?
					true : false);
	//fixed relative position setting
	mFixedRelativePosition = (
			mTmpl->parameter(std::string("fixed_relative_position"))
					== std::string("false") ? false : true);
	//look at distance and height settings
	mAbsLookAtDistance = (float) strtof(
			mTmpl->parameter(std::string("abs_lookat_distance")).c_str(), NULL);
	mAbsLookAtHeight = (float) strtof(
			mTmpl->parameter(std::string("abs_lookat_height")).c_str(), NULL);
	//max and min distance settings
	mAbsMaxDistance = (float) strtof(
			mTmpl->parameter(std::string("abs_max_distance")).c_str(), NULL);
	mAbsMinDistance = (float) strtof(
			mTmpl->parameter(std::string("abs_min_distance")).c_str(), NULL);
	//max and min height settings
	mAbsMaxHeight = (float) strtof(
			mTmpl->parameter(std::string("abs_max_height")).c_str(), NULL);
	mAbsMinHeight = (float) strtof(
			mTmpl->parameter(std::string("abs_min_height")).c_str(), NULL);
	//friction' settings
	mFriction = (float) strtof(mTmpl->parameter(std::string("friction")).c_str(), NULL);
	//chased object id
	mChasedId = ObjectId(
			mTmpl->parameter(std::string("chased_object")));
	//reference object id
	mReferenceId = ObjectId(
			mTmpl->parameter(std::string("reference_object")));
	//
	return result;
}

Chaser::Result Chaser::enable()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//if enabled return
	RETURN_ON_COND(mEnabled, Result::ERROR)

	//if chased node path is empty return
	RETURN_ON_COND(mChasedNodePath.is_empty(), Result::ERROR)

	//actual ebnabling
	doEnable();
	//
	return Result::OK;
}

void Chaser::doEnable()
{
	//check kinematic parameters
	if (mAbsMaxDistance < 0.0)
	{
		mAbsMaxDistance = 0.0;
	}
	if ((mAbsMinDistance > mAbsMaxDistance) or (mAbsMinDistance < 0.0))
	{
		mAbsMinDistance = mAbsMaxDistance;
	}
	if (mAbsMaxHeight < 0.0)
	{
		mAbsMaxHeight = 0.0;
	}
	if ((mAbsMinHeight > mAbsMaxHeight) or (mAbsMinHeight < 0.0))
	{
		mAbsMinHeight = mAbsMaxHeight;
	}
	if (mAbsLookAtDistance < 0.0)
	{
		mAbsLookAtDistance = 0.0;
	}
	if (mAbsLookAtHeight < 0.0)
	{
		mAbsLookAtHeight = 0.0;
	}
	//check if backward located
	float sign = (mBackward ? 1.0 : -1.0);
	//set chaser position (wrt chased node)
	mChaserPosition = LPoint3f(0.0, -mAbsMaxDistance * sign,
			mAbsMinHeight * 1.5);
	//set "look at" position (wrt chased node)
	mLookAtPosition = LPoint3f(0.0, mAbsLookAtDistance * sign,
			mAbsLookAtHeight);
	//
	mEnabled = true;
	//register event callbacks if any
	registerEventCallbacks();

	//add to the control manager update
	GameControlManager::GetSingletonPtr()->addToControlUpdate(this);
}

Chaser::Result Chaser::disable()
{
	{
		//lock (guard) the mutex
		HOLD_MUTEX(mMutex)

		//if disabling return
		RETURN_ON_ASYNC_COND(mDisabling, Result::CHASER_DISABLING)

		//if not enabled return
		RETURN_ON_COND(not mEnabled, Result::ERROR)

		//if chased node path is empty return
		RETURN_ON_COND(mChasedNodePath.is_empty(), Result::ERROR)

#ifdef ELY_THREAD
		mDisabling = true;
#endif
	}

	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);

	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//actual disabling
	doDisable();
	//
	return Result::OK;
}

void Chaser::doDisable()
{
	//unregister event callbacks if any
	unregisterEventCallbacks();
	//
#ifdef ELY_THREAD
		mDisabling = false;
#endif
	mEnabled = false;
}

void Chaser::onAddToObjectSetup()
{
	//set the (node path of) object chased by this component;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not, this component chases nothing.
	SMARTPTR(Object)chasedObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mChasedId);
	if (chasedObject != NULL)
	{
		mChasedNodePath = chasedObject->getNodePath();

		//set the (node path of) reference object;
		//that object is supposed to be already created,
		//set up and added to the created objects table;
		//if not, this will be the parent of the chased object.
		SMARTPTR(Object)referenceObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				mReferenceId);
		if (referenceObject != NULL)
		{
			mReferenceNodePath = referenceObject->getNodePath();
		}
		else
		{
			mReferenceNodePath = mChasedNodePath.get_parent();
		}
	}
}

void Chaser::onRemoveFromObjectCleanup()
{
	//
	reset();
}

void Chaser::onAddToSceneSetup()
{
	//if chased node path is empty return
	RETURN_ON_COND(mChasedNodePath.is_empty(),)

	//enable the component (if requested)
	if (mStartEnabled)
	{
		doEnable();
	}
	else
	{
		unregisterEventCallbacks();
	}

	//set chaser initial position/orientation
	mOwnerObject->getNodePath().set_pos(mChasedNodePath, mChaserPosition);
	mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
			LVector3::up());
}

void Chaser::onRemoveFromSceneCleanup()
{
	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);
}

void Chaser::update(void* data)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//update chaser position and orientation (see OgreBulletDemos)
	//position
	LPoint3f currentChaserPos = mReferenceNodePath.get_relative_point(
			mOwnerObject->getNodePath(), LPoint3f::zero());
	LPoint3f newPos;
	LPoint3f desiredChaserPos;
	if (mFixedRelativePosition)
	{
		//follow chased node from fixed position wrt it
		desiredChaserPos = mReferenceNodePath.get_relative_point(
				mChasedNodePath, mChaserPosition);
		newPos = doGetChaserPos(desiredChaserPos, currentChaserPos, dt);
		doCorrectChaserHeight(newPos);
	}
	else
	{
		//correct position only if distance < min distance or distance > max distance
		LPoint3f currentChasedPos = mReferenceNodePath.get_relative_point(
				mChasedNodePath, LPoint3f::zero());
		LVector3f distanceDir = currentChaserPos - currentChasedPos;
		float distance = distanceDir.length();
		if (distance < mAbsMinDistance)
		{
			distanceDir.normalize();
			desiredChaserPos = currentChasedPos + distanceDir * mAbsMinDistance;
			newPos = doGetChaserPos(desiredChaserPos, currentChaserPos, dt);
			doCorrectChaserHeight(newPos);
		}
		else if (distance > mAbsMaxDistance)
		{
			distanceDir.normalize();
			desiredChaserPos = currentChasedPos + distanceDir * mAbsMaxDistance;
			newPos = doGetChaserPos(desiredChaserPos, currentChaserPos, dt);
			doCorrectChaserHeight(newPos);
		}
		else
		{
			newPos = currentChaserPos;
		}
	}
	//
	mOwnerObject->getNodePath().set_pos(mReferenceNodePath, newPos);
	//orientation
	mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
			LVector3::up());
}

LPoint3f Chaser::doGetChaserPos(LPoint3f desiredChaserPos,
		LPoint3f currentChaserPos, float deltaTime)
{
	float kReductFactor = mFriction * deltaTime;
	//calculate difference between desiredChaserPos and currentChaserPos
	LVector3f deltaPos = currentChaserPos - desiredChaserPos;
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

void Chaser::doCorrectChaserHeight(LPoint3f& newPos)
{
	//correct chaser height (not in OgreBulletDemos)
	LPoint3f downTo = LPoint3f(newPos.get_x(), newPos.get_y(), -1000000.0);
	BulletClosestHitRayResult result =
			GamePhysicsManager::GetSingleton().bulletWorld()->ray_test_closest(
					newPos, downTo);
	float hitPosZ = result.get_hit_pos().get_z();
	if (newPos.get_z() < hitPosZ + mAbsMinHeight)
	{
		newPos.set_z(hitPosZ + mAbsMinHeight);
	}
	else if (newPos.get_z() > hitPosZ + mAbsMaxHeight)
	{
		newPos.set_z(hitPosZ + mAbsMaxHeight);
	}
}

//TypedObject semantics: hardcoded
TypeHandle Chaser::_type_handle;

} // namespace ely
