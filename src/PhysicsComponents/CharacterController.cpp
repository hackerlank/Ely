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
}

CharacterController::~CharacterController()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (GamePhysicsManager::GetSingletonPtr())
	{
		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
				DCAST(TypedObject, mCharacterNode));
	}
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
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void CharacterController::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

}

void CharacterController::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
				return characterController->mCharacterNode->get_shape();
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
				mDim2 = mModelDims.get_z();
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

//TypedObject semantics: hardcoded
TypeHandle CharacterController::_type_handle;

