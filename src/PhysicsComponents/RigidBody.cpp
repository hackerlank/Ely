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
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove_rigid_body(
			mRigidBodyNode);
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
	bool result = true;
	//get body type
	std::string bodyType = mTmpl->parameter(std::string("body_type"));
	if (bodyType == std::string("dynamic"))
	{
		mBodyType = DYNAMIC;
	}
	if (bodyType == std::string("static"))
	{
		mBodyType = STATIC;
	}
	if (bodyType == std::string("kinematic"))
	{
		mBodyType = KINEMATIC;
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
	mAutomaticShaping = true;
	if (shapeType == std::string("sphere"))
	{
		mShapeType = SPHERE;
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		if (radius != mTmpl->UNKNOWN)
		{
			mDim1 = (float) atof(radius.c_str());
			mDim1 > 0.0 ? mAutomaticShaping = false : mAutomaticShaping = true;
		}
	}
	if (shapeType == std::string("box"))
	{
		mShapeType = BOX;
		std::string half_x = mTmpl->parameter(std::string("shape_half_x"));
		std::string half_y = mTmpl->parameter(std::string("shape_half_y"));
		std::string half_z = mTmpl->parameter(std::string("shape_half_z"));
		if ((half_x != mTmpl->UNKNOWN) and (half_y != mTmpl->UNKNOWN)
				and (half_z != mTmpl->UNKNOWN))
		{
			mDim1 = (float) atof(half_x.c_str());
			mDim2 = (float) atof(half_y.c_str());
			mDim3 = (float) atof(half_z.c_str());
			mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0 ? mAutomaticShaping =
					false : mAutomaticShaping = true;
		}
	}
	if (shapeType == std::string("cylinder"))
	{
		mShapeType = CYLINDER;
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		std::string height = mTmpl->parameter(std::string("shape_height"));
		mUpAxis = mTmpl->parameter(std::string("shape_up"));
		if ((radius != mTmpl->UNKNOWN) and (height != mTmpl->UNKNOWN))
		{
			mDim1 = (float) atof(radius.c_str());
			mDim2 = (float) atof(height.c_str());
			mDim1 > 0.0 and mDim2 > 0.0 ? mAutomaticShaping = false :
					mAutomaticShaping = true;
		}
	}
	if (shapeType == std::string("capsule"))
	{
		mShapeType = CAPSULE;
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		std::string height = mTmpl->parameter(std::string("shape_height"));
		mUpAxis = mTmpl->parameter(std::string("shape_up"));
		if ((radius != mTmpl->UNKNOWN) and (height != mTmpl->UNKNOWN))
		{
			mDim1 = (float) atof(radius.c_str());
			mDim2 = (float) atof(height.c_str());
			mDim1 > 0.0 and mDim2 > 0.0 ? mAutomaticShaping = false :
					mAutomaticShaping = true;
		}
	}
	if (shapeType == std::string("cone"))
	{
		mShapeType = CONE;
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		std::string height = mTmpl->parameter(std::string("shape_height"));
		mUpAxis = mTmpl->parameter(std::string("shape_up"));
		if ((radius != mTmpl->UNKNOWN) and (height != mTmpl->UNKNOWN))
		{
			mDim1 = (float) atof(radius.c_str());
			mDim2 = (float) atof(height.c_str());
			mDim1 > 0.0 and mDim2 > 0.0 ? mAutomaticShaping = false :
					mAutomaticShaping = true;
		}
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
		mCollideMask.write(std::cout, 2);
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

void RigidBody::setPhysicalParameters()
{
	mRigidBodyNode->set_mass(mBodyMass);
	mRigidBodyNode->set_friction(mBodyFriction);
	mRigidBodyNode->set_restitution(mBodyRestitution);
}

LVector3 RigidBody::correctedPosition()
{

}

void RigidBody::onAddToSceneSetup()
{
	//create a Rigid Body Node
	mRigidBodyNode = new BulletRigidBodyNode(std::string(mComponentId).c_str());
	//set the physics parameters
	setPhysicalParameters();

	//create and add a Collision Shape
	mRigidBodyNode->add_shape(createShape(mShapeType));
	//attach to Bullet World
	//<BUG: you must first insert a dynamic body for switching to work
	mRigidBodyNode->set_mass(1.0);
	//BUG>
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach_rigid_body(
			mRigidBodyNode);
	//switch the body type (take precedence over mass)
	switchType(mBodyType);

	//create a node path for the rigid body
	NodePath rigidBodyNP = NodePath(mRigidBodyNode);
	//set collide mask
	rigidBodyNP.set_collide_mask(mCollideMask);
	//reparent the object node path as a child of the rigid body one
	mOwnerObject->nodePath().set_pos(correctedPosition());
	mOwnerObject->nodePath().flatten_light();
	mOwnerObject->nodePath().reparent_to(rigidBodyNP);
	//set the object node path as that of the rigid body
	mOwnerObject->nodePath() = rigidBodyNP;
}

BulletShape* RigidBody::createShape(ShapeType shapeType)
{
	//get the dimensions of object node path, that should represents
	//a model (if not all dimensions are set to 1.0)
	getDimensions(mOwnerObject->nodePath());

}

//def getDimensions(model, scale=None):
//    # get dimensions of panda
//    modelScale = scale
//    minP = LPoint3f()
//    maxP = LPoint3f()
//    model.calcTightBounds(minP, maxP)
//    if modelScale != None:
//        modelDims = LVector3f(abs(maxP.getX() - minP.getX()),
//                          abs(maxP.getY() - minP.getY()),
//                          abs(maxP.getZ() - minP.getZ())) * modelScale
//    else:
//        modelDims = LVector3f(abs(maxP.getX() - minP.getX()),
//                          abs(maxP.getY() - minP.getY()),
//                          abs(maxP.getZ() - minP.getZ()))
//    #
//    modelCenter = LPoint3f(maxP.getX() - minP.getX(),
//                       maxP.getY() - minP.getY(),
//                       maxP.getZ() - minP.getZ()) / 2.0
//    modelRadius = max(modelDims.getX(),
//                      modelDims.getY(),
//                      modelDims.getZ()) / 2.0
//    if modelScale != None:
//        model.setScale(modelScale)
//    return (modelDims, modelCenter, modelRadius)
void RigidBody::getDimensions(NodePath modelNP, LVecBase3 scale)
{
}

BulletRigidBodyNode* RigidBody::rigidBodyNode()
{
	return mRigidBodyNode.p();
}

void RigidBody::switchType(BodyType bodyType)
{
}

//TypedObject semantics: hardcoded
TypeHandle RigidBody::_type_handle;

