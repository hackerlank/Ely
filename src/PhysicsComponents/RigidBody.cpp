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
 * \file /Ely/src/PhysicsComponents/RigidBody.cpp
 *
 * \date 07/lug/2012 (15:58:35)
 * \author marco
 */

#include "PhysicsComponents/RigidBody.h"
#include "PhysicsComponents/RigidBodyTemplate.h"

RigidBody::RigidBody()
{
	// TODO Auto-generated constructor stub

}

RigidBody::RigidBody(RigidBodyTemplate* tmpl) :
		mTmpl(tmpl)
{
}

RigidBody::~RigidBody()
{
	if (GamePhysicsManager::GetSingletonPtr())
	{
		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove_rigid_body(
				mRigidBodyNode);
	}
}

const ComponentFamilyType RigidBody::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType RigidBody::componentType() const
{
	return mTmpl->componentType();
}

bool RigidBody::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//get body type
	std::string bodyType = mTmpl->parameter(std::string("body_type"));
	if (bodyType == std::string("static"))
	{
		mBodyType = STATIC;
	}
	else if (bodyType == std::string("kinematic"))
	{
		mBodyType = KINEMATIC;
	}
	else
	{
		mBodyType = DYNAMIC;
	}
	//get physical parameters
	mBodyMass = (float) atof(
			mTmpl->parameter(std::string("body_mass")).c_str());
	mBodyFriction = (float) atof(
			mTmpl->parameter(std::string("body_friction")).c_str());
	mBodyRestitution = (float) atof(
			mTmpl->parameter(std::string("body_restitution")).c_str());
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
	else if (shapeType == std::string("plane"))
	{
		mShapeType = PLANE;
		std::string norm_x = mTmpl->parameter(std::string("shape_norm_x"));
		std::string norm_y = mTmpl->parameter(std::string("shape_norm_y"));
		std::string norm_z = mTmpl->parameter(std::string("shape_norm_z"));
		std::string d = mTmpl->parameter(std::string("shape_d"));
		if ((not norm_x.empty()) and (not norm_y.empty())
				and (not norm_z.empty()))
		{
			LVector3 normal((float) atof(norm_x.c_str()),
					(float) atof(norm_y.c_str()), (float) atof(norm_z.c_str()));
			normal.normalize();
			mDim1 = normal.get_x();
			mDim2 = normal.get_y();
			mDim3 = normal.get_z();
			mDim4 = (float) atof(d.c_str());
			if (normal.length() > 0.0)
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
		if ((not radius.empty()) and (not height.empty()))
		{
			mDim1 = (float) atof(radius.c_str());
			mDim2 = (float) atof(height.c_str());
			if (mDim1 > 0.0 and mDim2 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
		std::string upAxis = mTmpl->parameter(std::string("shape_up"));
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
	//get ccd settings: enabled if both are greater than zero (> 0.0)
	mCcdMotionThreshold = (float) atof(
			mTmpl->parameter(std::string("ccd_motion_threshold")).c_str());
	mCcdSweptSphereRadius = (float) atof(
			mTmpl->parameter(std::string("ccd_swept_sphere_radius")).c_str());
	((mCcdMotionThreshold > 0.0) and (mCcdSweptSphereRadius > 0.0)) ? mCcdEnabled =
			true :
			mCcdEnabled = false;
	//
	return result;
}

void RigidBody::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//create a Rigid Body Node
	mRigidBodyNode = new BulletRigidBodyNode(std::string(mComponentId).c_str());
	//set the physics parameters
	setPhysicalParameters();

	//At this point a Scene component (Model, InstanceOf ...) should have
	//been already created and added to the object, so its node path should
	//be the same as the object's one.
	//Note: scaling is applied to a Scene component, so the object node path
	//has scaling already applied.

	//create and add a Collision Shape
	mRigidBodyNode->add_shape(createShape(mShapeType));
	//attach to Bullet World
	//<BUG: you must first insert a dynamic body for switching to work
	mRigidBodyNode->set_mass(1.0);
	//BUG>
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach_rigid_body(
			mRigidBodyNode);
	switchType(mBodyType);

	//create a node path for the rigid body
	mNodePath = NodePath(mRigidBodyNode);
	//set collide mask
	mNodePath.set_collide_mask(mCollideMask);

	//reparent the object node path as a child of the rigid body's one
	NodePath ownerNodePath = mOwnerObject->getNodePath();
	ownerNodePath.reparent_to(mNodePath);
	//correct (or possibly reset to zero) pos and hpr of the object node path
	ownerNodePath.set_pos_hpr(mModelDeltaCenter, LVecBase3::zero());
	if (mOwnerObject->isStatic())
	{
		ownerNodePath.flatten_light();
	}

	//set the object node path as this rigid body's one
	mOwnerObject->setNodePath(mNodePath);
}

void RigidBody::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//switch the body type (take precedence over mass)
	//force this component to static if owner object is static
	if (mOwnerObject->isStatic())
	{
		mBodyType = STATIC;
	}
	switchType(mBodyType);
}

void RigidBody::switchType(BodyType bodyType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	switch (bodyType)
	{
	case DYNAMIC:
		mRigidBodyNode->set_mass(mBodyMass);
		mRigidBodyNode->set_kinematic(false);
		mRigidBodyNode->set_static(false);
		mRigidBodyNode->set_deactivation_enabled(true);
		mRigidBodyNode->set_active(true);
		break;
	case STATIC:
		mRigidBodyNode->set_mass(0.0);
		mRigidBodyNode->set_kinematic(false);
		mRigidBodyNode->set_static(true);
		mRigidBodyNode->set_deactivation_enabled(true);
		mRigidBodyNode->set_active(false);
		break;
	case KINEMATIC:
		mRigidBodyNode->set_mass(0.0);
		mRigidBodyNode->set_kinematic(true);
		mRigidBodyNode->set_static(false);
		mRigidBodyNode->set_deactivation_enabled(false);
		mRigidBodyNode->set_active(false);
		break;
	default:
		break;
	}
}

NodePath RigidBody::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void RigidBody::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

BulletShape* RigidBody::createShape(ShapeType shapeType)
{
	//get the bounding dimensions of object node path, that
	//should represents a model
	getBoundingDimensions(mOwnerObject->getNodePath());
	// create the actual shape
	BulletShape* collisionShape = NULL;
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
	case PLANE:
		if (mAutomaticShaping)
		{
			//modify normal and d
			mDim1 = 0.0;
			mDim2 = 0.0;
			mDim3 = 1.0;
			mDim4 = 0.0;
		}
		collisionShape = new BulletPlaneShape(LVector3(mDim1, mDim2, mDim3),
				mDim4);
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

void RigidBody::getBoundingDimensions(NodePath modelNP)
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

void RigidBody::setPhysicalParameters()
{
	mRigidBodyNode->set_mass(mBodyMass);
	mRigidBodyNode->set_friction(mBodyFriction);
	mRigidBodyNode->set_restitution(mBodyRestitution);
	if (mCcdEnabled)
	{
		mRigidBodyNode->set_ccd_motion_threshold(mCcdMotionThreshold);
		mRigidBodyNode->set_ccd_swept_sphere_radius(mCcdSweptSphereRadius);
	}
}

//TypedObject semantics: hardcoded
TypeHandle RigidBody::_type_handle;

