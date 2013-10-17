/*
 * SoftBody.cpp
 *
 *  Created on: 08/ott/2013
 *      Author: marco
 */

#include "PhysicsComponents/SoftBody.h"
#include "PhysicsComponents/SoftBodyTemplate.h"
#include "Game/GamePhysicsManager.h"
#include "SceneComponents/Model.h"
#include <bulletSoftBodyWorldInfo.h>
#include <nurbsCurveEvaluator.h>

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
	std::list<std::string> paramList = mTmpl->parameterList(std::string("anchor_objects"));
	std::list<std::string>::iterator iter;
	for (iter = paramList.begin(); iter != paramList.end(); ++iter)
	{
		param = *iter;
		paramValuesStr = parseCompoundString(param, ':');
		valueNum = paramValuesStr.size();
		for (idx = 0; idx < valueNum; ++idx)
		{
			mAnchorObjects.push_back(ObjectId(paramValuesStr[idx]));
		}
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
	//At this point a Scene component (Model) should have been already
	//created and added to the object, so its node path should
	//be the same as the object's one.

	//Soft body world information
	BulletSoftBodyWorldInfo info =
			GamePhysicsManager::GetSingletonPtr()->bulletWorld()->get_world_info();
	info.set_air_density(mAirDensity);
	info.set_water_density(mWaterDensity);
	info.set_water_offset(mWaterOffset);
	info.set_water_normal(mWaterNormal);

	//create a Soft Body Node
	if (mBodyType == PATCH)
	{

	}
	else if (mBodyType == ELLIPSOID)
	{

	}
	else if (mBodyType == TRIMESH)
	{

	}
	else if (mBodyType == TETMESH)
	{

	}
	else
	{
		//ROPE
		//get points
		if (mPoints.size() < 2)
		{
			mPoints.resize(2, LPoint3f::zero());
		}
		//create rope
		mSoftBodyNode = BulletSoftBodyNode::make_rope(info, mPoints[0],
				mPoints[1], mRes[0], mFixeds);
		//link with NURBS curve
		SMARTPTR(NurbsCurveEvaluator)curve = new NurbsCurveEvaluator();
		curve->reset(mRes[0] + 2);
		mSoftBodyNode->link_curve(curve);
		//visualize with RopeNode
		SMARTPTR(Model)model = DCAST(Model,
				mOwnerObject->getComponent(ComponentFamilyType("Scene")));
		if (model)
		{
			SMARTPTR(RopeNode) ropeNode = DCAST(RopeNode,
					model->getNodePath().node());
			if (ropeNode)
			{
				ropeNode->set_curve(curve);
			}
		}
	}
	//set total mass
	mSoftBodyNode->set_total_mass(mBodyTotalMass);

	//create a node path for the soft body
	mNodePath = NodePath(mSoftBodyNode);

	//attach to Bullet World
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(mSoftBodyNode);

	//set collide mask
	mNodePath.set_collide_mask(mCollideMask);

	NodePath ownerNodePath = mOwnerObject->getNodePath();
	if (not ownerNodePath.is_empty())
	{
		//reparent the object node path as a child of the soft body's one
		ownerNodePath.reparent_to(mNodePath);
		//optimize
		mNodePath.flatten_strong();
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

//TypedObject semantics: hardcoded
TypeHandle SoftBody::_type_handle;

} /* namespace ely */
