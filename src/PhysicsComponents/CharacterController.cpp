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
 * \file /Ely/src/PhysicsComponents/CharacterController.cpp
 *
 * \date 30/ott/2012 (17:03:49)
 * \author consultit
 */

#include "PhysicsComponents/CharacterController.h"
#include "PhysicsComponents/CharacterControllerTemplate.h"

CharacterController::CharacterController()
{
	// TODO Auto-generated constructor stub
}

CharacterController::CharacterController(SMARTPTR(CharacterControllerTemplate)tmpl)
{
	CHECKEXISTENCE(GamePhysicsManager::GetSingletonPtr(),
			"CharacterController::CharacterController: invalid GamePhysicsManager")
	mTmpl = tmpl;
	mForward = false;
	mBackward = false;
	mStrafeLeft = false;
	mStrafeRight = false;
	mRollLeft = false;
	mRollRight = false;
	mJump = false;
	//reset events' sending
	mOnGroundSent = false;
	mOnAirSent = false;
}

CharacterController::~CharacterController()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//check if game physics manager exists
	if (GamePhysicsManager::GetSingletonPtr())
	{
		//remove from the physics manager update
		GamePhysicsManager::GetSingletonPtr()->removeFromPhysicsUpdate(this);
		//remove character controller from the physics world
		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
				DCAST(TypedObject, mCharacterController));
	}
	//Remove node path
	mNodePath.remove_node();
}

const ComponentFamilyType CharacterController::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType CharacterController::componentType() const
{
	return mTmpl->componentType();
}

bool CharacterController::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//throw events setting
	mThrowEvents = (
			mTmpl->parameter(std::string("throw_events"))
					== std::string("true") ? true : false);
	//get step height
	mStepHeight = (float) strtof(
			mTmpl->parameter(std::string("step_height")).c_str(), NULL);
	//get shape type
	std::string shapeType = mTmpl->parameter(std::string("shape_type"));
	//get shape size
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
			mDim1 = (float) strtof(radius.c_str(), NULL);
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
			mDim1 = (float) strtof(half_x.c_str(), NULL);
			mDim2 = (float) strtof(half_y.c_str(), NULL);
			mDim3 = (float) strtof(half_z.c_str(), NULL);
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
			mDim1 = (float) strtof(radius.c_str(), NULL);
			mDim2 = (float) strtof(height.c_str(), NULL);
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
	//get collide mask
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
	//set control parameters
	float linearSpeed = (float) strtof(
			mTmpl->parameter(std::string("linear_speed")).c_str(), NULL);
	mLinearSpeed = LVecBase2f(linearSpeed, linearSpeed);
	mIsLocal = (
			mTmpl->parameter(std::string("is_local")) == std::string("false") ?
					false : true);
	mAngularSpeed = (float) strtof(
			mTmpl->parameter(std::string("angular_speed")).c_str(), NULL);
	mFallSpeed = (float) strtof(
			mTmpl->parameter(std::string("fall_speed")).c_str(), NULL);
	mGravity = (float) strtof(mTmpl->parameter(std::string("gravity")).c_str(),
			NULL);
	mJumpSpeed = (float) strtof(
			mTmpl->parameter(std::string("jump_speed")).c_str(), NULL);
	mMaxSlope = (float) strtof(
			mTmpl->parameter(std::string("max_slope")).c_str(), NULL);
	mMaxJumpHeight = (float) strtof(
			mTmpl->parameter(std::string("max_jump_height")).c_str(), NULL);
	//key events setting
	//backward key
	mBackwardKey = (
			mTmpl->parameter(std::string("backward"))
					== std::string("enabled") ? true : false);
	//forward key
	mForwardKey = (
			mTmpl->parameter(std::string("forward")) == std::string("enabled") ?
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
	mRollLeftKey = (
			mTmpl->parameter(std::string("roll_left"))
					== std::string("enabled") ? true : false);
	//rollRight key
	mRollRightKey = (
			mTmpl->parameter(std::string("roll_right"))
					== std::string("enabled") ? true : false);
	//jump key
	mJumpKey = (
			mTmpl->parameter(std::string("jump")) == std::string("enabled") ?
					true : false);
	//use shape of (another object)
	mUseShapeOfId = ObjectId(mTmpl->parameter(std::string("use_shape_of")));
	//
	return result;
}

void CharacterController::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//At this point a Scene component (Model, InstanceOf ...) should have
	//been already created and added to the object, so its node path should
	//be the same as the object's one.
	//Note: scaling is applied to a Scene component, so the object node path
	//has scaling already applied.

	//create a Character Controller Node
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
	mCharacterController = new BulletCharacterControllerNode(
			createShape(mShapeType), mStepHeight, name.c_str());
	//set the control parameters
	setControlParameters();

	//attach it to Bullet World
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			mCharacterController);

	//create a node path for the character controller
	mNodePath = NodePath(mCharacterController);
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
	//Add to the physics manager update
	GamePhysicsManager::GetSingletonPtr()->addToPhysicsUpdate(this);
	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void CharacterController::enableForward(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mForwardKey)
	{
		mForward = enable;
	}
}

bool CharacterController::isForwardEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mForward;
}

void CharacterController::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

bool CharacterController::isBackwardEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mBackward;
}

void CharacterController::enableStrafeLeft(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

bool CharacterController::isStrafeLeftEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mStrafeLeft;
}

void CharacterController::enableStrafeRight(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

bool CharacterController::isStrafeRightEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mStrafeRight;
}

void CharacterController::enableRollLeft(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mRollLeftKey)
	{
		mRollLeft = enable;
	}
}

bool CharacterController::isRollLeftEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mRollLeft;
}

void CharacterController::enableRollRight(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mRollRightKey)
	{
		mRollRight = enable;
	}
}

bool CharacterController::isRollRightEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mRollRight;
}

void CharacterController::enableJump(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mJumpKey)
	{
		mJump = enable;
	}
}

bool CharacterController::isJumpEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mJump;
}

LVecBase2f CharacterController::getLinearSpeed()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mLinearSpeed;
}

void CharacterController::setLinearSpeed(LVecBase2f speed)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mLinearSpeed = speed;
}

float CharacterController::getAngularSpeed()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAngularSpeed;
}

void CharacterController::setAngularSpeed(float speed)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mAngularSpeed = speed;
}

void CharacterController::setIsLocal(bool isLocal)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mIsLocal = isLocal;
}

bool CharacterController::getIsLocal()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsLocal;
}

void CharacterController::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	LVector3 speed(0, 0, 0);
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
	if (mStrafeLeft)
	{
		speed.set_x(-mLinearSpeed.get_x());
	}
	if (mStrafeRight)
	{
		speed.set_x(mLinearSpeed.get_x());
	}
	if (mRollLeft)
	{
		omega = mAngularSpeed;
	}
	if (mRollRight)
	{
		omega = -mAngularSpeed;
	}
	// set movements
	mCharacterController->set_linear_movement(speed, mIsLocal);
	mCharacterController->set_angular_movement(omega);

	//handle CharacterController ground-air
	if (mCharacterController->is_on_ground())
	{
		//throw OnGround event (if enabled)
		if (mThrowEvents and (not mOnGroundSent))
		{
			throw_event(std::string("OnGround"), EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mOnGroundSent = true;
			mOnAirSent = false;
		}
		//jump if requested
		if (mJump)
		{
			mCharacterController->do_jump();
		}
	}
	else
	{
		//throw OffGround event (if enabled)
		if (mThrowEvents and (not mOnAirSent))
		{
			throw_event(std::string("OnAir"), EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mOnAirSent = true;
			mOnGroundSent = false;
		}
	}
}

NodePath CharacterController::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void CharacterController::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

SMARTPTR(BulletShape)CharacterController::createShape(GamePhysicsManager::ShapeType shapeType)
{
	//check if it should use shape of another (already) created object
	if (not mUseShapeOfId.empty())
	{
		SMARTPTR(Object)createdObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				mUseShapeOfId);
		if (createdObject != NULL)
		{
			SMARTPTR(Component) component =
			createdObject->getComponent(ComponentFamilyType("Physics"));
			if (component->is_of_type(CharacterController::get_class_type()))
			{
				//object already exists
				SMARTPTR(CharacterController)characterController =
				DCAST(CharacterController, component);
				if (characterController != NULL)
				{
					//physics component is a character controller:
					//return a reference to its shape
					return characterController->mCharacterController->get_shape();
				}
			}
		}
	}

	// create and return the current shape
	return GamePhysicsManager::GetSingletonPtr()->createShape(
			mOwnerObject->getNodePath(), mShapeType, mShapeSize,
			mModelDims, mModelDeltaCenter, mModelRadius, mDim1, mDim2,
			mDim3, mDim4, mAutomaticShaping, mUpAxis);
}

void CharacterController::setControlParameters()
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

//TypedObject semantics: hardcoded
TypeHandle CharacterController::_type_handle;

