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
 * \date 30/ott/2012 17:03:49
 * \author marco
 */

#include "PhysicsComponents/CharacterController.h"
#include "PhysicsComponents/CharacterControllerTemplate.h"

CharacterController::CharacterController()
{
	// TODO Auto-generated constructor stub
}

CharacterController::CharacterController(SMARTPTR(CharacterControllerTemplate)tmpl)
{
	mTmpl = tmpl;
	mForward = false;
	mBackward = false;
	mStrafeLeft = false;
	mStrafeRight = false;
	mRollLeft = false;
	mRollRight = false;
	mJump = false;
}

CharacterController::~CharacterController()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//first check if game physics manager exists
	if (GamePhysicsManager::GetSingletonPtr())
	{
		//Remove from the physics manager update
		GamePhysicsManager::GetSingletonPtr()->removeFromPhysicsUpdate(this);
		//Remove character controller from the physics world
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
	//get step height
	mStepHeight = (float) atof(
			mTmpl->parameter(std::string("step_height")).c_str());
	//get shape type
	std::string shapeType = mTmpl->parameter(std::string("shape_type"));
	//default auto shaping
	mAutomaticShaping = true;
	if (shapeType == std::string("sphere"))
	{
		mShapeType = SPHERE;
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		if (not radius.empty())
		{
			mDim1 = (float) atof(radius.c_str());
			if (mDim1 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == std::string("box"))
	{
		mShapeType = BOX;
		std::string half_x = mTmpl->parameter(std::string("shape_half_x"));
		std::string half_y = mTmpl->parameter(std::string("shape_half_y"));
		std::string half_z = mTmpl->parameter(std::string("shape_half_z"));
		if ((not half_x.empty()) and (not half_y.empty())
				and (not half_z.empty()))
		{
			mDim1 = (float) atof(half_x.c_str());
			mDim2 = (float) atof(half_y.c_str());
			mDim3 = (float) atof(half_z.c_str());
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
			mShapeType = CYLINDER;
		}
		else if (shapeType == std::string("capsule"))
		{
			mShapeType = CAPSULE;
		}
		else
		{
			mShapeType = CONE;
		}
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		std::string height = mTmpl->parameter(std::string("shape_height"));
		std::string upAxis = mTmpl->parameter(std::string("shape_up"));
		if ((not radius.empty()) and (not height.empty()))
		{
			mDim1 = (float) atof(radius.c_str());
			mDim2 = (float) atof(height.c_str());
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
		mShapeType = SPHERE;
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
		uint32_t mask = (uint32_t) atoi(collideMask.c_str());
		mCollideMask.set_word(mask);
#ifdef DEBUG
		mCollideMask.write(std::cout, 0);
#endif
	}
	//set control parameters
	mLinearSpeed = (float) atof(
			mTmpl->parameter(std::string("linear_speed")).c_str());
	mAngularSpeed = (float) atof(
			mTmpl->parameter(std::string("angular_speed")).c_str());
	mFallSpeed = (float) atof(
			mTmpl->parameter(std::string("fall_speed")).c_str());
	mGravity = (float) atof(mTmpl->parameter(std::string("gravity")).c_str());
	mJumpSpeed = (float) atof(
			mTmpl->parameter(std::string("jump_speed")).c_str());
	mMaxSlope = (float) atof(
			mTmpl->parameter(std::string("max_slope")).c_str());
	mMaxJumpHeight = (float) atof(
			mTmpl->parameter(std::string("max_jump_height")).c_str());
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
	//setup event callbacks if any
	setupEvents();
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
	mCharacterController = new BulletCharacterControllerNode(
			createShape(mShapeType), mStepHeight,
			std::string(mComponentId).c_str());
	//set the control parameters
	setControlParameters();

	//attach it to Bullet World
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			DCAST(TypedObject, mCharacterController));

	//create a node path for the character controller
	mNodePath = NodePath(mCharacterController);
	//set collide mask
	mNodePath.set_collide_mask(mCollideMask);

	//reparent the object node path as a child of the character controller's one
	NodePath ownerNodePath = mOwnerObject->getNodePath();
	ownerNodePath.reparent_to(mNodePath);

	//set the object node path as this character controller's one
	mOwnerObject->setNodePath(mNodePath);
	//correct (or possibly reset to zero) pos and hpr of the object node path
	ownerNodePath.set_pos_hpr(mModelDeltaCenter, LVecBase3::zero());

	//Add to the physics manager update
	//first check if game physics manager exists
	if (GamePhysicsManager::GetSingletonPtr())
	{
		GamePhysicsManager::GetSingletonPtr()->addToPhysicsUpdate(this);
	}
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

void CharacterController::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
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

void CharacterController::enableStrafeRight(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
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

void CharacterController::enableRollRight(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mRollRightKey)
	{
		mRollRight = enable;
	}
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

float CharacterController::getLinearSpeed()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mLinearSpeed;
}

void CharacterController::setLinearSpeed(float speed)
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
		speed.set_y(-mLinearSpeed);
	}
	if (mBackward)
	{
		speed.set_y(mLinearSpeed);
	}
	if (mStrafeLeft)
	{
		speed.set_x(-mLinearSpeed);
	}
	if (mStrafeRight)
	{
		speed.set_x(mLinearSpeed);
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
	mCharacterController->set_linear_movement(speed, true);
	mCharacterController->set_angular_movement(omega);
	if (mJump)
	{
		if (mCharacterController->is_on_ground())
		{
			mCharacterController->do_jump();
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

SMARTPTR(BulletShape)CharacterController::createShape(ShapeType shapeType)
{
	//check if it should use shape of another (already) created object
	ObjectId useShapeOfId = ObjectId(mTmpl->parameter(std::string("use_shape_of")));
	if (not useShapeOfId.empty())
	{
		SMARTPTR(Object)createdObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				useShapeOfId);
		if (createdObject != NULL)
		{
			//object already exists
			SMARTPTR(CharacterController)characterController = DCAST(CharacterController, createdObject->
					getComponent(ComponentFamilyType("Physics")));
			if (characterController != NULL)
			{
				//physics component is a character controller:
				//return a reference to its shape
				return characterController->mCharacterController->get_shape();
			}
		}
	}
	//get the bounding dimensions of object node path, that
	//should represents a model
	getBoundingDimensions(mOwnerObject->getNodePath());
	// create the actual shape
	SMARTPTR(BulletShape) collisionShape = NULL;
	LVecBase3 localScale;
	switch (mShapeType)
	{
		case SPHERE:
		if (mAutomaticShaping)
		{
			//modify radius
			mDim1 = mModelRadius;
		}
		collisionShape = new BulletSphereShape(mDim1);
		break;
		case BOX:
		if (mAutomaticShaping)
		{
			//modify half dimensions
			mDim1 = mModelDims.get_x() / 2.0;
			mDim2 = mModelDims.get_y() / 2.0;
			mDim3 = mModelDims.get_z() / 2.0;
		}
		collisionShape = new BulletBoxShape(LVector3(mDim1, mDim2, mDim3));
		break;
		case CYLINDER:
		if (mAutomaticShaping)
		{
			//modify radius and height
			if (mUpAxis == X_up)
			{
				mDim1 = max(mModelDims.get_y(), mModelDims.get_z()) / 2.0;
				mDim2 = mModelDims.get_x();
			}
			else if (mUpAxis == Y_up)
			{
				mDim1 = max(mModelDims.get_x(), mModelDims.get_z()) / 2.0;
				mDim2 = mModelDims.get_y();
			}
			else
			{
				mDim1 = max(mModelDims.get_x(), mModelDims.get_y()) / 2.0;
				mDim2 = mModelDims.get_z();
			}
		}
		collisionShape = new BulletCylinderShape(mDim1, mDim2, mUpAxis);
		break;
		case CAPSULE:
		if (mAutomaticShaping)
		{
			//modify radius and height
			if (mUpAxis == X_up)
			{
				mDim1 = max(mModelDims.get_y(), mModelDims.get_z()) / 2.0;
				mDim2 = mModelDims.get_x();
			}
			else if (mUpAxis == Y_up)
			{
				mDim1 = max(mModelDims.get_x(), mModelDims.get_z()) / 2.0;
				mDim2 = mModelDims.get_y();
			}
			else
			{
				mDim1 = max(mModelDims.get_x(), mModelDims.get_y()) / 2.0;
				mDim2 = mModelDims.get_z() - 2*mDim1;
				if (mDim2 <= 0.0)
				{
					mDim2 = 0.0;
				}
			}
		}
		collisionShape = new BulletCapsuleShape(mDim1, mDim2, mUpAxis);
		break;
		case CONE:
		if (mAutomaticShaping)
		{
			//modify radius and height
			if (mUpAxis == X_up)
			{
				mDim1 = max(mModelDims.get_y(), mModelDims.get_z()) / 2.0;
				mDim2 = mModelDims.get_x();
			}
			else if (mUpAxis == Y_up)
			{
				mDim1 = max(mModelDims.get_x(), mModelDims.get_z()) / 2.0;
				mDim2 = mModelDims.get_y();
			}
			else
			{
				mDim1 = max(mModelDims.get_x(), mModelDims.get_y()) / 2.0;
				mDim2 = mModelDims.get_z();
			}
		}
		collisionShape = new BulletConeShape(mDim1, mDim2, mUpAxis);
		break;
		default:
		break;
	}
	//
	return collisionShape;
}

void CharacterController::getBoundingDimensions(NodePath modelNP)
{
	//get "tight" dimensions of panda
	LPoint3 minP, maxP;
	modelNP.calc_tight_bounds(minP, maxP);
	//
	LVecBase3 delta = maxP - minP;
	//
	mModelDims = LVector3(abs(delta.get_x()), abs(delta.get_y()),
			abs(delta.get_z()));
	mModelDeltaCenter = -(minP + delta / 2.0);
	mModelRadius = max(max(mModelDims.get_x(), mModelDims.get_y()),
			mModelDims.get_z()) / 2.0;
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

