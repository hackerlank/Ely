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
#include <bulletHelper.h>
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
	value = strtof(mTmpl->parameter(std::string("body_total_mass")).c_str(),
	NULL);
	mBodyTotalMass = (value >= 0.0 ? value : -value);
	//body mass from faces
	mBodyMassFromFaces = (
			mTmpl->parameter(std::string("body_mass_from_faces"))
					== std::string("true") ? true : false);
	//air density
	value = strtof(mTmpl->parameter(std::string("air_density")).c_str(),
	NULL);
	mAirDensity = (value >= 0.0 ? value : -value);
	//water density
	value = strtof(mTmpl->parameter(std::string("water_density")).c_str(),
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
	//gendiags
	mGendiags = (
			mTmpl->parameter(std::string("gendiags")) == std::string("false") ?
					false : true);
	//radius
	param = mTmpl->parameter(std::string("radius"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 3)
	{
		paramValuesStr.resize(3, "0.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mRadius[idx] = strtof(paramValuesStr[idx].c_str(), NULL);
	}
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
		//get points
		if (mPoints.size() < 4)
		{
			mPoints.resize(4, LPoint3f::zero());
		}
		//get res
		if (mRes.size() < 2)
		{
			mRes.resize(2, 0);
		}
		//create rope
		mSoftBodyNode = BulletSoftBodyNode::make_patch(info, mPoints[0],
				mPoints[1], mPoints[2], mPoints[3], mRes[0], mRes[1], mFixeds,
				mGendiags);
		//link with Geom
		CSMARTPTR(GeomVertexFormat) format = GeomVertexFormat::get_v3n3t2();
		SMARTPTR(Geom) geom =
				BulletHelper::make_geom_from_faces(mSoftBodyNode, format, true).p();
		mSoftBodyNode->link_geom(geom);
		//visualize with GeomNode (if any)
		SMARTPTR(GeomNode)geomNode = DCAST(GeomNode,
				mOwnerObject->getNodePath().node());
		if (geomNode)
		{
			geomNode->add_geom(geom);
		}
		//Now we want to have a texture and texture coordinates.
		//The geom's format has already a column for texcoords, so we just need
		//to write texcoords using a GeomVertexRewriter.
		BulletHelper::make_texcoords_for_patch(geom, mRes[0], mRes[1]);
	}
	else if (mBodyType == ELLIPSOID)
	{
		//get points
		if (mPoints.size() < 1)
		{
			mPoints.resize(1, LPoint3f::zero());
		}
		//get res
		if (mRes.size() < 1)
		{
			mRes.resize(1, 0);
		}
		//create ellipsoid
		mSoftBodyNode = BulletSoftBodyNode::make_ellipsoid(info, mPoints[0],
				mRadius, mRes[0]);
		//link with Geom
		CSMARTPTR(GeomVertexFormat) format = GeomVertexFormat::get_v3n3t2();
		SMARTPTR(Geom) geom =
				BulletHelper::make_geom_from_faces(mSoftBodyNode, format, true).p();
		mSoftBodyNode->link_geom(geom);
		//visualize with GeomNode (if any)
		SMARTPTR(GeomNode)geomNode = DCAST(GeomNode,
				mOwnerObject->getNodePath().node());
		if (geomNode)
		{
			geomNode->add_geom(geom);
		}
		//Now we want to have a texture and texture coordinates.
		//The geom's format has already a column for texcoords, so we just need
		//to write texcoords using a GeomVertexRewriter.
		//Note: the following static function is to be written!!!
//		BulletHelper::make_texcoords_for_ellipsoid(geom, radius, mRes[0]);
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
		//get res
		if (mRes.size() < 1)
		{
			mRes.resize(1, 0);
		}
		//create rope
		mSoftBodyNode = BulletSoftBodyNode::make_rope(info, mPoints[0],
				mPoints[1], mRes[0], mFixeds);
		//link with NURBS curve
		SMARTPTR(NurbsCurveEvaluator)curve = new NurbsCurveEvaluator();
		curve->reset(mRes[0] + 2);
		mSoftBodyNode->link_curve(curve);
		//visualize with RopeNode (if any)
		SMARTPTR(RopeNode)ropeNode = DCAST(RopeNode,
				mOwnerObject->getNodePath().node());
		if (ropeNode)
		{
			ropeNode->set_curve(curve);
		}
	}
	//set total mass
	mSoftBodyNode->set_total_mass(mBodyTotalMass, mBodyMassFromFaces);

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
	///HACK: rope node's parent node path correction (see bullet samples)
	if (mBodyType == ROPE)
	{
		if (DCAST(RopeNode,	mNodePath.get_child(0).node()))
		{
			//the child is the rope node: reparent to the owner object node path
			mRopeNodePath = mNodePath.get_child(0);
			mRopeNodePath.reparent_to(mOwnerObject->getNodePath().get_parent());
		}
	}
}

void SoftBody::onRemoveFromSceneCleanup()
{
	///HACK: rope node's parent node path correction (see bullet samples)
	if (mBodyType == ROPE)
	{
		if (not mRopeNodePath.is_empty())
		{
			//the child is the rope node reparent to this node path
			mRopeNodePath.reparent_to(mNodePath);
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle SoftBody::_type_handle;

} /* namespace ely */
