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
	//body type
	std::string bodyType = mTmpl->parameter(std::string("body_type"));
	if (bodyType == std::string("patch"))
	{
		mBodyType = PATCH;
	}
	else if (bodyType == std::string("ellipsoid"))
	{
		mBodyType = ELLIPSOID;
	}
	else if (bodyType == std::string("tri_mesh"))
	{
		mBodyType = TRIMESH;
	}
	else if (bodyType == std::string("tet_mesh"))
	{
		mBodyType = TETMESH;
	}
	else
	{
		mBodyType = ROPE;
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
	//
	std::string param;
	unsigned int idx, valueNum;
	std::vector<std::string> paramValuesStr;
	float value;
	int valueInt;
	//body total mass
	value = strtof(
			mTmpl->parameter(std::string("body_total_mass")).c_str(),
			NULL);
	mBodyTotalMass = (value >= 0.0 ? value : -value);
	//air density
	value = strtof(mTmpl->parameter(std::string("air_density")).c_str(),
			NULL);
	mAirDensity = (value >= 0.0 ? value : -value);
	//water density
	value = strtof(
			mTmpl->parameter(std::string("water_density")).c_str(),
			NULL);
	mWaterDensity = (value >= 0.0 ? value : -value);
	//water offset
	value = strtof(mTmpl->parameter(std::string("water_offset")).c_str(),
			NULL);
	mWaterOffset = (value >= 0.0 ? value : -value);
	//water normal
	param = mTmpl->parameter(std::string("water_normal"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 3)
	{
		paramValuesStr.resize(3, "0.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mWaterNormal[idx] = strtof(paramValuesStr[idx].c_str(), NULL);
	}
	//anchor objects
	param = mTmpl->parameter(std::string("anchor_objects"));
	paramValuesStr = parseCompoundString(param, ':');
	valueNum = paramValuesStr.size();
	for (idx = 0; idx < valueNum; ++idx)
	{
		mAnchorObjects.push_back(ObjectId(paramValuesStr[idx]));
	}
	//points
	param = mTmpl->parameter(std::string("points"));
	paramValuesStr = parseCompoundString(param, ':');
	valueNum = paramValuesStr.size();
	for (idx = 0; (idx < valueNum) and (idx < 4); ++idx)
	{
		std::vector<std::string> paramValuesStrExt = parseCompoundString(
				paramValuesStr[idx], ',');
		if (paramValuesStrExt.size() < 3)
		{
			paramValuesStrExt.resize(3, "0.0");
		}
		LPoint3f point;
		for (unsigned int i = 0; i < 3; ++i)
		{
			point[i] = strtof(paramValuesStrExt[i].c_str(), NULL);
		}
		mPoints.push_back(point);
	}
	//res
	param = mTmpl->parameter(std::string("res"));
	paramValuesStr = parseCompoundString(param, ':');
	valueNum = paramValuesStr.size();
	for (idx = 0; (idx < valueNum) and (idx < 2); ++idx)
	{
		valueInt = strtol(paramValuesStr[idx].c_str(), NULL, 0);
		mRes.push_back(valueInt >= 0 ? valueInt : -valueInt);
	}
	//fixeds
	mFixeds = strtol(mTmpl->parameter(std::string("fixeds")).c_str(), NULL, 0);
	//
	return result;
}

void SoftBody::onAddToObjectSetup()
{
	///TODO

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
