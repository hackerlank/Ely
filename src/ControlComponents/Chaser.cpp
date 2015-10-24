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
#include "Game/GameControlManager.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

Chaser::Chaser(SMARTPTR(ChaserTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameControlManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GameControlManager")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"Chaser::Chaser: invalid GamePhysicsManager")

	mTmpl = tmpl;
	reset();
}

Chaser::~Chaser()
{
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
	//
	float value;
	//abs max distance
	value = strtof(mTmpl->parameter(std::string("abs_max_distance")).c_str(),
			NULL);
	mAbsMaxDistance = (value >= 0.0 ? value : -value);
	//abs min distance
	value = strtof(mTmpl->parameter(std::string("abs_min_distance")).c_str(),
			NULL);
	mAbsMinDistance = (value >= 0.0 ? value : -value);
	//abs max height
	value = strtof(mTmpl->parameter(std::string("abs_max_height")).c_str(),
			NULL);
	mAbsMaxHeight = (value >= 0.0 ? value : -value);
	//abs min height
	value = strtof(mTmpl->parameter(std::string("abs_min_height")).c_str(),
			NULL);
	mAbsMinHeight = (value >= 0.0 ? value : -value);
	//friction
	value = strtof(mTmpl->parameter(std::string("friction")).c_str(), NULL);
	mFriction = (value >= 0.0 ? value : -value);
	//chased object id
	mChasedId = ObjectId(mTmpl->parameter(std::string("chased_object")));
	//reference object id
	mReferenceId = ObjectId(mTmpl->parameter(std::string("reference_object")));
	//fixed look at
	mFixedLookAt = (
			mTmpl->parameter(std::string("fixed_lookat"))
					== std::string("false") ? false : true);
	//abs lookat distance
	value = strtof(mTmpl->parameter(std::string("abs_lookat_distance")).c_str(),
			NULL);
	mAbsLookAtDistance = (value >= 0.0 ? value : -value);
	//abs lookat height
	value = strtof(mTmpl->parameter(std::string("abs_lookat_height")).c_str(),
			NULL);
	mAbsLookAtHeight = (value >= 0.0 ? value : -value);
	//mouse movement setting
	mMouseEnabledH = (
			mTmpl->parameter(std::string("mouse_enabled_h"))
					== std::string("true") ? true : false);
	mMouseEnabledP = (
			mTmpl->parameter(std::string("mouse_enabled_p"))
					== std::string("true") ? true : false);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->parameter(std::string("head_left"))
					== std::string("enabled") ? true : false);
	//headRight key
	mHeadRightKey = (
			mTmpl->parameter(std::string("head_right"))
					== std::string("enabled") ? true : false);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->parameter(std::string("pitch_up"))
					== std::string("enabled") ? true : false);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->parameter(std::string("pitch_down"))
					== std::string("enabled") ? true : false);
	//inverted rotation
	mSignOfMouse = (
			mTmpl->parameter(std::string("inverted_rotation"))
					== std::string("true") ? -1 : 1);
	//sens x
	value = strtof(mTmpl->parameter(std::string("sens_x")).c_str(),
	NULL);
	mSensX = (value >= 0.0 ? value : -value);
	mHeadSensX = mSensX * 375.0;
	//sens y
	value = strtof(mTmpl->parameter(std::string("sens_y")).c_str(),
	NULL);
	mSensY = (value >= 0.0 ? value : -value);
	mHeadSensY = mSensY * 375.0;
	//
	return result;
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
	//
	GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
	mCentX = win->get_properties().get_x_size() / 2;
	mCentY = win->get_properties().get_y_size() / 2;
}

void Chaser::onRemoveFromObjectCleanup()
{
	//see disable
	if (mEnabled and (not mFixedLookAt) and (mMouseEnabledH or mMouseEnabledP))
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}
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
		//when enabled set chaser initial position/orientation
		mOwnerObject->getNodePath().set_pos(mChasedNodePath, mChaserPosition);
		mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
				LVector3f::up());
	}
	else
	{
		unregisterEventCallbacks();
	}
}

void Chaser::onRemoveFromSceneCleanup()
{
	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);
}

Chaser::Result Chaser::enable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

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
	//check if backward located
	float sign = (mBackward ? 1.0 : -1.0);
	//set chaser position (wrt chased node)
	mChaserPosition = LPoint3f(0.0, -mAbsMaxDistance * sign,
			mAbsMinHeight * 1.5);
	//set "look at" position (wrt chased node)
	mLookAtPosition = LPoint3f(0.0, mAbsLookAtDistance * sign,
			mAbsLookAtHeight);
	//
	if ((not mFixedLookAt) and (mMouseEnabledH or mMouseEnabledP))
	{
		//we want control through mouse movements
		//hide mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(true);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
		//reset mouse to start position
		win->move_pointer(0, mCentX, mCentY);
	}
	//
	mFixedLookAtNodePath = mOwnerObject->getNodePath().get_parent().attach_new_node(
			"fixedLookAtNP");
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
		HOLD_REMUTEX(mMutex)

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
	HOLD_REMUTEX(mMutex)

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
	mFixedLookAtNodePath.remove_node();
	//
	if ((not mFixedLookAt) and (mMouseEnabledH or mMouseEnabledP))
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}
	//
#ifdef ELY_THREAD
	mDisabling = false;
#endif
	//
	mEnabled = false;
}

void Chaser::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

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
		LPoint3f currentChasedPos = mReferenceNodePath.get_relative_point(
				mChasedNodePath, LPoint3f::zero());
		doCorrectChaserHeight(newPos, currentChasedPos.get_z());
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
			doCorrectChaserHeight(newPos, currentChasedPos.get_z());
		}
		else if (distance > mAbsMaxDistance)
		{
			distanceDir.normalize();
			desiredChaserPos = currentChasedPos + distanceDir * mAbsMaxDistance;
			newPos = doGetChaserPos(desiredChaserPos, currentChaserPos, dt);
			doCorrectChaserHeight(newPos, currentChasedPos.get_z());
		}
		else
		{
			newPos = currentChaserPos;
		}
	}
	//
	mOwnerObject->getNodePath().set_pos(mReferenceNodePath, newPos);
	//orientation
	if (mFixedLookAt)
	{
		//look at fixed location
		mOwnerObject->getNodePath().look_at(mChasedNodePath, mLookAtPosition,
				LVector3f::up());
	}
	else
	{
		//adjust look at with mouse and/or key events
		//handle mouse
		float deltaH = 0.0, deltaP = 0.0, deltaR = 0.0;
		bool wantRotate = false;
		if (mMouseEnabledH or mMouseEnabledP)
		{
			GraphicsWindow* win =
					mTmpl->windowFramework()->get_graphics_window();
			MouseData md = win->get_pointer(0);
			float deltaX = md.get_x() - mCentX;
			float deltaY = md.get_y() - mCentY;

			if (win->move_pointer(0, mCentX, mCentY))
			{
				if (mMouseEnabledH and (deltaX != 0.0))
				{
					deltaH -= deltaX * mSensX * mSignOfMouse;
					wantRotate = true;
				}
				if (mMouseEnabledP and (deltaY != 0.0))
				{
					deltaP -= deltaY * mSensY * mSignOfMouse;
					wantRotate = true;
				}
			}
		}

		//handle keys:
		if (mHeadLeft and (not mHeadRight))
		{
			deltaH += mHeadSensX * dt * mSignOfMouse;
			wantRotate = true;
		}
		else if (mHeadRight and (not mHeadLeft))
		{
			deltaH -= mHeadSensX * dt * mSignOfMouse;
			wantRotate = true;
		}
		if (mPitchUp and (not mPitchDown))
		{
			deltaP += mHeadSensY * dt * mSignOfMouse;
			wantRotate = true;
		}
		else if (mPitchDown and (not mPitchUp))
		{
			deltaP -= mHeadSensY * dt * mSignOfMouse;
			wantRotate = true;
		}

		//update look at
		if (wantRotate)
		{
			//want rotate: update to desired look up
			mOwnerObject->getNodePath().set_hpr(
					mOwnerObject->getNodePath().get_hpr()
							+ LVecBase3f(deltaH, deltaP, deltaR));
		}
		else if (not mHoldLookAt)
		{
			//don't want rotate: return to look up to fixed location
			mFixedLookAtNodePath.set_pos(mOwnerObject->getNodePath().get_pos());
			mFixedLookAtNodePath.look_at(mChasedNodePath, mLookAtPosition,
					LVector3f::up());
			LVecBase3f deltaHPR = mOwnerObject->getNodePath().get_hpr(mFixedLookAtNodePath);
			float kReductFactor = mFriction * dt;
			if (kReductFactor > 1.0)
			{
				kReductFactor = 1.0;
			}
			if (deltaHPR.length_squared() > 0.0)
			{
				deltaHPR -= deltaHPR * kReductFactor;
			}
			mOwnerObject->getNodePath().set_hpr(mFixedLookAtNodePath, deltaHPR);
		}
	}
}

LPoint3f Chaser::doGetChaserPos(LPoint3f desiredChaserPos,
		LPoint3f currentChaserPos, float deltaTime)
{
	float kReductFactor = mFriction * deltaTime;
	if (kReductFactor > 1.0)
	{
		kReductFactor = 1.0;
	}
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

void Chaser::doCorrectChaserHeight(LPoint3f& newPos, float baseHeight)
{
	//correct chaser height (not in OgreBulletDemos)
	LPoint3f downTo = LPoint3f(newPos.get_x(), newPos.get_y(), -1000000.0);
	BulletClosestHitRayResult result =
			GamePhysicsManager::GetSingleton().bulletWorld()->ray_test_closest(
					newPos, downTo);
	float hitPosZ = max(result.get_hit_pos().get_z(), baseHeight);
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

///Template

ChaserTemplate::ChaserTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"DriverTemplate::ChaserTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"DriverTemplate::ChaserTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameControlManager::GetSingletonPtr(),
			"DriverTemplate::ChaserTemplate: invalid GameControlManager")
	//
	setParametersDefaults();
}

ChaserTemplate::~ChaserTemplate()
{
	
}

ComponentType ChaserTemplate::componentType() const
{
	return ComponentType(Chaser::get_class_type().get_name());
}

ComponentFamilyType ChaserTemplate::componentFamilyType() const
{
	return ComponentFamilyType("Control");
}

SMARTPTR(Component)ChaserTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Chaser) newChaser = new Chaser(this);
	newChaser->setComponentId(compId);
	if (not newChaser->initialize())
	{
		return NULL;
	}
	return newChaser.p();
}

void ChaserTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("enabled", "true"));
	mParameterTable.insert(ParameterNameValue("backward", "true"));
	mParameterTable.insert(ParameterNameValue("fixed_relative_position", "true"));
	mParameterTable.insert(ParameterNameValue("friction", "1.0"));
	mParameterTable.insert(ParameterNameValue("fixed_lookat", "true"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_h", "false"));
	mParameterTable.insert(ParameterNameValue("mouse_enabled_p", "false"));
	mParameterTable.insert(ParameterNameValue("head_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("head_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
	mParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
	mParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
	mParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
	mParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
}

//TypedObject semantics: hardcoded
TypeHandle ChaserTemplate::_type_handle;

} // namespace ely
