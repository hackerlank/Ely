/*
 * SoftBody.cpp
 *
 *  Created on: 08/ott/2013
 *      Author: marco
 */

#include "PhysicsComponents/SoftBody.h"
#include "PhysicsComponents/SoftBodyTemplate.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

SoftBody::SoftBody()
{
	// TODO Auto-generated constructor stub
}

SoftBody::SoftBody(SMARTPTR(SoftBodyTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"SoftBody::SoftBody: invalid GamePhysicsManager")

	mTmpl = tmpl;
	reset();
}

SoftBody::~SoftBody()
{
}

ComponentFamilyType SoftBody::familyType() const
{
	return mTmpl->familyType();
}

ComponentType SoftBody::componentType() const
{
	return mTmpl->componentType();
}

bool SoftBody::initialize()
{
	bool result = true;
//	//get body type
//	std::string bodyType = mTmpl->parameter(std::string("body_type"));
//	if (bodyType == std::string("static"))
//	{
//		mBodyType = STATIC;
//	}
//	else if (bodyType == std::string("kinematic"))
//	{
//		mBodyType = KINEMATIC;
//	}
//	else
//	{
//		mBodyType = DYNAMIC;
//	}
//	//get physical parameters
//	mBodyTotalMass = strtof(
//			mTmpl->parameter(std::string("body_mass")).c_str(), NULL);
//	mBodyFriction = strtof(
//			mTmpl->parameter(std::string("body_friction")).c_str(), NULL);
//	mBodyRestitution = strtof(
//			mTmpl->parameter(std::string("body_restitution")).c_str(), NULL);
//	//get shape type
//	std::string shapeType = mTmpl->parameter(std::string("shape_type"));
//	//get shape size
//	std::string shapeSize = mTmpl->parameter(std::string("shape_size"));
//	if (shapeSize == std::string("minimum"))
//	{
//		mShapeSize = GamePhysicsManager::MINIMUN;
//	}
//	else if (shapeSize == std::string("maximum"))
//	{
//		mShapeSize = GamePhysicsManager::MAXIMUM;
//	}
//	else
//	{
//		mShapeSize = GamePhysicsManager::MEDIUM;
//	}
//	//default auto shaping
//	mAutomaticShaping = true;
//	if (shapeType == std::string("sphere"))
//	{
//		mShapeType = GamePhysicsManager::SPHERE;
//		std::string radius = mTmpl->parameter(std::string("shape_radius"));
//		if (not radius.empty())
//		{
//			mDim1 = strtof(radius.c_str(), NULL);
//			if (mDim1 > 0.0)
//			{
//				mAutomaticShaping = false;
//			}
//		}
//	}
//	else if (shapeType == std::string("plane"))
//	{
//		mShapeType = GamePhysicsManager::PLANE;
//		std::string norm_x = mTmpl->parameter(std::string("shape_norm_x"));
//		std::string norm_y = mTmpl->parameter(std::string("shape_norm_y"));
//		std::string norm_z = mTmpl->parameter(std::string("shape_norm_z"));
//		std::string d = mTmpl->parameter(std::string("shape_d"));
//		if ((not norm_x.empty()) and (not norm_y.empty())
//				and (not norm_z.empty()))
//		{
//			LVector3 normal(strtof(norm_x.c_str(), NULL),
//					strtof(norm_y.c_str(), NULL),
//					strtof(norm_z.c_str(), NULL));
//			normal.normalize();
//			mDim1 = normal.get_x();
//			mDim2 = normal.get_y();
//			mDim3 = normal.get_z();
//			mDim4 = strtof(d.c_str(), NULL);
//			if (normal.length() > 0.0)
//			{
//				mAutomaticShaping = false;
//			}
//		}
//	}
//	else if (shapeType == std::string("box"))
//	{
//		mShapeType = GamePhysicsManager::BOX;
//		std::string half_x = mTmpl->parameter(std::string("shape_half_x"));
//		std::string half_y = mTmpl->parameter(std::string("shape_half_y"));
//		std::string half_z = mTmpl->parameter(std::string("shape_half_z"));
//		if ((not half_x.empty()) and (not half_y.empty())
//				and (not half_z.empty()))
//		{
//			mDim1 = strtof(half_x.c_str(), NULL);
//			mDim2 = strtof(half_y.c_str(), NULL);
//			mDim3 = strtof(half_z.c_str(), NULL);
//			if (mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0)
//			{
//				mAutomaticShaping = false;
//			}
//		}
//	}
//	else if (shapeType == std::string("cylinder")
//			or shapeType == std::string("capsule")
//			or shapeType == std::string("cone"))
//	{
//		if (shapeType == std::string("cylinder"))
//		{
//			mShapeType = GamePhysicsManager::CYLINDER;
//		}
//		else if (shapeType == std::string("capsule"))
//		{
//			mShapeType = GamePhysicsManager::CAPSULE;
//		}
//		else
//		{
//			mShapeType = GamePhysicsManager::CONE;
//		}
//		std::string radius = mTmpl->parameter(std::string("shape_radius"));
//		std::string height = mTmpl->parameter(std::string("shape_height"));
//		std::string upAxis = mTmpl->parameter(std::string("shape_up"));
//		if ((not radius.empty()) and (not height.empty()))
//		{
//			mDim1 = strtof(radius.c_str(), NULL);
//			mDim2 = strtof(height.c_str(), NULL);
//			if (mDim1 > 0.0 and mDim2 > 0.0)
//			{
//				mAutomaticShaping = false;
//			}
//		}
//		if (upAxis == std::string("x"))
//		{
//			mUpAxis = X_up;
//		}
//		else if (upAxis == std::string("y"))
//		{
//			mUpAxis = Y_up;
//		}
//		else
//		{
//			mUpAxis = Z_up;
//		}
//	}
//	else if (shapeType == std::string("heightfield"))
//	{
//		mShapeType = GamePhysicsManager::HEIGHTFIELD;
//		std::string heightfield_file = mTmpl->parameter(
//				std::string("shape_heightfield_file"));
//		mHeightfieldFile = Filename(heightfield_file);
//		std::string height = mTmpl->parameter(std::string("shape_height"));
//		std::string scale_w = mTmpl->parameter(std::string("shape_scale_w"));
//		std::string scale_d = mTmpl->parameter(std::string("shape_scale_d"));
//		std::string upAxis = mTmpl->parameter(std::string("shape_up"));
//		if ((not height.empty()) and (not scale_w.empty())
//				and (not scale_d.empty()))
//		{
//			mDim1 = strtof(height.c_str(), NULL);
//			mDim2 = strtof(scale_w.c_str(), NULL);
//			mDim3 = strtof(scale_d.c_str(), NULL);
//			if (mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0)
//			{
//				mAutomaticShaping = false;
//			}
//		}
//		if (upAxis == std::string("x"))
//		{
//			mUpAxis = X_up;
//		}
//		else if (upAxis == std::string("y"))
//		{
//			mUpAxis = Y_up;
//		}
//		else
//		{
//			mUpAxis = Z_up;
//		}
//	}
//	else if (shapeType == std::string("triangle_mesh"))
//	{
//		mShapeType = GamePhysicsManager::TRIANGLEMESH;
//	}
//	else
//	{
//		//default a sphere (with auto shaping)
//		mShapeType = GamePhysicsManager::SPHERE;
//	}
//	//get collide mask
//	std::string collideMask = mTmpl->parameter(std::string("collide_mask"));
//	if (collideMask == std::string("all_on"))
//	{
//		mCollideMask = BitMask32::all_on();
//	}
//	else if (collideMask == std::string("all_off"))
//	{
//		mCollideMask = BitMask32::all_off();
//	}
//	else
//	{
//		uint32_t mask = (uint32_t) strtol(collideMask.c_str(), NULL, 0);
//		mCollideMask.set_word(mask);
//#ifdef ELY_DEBUG
//		mCollideMask.write(std::cout, 0);
//#endif
//	}
//	//get ccd settings: enabled if both are greater than zero (> 0.0)
//	mCcdMotionThreshold = strtof(
//			mTmpl->parameter(std::string("ccd_motion_threshold")).c_str(),
//			NULL);
//	mCcdSweptSphereRadius = strtof(
//			mTmpl->parameter(std::string("ccd_swept_sphere_radius")).c_str(),
//			NULL);
//	((mCcdMotionThreshold > 0.0) and (mCcdSweptSphereRadius > 0.0)) ?
//			mCcdEnabled = true : mCcdEnabled = false;
//	//use shape of (another object)
//	mUseShapeOfId = ObjectId(mTmpl->parameter(std::string("use_shape_of")));
//	//
	return result;
}

void SoftBody::onAddToObjectSetup()
{
	//create a Soft Body Node
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
//	mSoftBodyNode = new BulletSoftBodyNode(name.c_str());

	//At this point a Scene component (Model, InstanceOf ...) should have
	//been already created and added to the object, so its node path should
	//be the same as the object's one.
	//Note: scaling is applied to a Scene component, so the object node path
	//has scaling already applied.

	//create a node path for the rigid body
	mNodePath = NodePath(mSoftBodyNode);

	//attach to Bullet World
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			mSoftBodyNode);
	///<BUG: if you want to switch the body type (e.g. dynamic to static, static to
	///dynamic, etc...) after it has been attached to the world, you must first
	///attach it as a dynamic body and then switch its type:
	///		mSoftBodyNode->set_mass(1.0);
	///		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(mSoftBodyNode);
	///		switchType(mBodyType);
	////BUG>

	//set collide mask
	mNodePath.set_collide_mask(mCollideMask);

	NodePath ownerNodePath = mOwnerObject->getNodePath();
	if (not ownerNodePath.is_empty())
	{
		//reparent the object node path as a child of the rigid body's one
		ownerNodePath.reparent_to(mNodePath);
		//optimize
		mNodePath.flatten_strong();
	}
	else
	{
	}

	//set this rigid body node path as the object's one
	mOwnerObject->setNodePath(mNodePath);
}

void SoftBody::onRemoveFromObjectCleanup()
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

	//remove rigid body from the physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
			mSoftBodyNode);

	//Remove node path
	mNodePath.remove_node();
	//
	reset();
}

void SoftBody::onAddToSceneSetup()
{
	//switch the body type (take precedence over mass)
	//force this component to static if owner object is static
	if (mOwnerObject->isSteady())
	{
		mBodyType = ROPE;
	}
	//
}

//TypedObject semantics: hardcoded
TypeHandle SoftBody::_type_handle;

} /* namespace ely */
