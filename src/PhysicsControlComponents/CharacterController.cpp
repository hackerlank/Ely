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
 * \file /Ely/src/PhysicsControlComponents/CharacterController.cpp
 *
 * \date 30/ott/2012 (17:03:49)
 * \author consultit
 */

#include "PhysicsControlComponents/CharacterController.h"
#include "ObjectModel/ObjectTemplateManager.h"

namespace ely
{

CharacterController::CharacterController(SMARTPTR(CharacterControllerTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"CharacterController::CharacterController: invalid GamePhysicsManager")

	mTmpl = tmpl;
	reset();
}

CharacterController::~CharacterController()
{
}

bool CharacterController::initialize()
{
	bool result = true;
	//
	float value, absValue;
	//step height
	value = strtof(mTmpl->parameter(std::string("step_height")).c_str(), NULL);
	mStepHeight = (value >= 0.0 ? value : -value);
	//shape type
	std::string shapeType = mTmpl->parameter(std::string("shape_type"));
	//shape size
	std::string shapeSize = mTmpl->parameter(std::string("shape_size"));
	if (shapeSize == std::string("minimum"))
	{
		mShapeSize = GamePhysicsManager::MINIMUN;
	}
	else if (shapeSize == std::string("maximum"))
	{
		mShapeSize = GamePhysicsManager::MAXIMUM;
	}
	else
	{
		mShapeSize = GamePhysicsManager::MEDIUM;
	}
	//default auto shaping
	mAutomaticShaping = true;
	if (shapeType == std::string("sphere"))
	{
		mShapeType = GamePhysicsManager::SPHERE;
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		if (not radius.empty())
		{
			mDim1 = strtof(radius.c_str(), NULL);
			if (mDim1 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == std::string("box"))
	{
		mShapeType = GamePhysicsManager::BOX;
		std::string half_x = mTmpl->parameter(std::string("shape_half_x"));
		std::string half_y = mTmpl->parameter(std::string("shape_half_y"));
		std::string half_z = mTmpl->parameter(std::string("shape_half_z"));
		if ((not half_x.empty()) and (not half_y.empty())
				and (not half_z.empty()))
		{
			mDim1 = strtof(half_x.c_str(), NULL);
			mDim2 = strtof(half_y.c_str(), NULL);
			mDim3 = strtof(half_z.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == std::string("cylinder")
			or shapeType == std::string("capsule")
			or shapeType == std::string("cone"))
	{
		if (shapeType == std::string("cylinder"))
		{
			mShapeType = GamePhysicsManager::CYLINDER;
		}
		else if (shapeType == std::string("capsule"))
		{
			mShapeType = GamePhysicsManager::CAPSULE;
		}
		else
		{
			mShapeType = GamePhysicsManager::CONE;
		}
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		std::string height = mTmpl->parameter(std::string("shape_height"));
		std::string upAxis = mTmpl->parameter(std::string("shape_up"));
		if ((not radius.empty()) and (not height.empty()))
		{
			mDim1 = strtof(radius.c_str(), NULL);
			mDim2 = strtof(height.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
		if (upAxis == std::string("x"))
		{
			mUpAxis = X_up;
		}
		else if (upAxis == std::string("y"))
		{
			mUpAxis = Y_up;
		}
		else
		{
			mUpAxis = Z_up;
		}
	}
	else
	{
		//default a sphere (with auto shaping)
		mShapeType = GamePhysicsManager::SPHERE;
	}
	//collide mask
	std::string collideMask = mTmpl->parameter(std::string("collide_mask"));
	if (collideMask == std::string("all_on"))
	{
		mCollideMask = BitMask32::all_on();
	}
	else if (collideMask == std::string("all_off"))
	{
		mCollideMask = BitMask32::all_off();
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(collideMask.c_str(), NULL, 0);
		mCollideMask.set_word(mask);
#ifdef ELY_DEBUG
		mCollideMask.write(std::cout, 0);
#endif
	}
	//linear speed
	value = strtof(mTmpl->parameter(std::string("linear_speed")).c_str(), NULL);
	absValue = (value >= 0.0 ? value : -value);
	mLinearSpeed = LVecBase3f(absValue, absValue, absValue);
	//is local
	mIsLocal = (
			mTmpl->parameter(std::string("is_local")) == std::string("false") ?
					false : true);
	//angular speed
	value = strtof(mTmpl->parameter(std::string("angular_speed")).c_str(),
	NULL);
	mAngularSpeed = (value >= 0.0 ? value : -value);
	//fall speed
	value = strtof(mTmpl->parameter(std::string("fall_speed")).c_str(), NULL);
	mFallSpeed = (value >= 0.0 ? value : -value);
	//gravity
	value = strtof(mTmpl->parameter(std::string("gravity")).c_str(),
	NULL);
	mGravity = (value >= 0.0 ? value : -value);
	//jump speed
	value = strtof(mTmpl->parameter(std::string("jump_speed")).c_str(), NULL);
	mJumpSpeed = (value >= 0.0 ? value : -value);
	//max slope
	value = strtof(mTmpl->parameter(std::string("max_slope")).c_str(), NULL);
	mMaxSlope = (value >= 0.0 ? value : -value);
	//max jump height
	value = strtof(mTmpl->parameter(std::string("max_jump_height")).c_str(),
	NULL);
	mMaxJumpHeight = (value >= 0.0 ? value : -value);
	//key events setting
	//backward key
	mBackwardKey = (
			mTmpl->parameter(std::string("backward"))
					== std::string("enabled") ? true : false);
	//forward key
	mForwardKey = (
			mTmpl->parameter(std::string("forward")) == std::string("enabled") ?
					true : false);
	//up key
	mUpKey = (
			mTmpl->parameter(std::string("up")) == std::string("enabled") ?
					true : false);
	//down key
	mDownKey = (
			mTmpl->parameter(std::string("down")) == std::string("enabled") ?
					true : false);
	//strafeLeft key
	mStrafeLeftKey = (
			mTmpl->parameter(std::string("strafe_left"))
					== std::string("enabled") ? true : false);
	//strafeRight key
	mStrafeRightKey = (
			mTmpl->parameter(std::string("strafe_right"))
					== std::string("enabled") ? true : false);
	//rollLeft key
	mHeadLeftKey = (
			mTmpl->parameter(std::string("head_left"))
					== std::string("enabled") ? true : false);
	//rollRight key
	mHeadRightKey = (
			mTmpl->parameter(std::string("head_right"))
					== std::string("enabled") ? true : false);
	//jump key
	mJumpKey = (
			mTmpl->parameter(std::string("jump")) == std::string("enabled") ?
					true : false);
	//use shape of (another object)
	mUseShapeOfId = ObjectId(mTmpl->parameter(std::string("use_shape_of")));
	//thrown events
	mThrownEventsParam = mTmpl->parameter(std::string("thrown_events"));
	//
	return result;
}

void CharacterController::onAddToObjectSetup()
{
	//At this point a Scene component (Model, InstanceOf ...) should have
	//been already created and added to the object, so its node path should
	//be the same as the object's one.
	//Note: scaling is applied to a Scene component, so the object node path
	//has scaling already applied.

	//create a Character Controller Node
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
	mCharacterController = new BulletCharacterControllerNode(
			doCreateShape(mShapeType), mStepHeight, name.c_str());
	//add to table of all physics components indexed by
	//(underlying) Bullet PandaNodes
	GamePhysicsManager::GetSingletonPtr()->setPhysicsComponentByPandaNode(
			mCharacterController.p(), this);

	//set the control parameters
	doSetControlParameters();

	//create a node path for the character controller
	mNodePath = NodePath(mCharacterController);

	HOLD_REMUTEX(GamePhysicsManager::GetSingletonPtr()->getMutex())
	{
		//attach it to Bullet World
		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
				mCharacterController);
	}

	//set collide mask
	mNodePath.set_collide_mask(mCollideMask);

	NodePath ownerNodePath = mOwnerObject->getNodePath();
	if (not ownerNodePath.is_empty())
	{
		//reparent the object node path as a child of the character controller's one
		ownerNodePath.reparent_to(mNodePath);
		//correct (or possibly reset to zero) pos and hpr of the former object node path
		ownerNodePath.set_pos_hpr(mModelDeltaCenter, LVecBase3::zero());
		//optimize
		mNodePath.flatten_light();
	}

	//set this character controller node path as the object's one
	mOwnerObject->setNodePath(mNodePath);

	//set thrown events if any
	unsigned int idx1, valueNum1;
	std::vector<std::string> paramValuesStr1, paramValuesStr2;
	if (mThrownEventsParam != std::string(""))
	{
		//events specified
		//event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]
		paramValuesStr1 = parseCompoundString(mThrownEventsParam, ':');
		valueNum1 = paramValuesStr1.size();
		for (idx1 = 0; idx1 < valueNum1; ++idx1)
		{
			//eventX@[event_nameX]@[frequencyX]
			paramValuesStr2 = parseCompoundString(paramValuesStr1[idx1], '@');
			if (paramValuesStr2.size() >= 3)
			{
				EventThrown event;
				ThrowEventData eventData;
				//get default name prefix
				std::string objectType = std::string(
						mOwnerObject->objectTmpl()->objectType());
				//get name
				std::string name = paramValuesStr2[1];
				//get frequency
				float frequency = strtof(paramValuesStr2[2].c_str(), NULL);
				if (frequency <= 0.0)
				{
					frequency = 30.0;
				}
				//get event
				if (paramValuesStr2[0] == "on_ground")
				{
					event = ONGROUNDEVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_CharacterController_OnGround";
					}
				}
				else if (paramValuesStr2[0] == "in_air")
				{
					event = INAIREVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_CharacterController_InAir";
					}
				}
				else
				{
					//paramValuesStr2[0] is not a suitable event:
					//continue with the next event
					continue;
				}
				//set event data
				eventData.mEnable = true;
				eventData.mEventName = name;
				eventData.mTimeElapsed = 0;
				eventData.mFrequency = frequency;
				//enable the event
				doEnableCharacterControllerEvent(event, eventData);
			}
		}
	}
	//clear all no more needed "Param" variables
	mThrownEventsParam.clear();
}

void CharacterController::onRemoveFromObjectCleanup()
{
	NodePath oldObjectNodePath;
	//set the object node path to the first child of rigid body's one (if any)
	if (mNodePath.get_num_children() > 0)
	{
		oldObjectNodePath = mNodePath.get_child(0);
		//detach the object node path from the rigid body's one
		oldObjectNodePath.detach_node();
	}
	else
	{
		oldObjectNodePath = NodePath();
	}
	//set the object node path to the old one
	mOwnerObject->setNodePath(oldObjectNodePath);

	//remove from table of all physics components indexed by
	//(underlying) Bullet PandaNodes
	GamePhysicsManager::GetSingletonPtr()->setPhysicsComponentByPandaNode(
			mCharacterController.p(), NULL);

	HOLD_REMUTEX(GamePhysicsManager::GetSingletonPtr()->getMutex())
	{
		//remove character controller from the physics world
		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(mCharacterController);
	}

	//Remove node path
	mNodePath.remove_node();
	//
	reset();
}

void CharacterController::onAddToSceneSetup()
{
	//Add to the physics manager update
	GamePhysicsManager::GetSingletonPtr()->addToPhysicsUpdate(this);
}

void CharacterController::onRemoveFromSceneCleanup()
{
	//remove from the physics manager update
	GamePhysicsManager::GetSingletonPtr()->removeFromPhysicsUpdate(this);
}

void CharacterController::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

#ifdef TESTING
	float dt = *(reinterpret_cast<float*>(data));

	dt = 0.016666667; //60 fps
#endif

	LVector3f speed(0, 0, 0);
	float omega = 0.0;

	//handle keys:
	if (mForward)
	{
		speed.set_y(-mLinearSpeed.get_y());
	}
	if (mBackward)
	{
		speed.set_y(mLinearSpeed.get_y());
	}
	if (mUp)
	{
		speed.set_z(mLinearSpeed.get_z());
	}
	if (mDown)
	{
		speed.set_z(-mLinearSpeed.get_z());
	}
	if (mStrafeLeft)
	{
		speed.set_x(-mLinearSpeed.get_x());
	}
	if (mStrafeRight)
	{
		speed.set_x(mLinearSpeed.get_x());
	}
	if (mHeadLeft)
	{
		omega = mAngularSpeed;
	}
	if (mHeadRight)
	{
		omega = -mAngularSpeed;
	}
	// set movements
	mCharacterController->set_linear_movement(speed, mIsLocal);
	mCharacterController->set_angular_movement(omega);

	//handle CharacterController ground-air events
	if (mCharacterController->is_on_ground())
	{
		//handle events
		//throw OnGround event (if enabled)
		if (mOnGround.mEnable)
		{
			doThrowEvent(mOnGround);
		}
		//reset InAir event (if enabled and if thrown)
		if (mInAir.mEnable and mInAir.mThrown)
		{
			mInAir.mThrown = false;
			mInAir.mTimeElapsed = 0.0;
		}

		//take other actions
		//jump if requested
		if (mJump)
		{
			mCharacterController->do_jump();
		}
	}
	else
	{
		//handle events
		//reset OnGround event (if enabled and if thrown)
		if (mOnGround.mEnable and mOnGround.mThrown)
		{
			mOnGround.mThrown = false;
			mOnGround.mTimeElapsed = 0.0;
		}
		//throw InAir event (if enabled)
		if (mInAir.mEnable)
		{
			doThrowEvent(mInAir);
		}
	}
}

SMARTPTR(BulletShape)CharacterController::doCreateShape(GamePhysicsManager::ShapeType shapeType)
{
	//check if it should use shape of another (already) created object
	if (not mUseShapeOfId.empty())
	{
		SMARTPTR(Object)createdObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				mUseShapeOfId);
		if (createdObject != NULL)
		{
			//object already exists
			SMARTPTR(Component)physicsControlComp =
					createdObject->getComponent(ComponentFamilyType("PhysicsControl"));
			if(physicsControlComp and physicsControlComp->is_of_type(CharacterController::get_class_type()))
			{
				//physics component is a character controller:
				//return a reference to its shape
				return DCAST(CharacterController,
						physicsControlComp)->mCharacterController->get_shape();
			}
		}
	}

	// create and return the current shape
	return GamePhysicsManager::GetSingletonPtr()->createShape(
			mOwnerObject->getNodePath(), mShapeType, mShapeSize,
			mModelDims, mModelDeltaCenter, mModelRadius, mDim1, mDim2,
			mDim3, mDim4, mAutomaticShaping, mUpAxis);
}

void CharacterController::doSetControlParameters()
{
	mCharacterController->set_fall_speed(mFallSpeed);
	mCharacterController->set_gravity(mGravity);
	mCharacterController->set_jump_speed(mJumpSpeed);
	mCharacterController->set_max_slope(mMaxSlope);
	if (mMaxJumpHeight > 0.0)
	{
		mCharacterController->set_max_jump_height(mMaxJumpHeight);
	}
}

void CharacterController::doEnableCharacterControllerEvent(EventThrown event, ThrowEventData eventData)
{
	//some checks
	RETURN_ON_COND(eventData.mEventName == std::string(""),)
	if (eventData.mFrequency <= 0.0)
	{
		eventData.mFrequency = 30.0;
	}

	switch (event)
	{
	case ONGROUNDEVENT:
		if(mOnGround.mEnable != eventData.mEnable)
		{
			mOnGround = eventData;
			mOnGround.mTimeElapsed = 0;
		}
		break;
	case INAIREVENT:
		if(mInAir.mEnable != eventData.mEnable)
		{
			mInAir = eventData;
			mInAir.mTimeElapsed = 0;
		}
		break;
	default:
		break;
	}
}

//TypedObject semantics: hardcoded
TypeHandle CharacterController::_type_handle;

///Template

CharacterControllerTemplate::CharacterControllerTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"CharacterTemplate::CharacterTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"CharacterTemplate::CharacterTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"CharacterTemplate::CharacterTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

CharacterControllerTemplate::~CharacterControllerTemplate()
{
	
}

ComponentType CharacterControllerTemplate::componentType() const
{
	return ComponentType(CharacterController::get_class_type().get_name());
}

ComponentFamilyType CharacterControllerTemplate::familyType() const
{
	return ComponentFamilyType("PhysicsControl");
}

SMARTPTR(Component)CharacterControllerTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(CharacterController) newCharacter = new CharacterController(this);
	newCharacter->setComponentId(compId);
	if (not newCharacter->initialize())
	{
		return NULL;
	}
	return newCharacter.p();
}

void CharacterControllerTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("step_height", "1.0"));
	mParameterTable.insert(ParameterNameValue("collide_mask", "all_on"));
	mParameterTable.insert(ParameterNameValue("shape_type", "sphere"));
	mParameterTable.insert(ParameterNameValue("shape_size", "medium"));
	mParameterTable.insert(ParameterNameValue("shape_up", "z"));
	mParameterTable.insert(ParameterNameValue("fall_speed", "55.0"));
	mParameterTable.insert(ParameterNameValue("gravity", "29.4"));
	mParameterTable.insert(ParameterNameValue("jump_speed", "10.0"));
	mParameterTable.insert(ParameterNameValue("max_slope", "45.0"));
	mParameterTable.insert(ParameterNameValue("forward", "enabled"));
	mParameterTable.insert(ParameterNameValue("backward", "enabled"));
	mParameterTable.insert(ParameterNameValue("up", "enabled"));
	mParameterTable.insert(ParameterNameValue("down", "enabled"));
	mParameterTable.insert(ParameterNameValue("head_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("head_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
	mParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
	mParameterTable.insert(ParameterNameValue("jump", "enabled"));
	mParameterTable.insert(ParameterNameValue("linear_speed", "10.0"));
	mParameterTable.insert(ParameterNameValue("angular_speed", "45.0"));
	mParameterTable.insert(ParameterNameValue("is_local", "true"));
}

//TypedObject semantics: hardcoded
TypeHandle CharacterControllerTemplate::_type_handle;

}  // namespace ely
