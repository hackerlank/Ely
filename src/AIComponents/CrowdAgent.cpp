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
 * \file /Ely/src/AIComponents/CrowdAgent.cpp
 *
 * \date 06/giu/2013 (19:27:21)
 * \author consultit
 */

#include "AIComponents/CrowdAgent.h"
#include "AIComponents/CrowdAgentTemplate.h"
#include "AIComponents/NavMesh.h"
#include "AIComponents/RecastNavigation/CrowdTool.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"

namespace ely
{

CrowdAgent::CrowdAgent()
{
	// TODO Auto-generated constructor stub
}

CrowdAgent::CrowdAgent(SMARTPTR(CrowdAgentTemplate)tmpl)
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"CrowdAgent::CrowdAgent: invalid GameAIManager")
	mTmpl = tmpl;
	//
	mAgent = NULL;
	mAgentIdx = -1;
	mNavMeshObject = NULL;
	mAddedToHandling = false;
	mMovType = RECAST;
}

CrowdAgent::~CrowdAgent()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	removeFromNavMesh();
}

const ComponentFamilyType CrowdAgent::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType CrowdAgent::componentType() const
{
	return mTmpl->componentType();
}

bool CrowdAgent::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//throw events setting
	mThrowEvents = (
			mTmpl->parameter(std::string("throw_events"))
					== std::string("true") ? true : false);
	//set CrowdAgent parameters
	//register to navmesh objectId
	mNavMeshObjectId = mTmpl->parameter(std::string("register_to_navmesh"));
	//agent params
	mAgentParams.maxAcceleration = (float) strtof(
			mTmpl->parameter(std::string("max_acceleration")).c_str(), NULL);
	mAgentParams.maxSpeed = (float) strtof(
			mTmpl->parameter(std::string("max_speed")).c_str(), NULL);
	mAgentParams.collisionQueryRange = (float) strtof(
			mTmpl->parameter(std::string("collision_query_range")).c_str(), NULL);
	mAgentParams.pathOptimizationRange = (float) strtof(
			mTmpl->parameter(std::string("path_optimization_range")).c_str(), NULL);
	mAgentParams.separationWeight = (float) strtof(
			mTmpl->parameter(std::string("separation_weight")).c_str(), NULL);
	mAgentParams.updateFlags = strtol(
			mTmpl->parameter(std::string("update_flags")).c_str(), NULL, 0);
	if (mAgentParams.updateFlags <= 0)
	{
		mAgentParams.updateFlags = 0x1b;
	}
	mAgentParams.obstacleAvoidanceType = strtol(
			mTmpl->parameter(std::string("obstacle_avoidance_type")).c_str(), NULL, 0);
	if (mAgentParams.obstacleAvoidanceType < 0)
	{
		mAgentParams.obstacleAvoidanceType = 3;
	}
	//
	return result;
}

void CrowdAgent::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//set referenceNP to the owner object parent
	mReferenceNP = mOwnerObject->getNodePath().get_parent();

	//get NavMesh owner object if register_to_navmesh == existent navmesh
	mNavMeshObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			mNavMeshObjectId);
	//set Agent radius/height of NavMesh settings.
	if(mNavMeshObject)
	{
		LPoint3f min_point, max_point;
		mOwnerObject->getNodePath().calc_tight_bounds(min_point, max_point);
		float radius = sqrt(
				pow((max_point.get_x() - min_point.get_x()), 2)
						+ pow((max_point.get_y() - min_point.get_y()), 2))
				/ 2.0;
		float height = max_point.get_z() - min_point.get_z();
		// the NavMesh xml agent_radius/height will be overwritten
		//(may be in an unpredictable order) by the dimensions of
		//the CrowdAgents at startup, so they should have the
		//same dimensions to avoid strange results.
		//get nav mesh component
		SMARTPTR(NavMesh) navMesh =
				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
		//note: in threading hold the NavMesh mutex during the whole transaction.
		HOLDMUTEX(navMesh->getMutex())

		NavMeshSettings settings = navMesh->getNavMeshSettings();
		settings.m_agentRadius = radius;
		settings.m_agentHeight = height;
		navMesh->setNavMeshSettings(settings);
	}

	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

dtCrowdAgent* CrowdAgent::getDtAgent()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAgent;
}

int CrowdAgent::getIdx()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAgentIdx;
}

void CrowdAgent::setMovType(AgentMovType movType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mMovType = movType;
	if(mMovType == KINEMATIC)
	{
///TODO
	}
}

void CrowdAgent::setParams(const dtCrowdAgentParams& agentParams)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mAgentParams = agentParams;
	if(mNavMeshObject and mAddedToHandling)
	{
		//get nav mesh component
		SMARTPTR(NavMesh) navMesh =
				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
		dynamic_cast<CrowdTool*>(navMesh->getTool())->
				getState()->getCrowd()->updateAgentParameters(mAgentIdx, &mAgentParams);
	}
}

dtCrowdAgentParams CrowdAgent::getParams()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if(mNavMeshObject and mAddedToHandling)
	{
		//get nav mesh component
		SMARTPTR(NavMesh) navMesh =
				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
		mAgentParams = dynamic_cast<CrowdTool*>(navMesh->getTool())->
				getState()->getCrowd()->getAgent(mAgentIdx)->params;
	}
	return mAgentParams;
}

void CrowdAgent::setMoveTarget(const LPoint3f& pos)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if(mNavMeshObject and mAddedToHandling)
	{
		//get nav mesh component
		SMARTPTR(NavMesh) navMesh =
				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
		float p[3];
		LVecBase3fToRecast(pos, p);
		dynamic_cast<CrowdTool*>(navMesh->getTool())->
				getState()->setMoveTarget(mAgentIdx, p);
		mCurrentTarget = pos;
	}
}

LPoint3f CrowdAgent::getMoveTarget()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mCurrentTarget;
}

void CrowdAgent::setMoveVelocity(const LVector3f& vel)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if(mNavMeshObject and mAddedToHandling)
	{
		//get nav mesh component
		SMARTPTR(NavMesh) navMesh =
				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
		float v[3];
		LVecBase3fToRecast(vel, v);
		dynamic_cast<CrowdTool*>(navMesh->getTool())->
				getState()->setMoveVelocity(mAgentIdx,v);
		mCurrentVelocity = vel;
	}
}

LVector3f CrowdAgent::getMoveVelocity()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mCurrentVelocity;
}

void CrowdAgent::setNavMeshObject(SMARTPTR(Object)navMeshObject)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshObject = navMeshObject;
}

SMARTPTR(Object) CrowdAgent::getNavMeshObject()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshObject;
}

void CrowdAgent::addToNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if(mNavMeshObject and (not mAddedToHandling))
	{
		//get nav mesh component
		SMARTPTR(NavMesh) navMesh =
				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
		//add to nav mesh
		//note: in threading hold the NavMesh mutex during the whole transaction.
		HOLDMUTEX(navMesh->getMutex())

		LPoint3f pos;
		NodePath referenceNP = navMesh->getReferenceNP();
		NodePath ownerObjectNP = mOwnerObject->getNodePath();
		if(referenceNP != mReferenceNP)
		{
			//the owner object is reparented to the NavMesh reference node path
			pos = ownerObjectNP.get_pos(referenceNP);
			ownerObjectNP.reparent_to(referenceNP);
			ownerObjectNP.set_pos(pos);
		}
		else
		{
			pos = ownerObjectNP.get_pos();
		}
		//get recast p (y-up)
		float p[3];
		LVecBase3fToRecast(pos, p);
		//add recast agent
		mAgentIdx = dynamic_cast<CrowdTool*>(navMesh->getTool())
				->getState()->addAgent(p, &mAgentParams);
		//add Agent to list
		navMesh->addCrowdAgent(mOwnerObject);
		mAddedToHandling = true;
	}
}

void CrowdAgent::removeFromNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if(mNavMeshObject and mAddedToHandling)
	{
		//get nav mesh component
		SMARTPTR(NavMesh) navMesh =
				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
		//remove from nav mesh
		//note: in threading hold the NavMesh mutex during the whole transaction.
		HOLDMUTEX(navMesh->getMutex())

		if(navMesh->getReferenceNP() != mReferenceNP)
		{
			//the owner object is reparented to the original reference node path
			NodePath ownerObjectNP = mOwnerObject->getNodePath();
			LPoint3f pos = ownerObjectNP.get_pos(mReferenceNP);
			ownerObjectNP.reparent_to(mReferenceNP);
			ownerObjectNP.set_pos(pos);
		}
		//remove recast agent
		dynamic_cast<CrowdTool*>(navMesh->getTool())
				->getState()->removeAgent(mAgentIdx);
		//remove Agent from list
		navMesh->removeCrowdAgent(mOwnerObject);
		mAddedToHandling = false;
	}
}

///TODO
/*

#ifdef WITHCHARACTER
void CrowdAgent::updateVel(float dt, const LPoint3f& pos, const LVector3f& vel)
{
	m_vel = RecastToLVecBase3f(v);
	LVector3f direction = m_vel;
	if (m_vel.length_squared() > 0.1)
	{
		//set linear velocity
		DCAST(BulletCharacterControllerNode, m_pandaNP.node())->set_linear_movement(
				m_vel, false);
		//set angular velocity (in the x-y plane)
		//0 <= A <= 180.0
		direction.normalize();
		float H = m_pandaNP.get_h();
		float A = 57.295779513f * acos(direction.get_y());
		float deltaAngle;
		if (direction.get_x() <= 0.0)
		{
			if (H <= 0.0)
			{
				deltaAngle = -H + A - 180;
			}
			else
			{
				deltaAngle = (A <= H ? -H + A + 180 : -H + A - 180);
			}
		}
		else
		{
			if (H >= 0.0)
			{
				deltaAngle = -H - A + 180;
			}
			else
			{
				deltaAngle = (A >= -H ? -H - A + 180 : -H - A - 180);
			}
		}
		DCAST(BulletCharacterControllerNode, m_pandaNP.node())->set_angular_movement(
				deltaAngle);
//		LPoint3f lookAtPos = RecastToLVecBase3f(p) - m_pandaNP.get_pos() - m_vel * 100000;
//		m_pandaNP.heads_up(lookAtPos);
		//get current vel
		LVector3f currentVel = (m_pandaNP.get_pos() - m_oldPos) / dt;
		m_anims->get_anim(0)->set_play_rate(currentVel.length() / rateFactor);
		m_oldPos = m_pandaNP.get_pos();
//		m_anims->get_anim(0)->set_play_rate(m_vel.length() / rateFactor);
		if (not m_anims->get_anim(0)->is_playing())
		{
			m_anims->get_anim(0)->loop(true);
		}
	}
	else
	{
		DCAST(BulletCharacterControllerNode, m_pandaNP.node())->set_linear_movement(
				LVector3f::zero(), false);
		if (m_anims->get_anim(0)->is_playing())
		{
//			m_anims->get_anim(0)->pose(0);
			m_anims->get_anim(0)->stop();
		}
	}
}
#else
void CrowdAgent::updatePosDir(float dt, const LPoint3f& pos, const LVector3f& vel)
{
	//only for kinematic case
	//raycast in the near of recast mesh:
	//float rcConfig::detailSampleMaxError
	LPoint3f kinematicPos;
	if (vel.length_squared() > 0.1)
	{
		switch (mMovType)
		{
			case RECAST:
			mOwnerObject->getNodePath().set_pos(pos);
			break;
			case KINEMATIC:
			//set recast pos anyway
			kinematicPos = pos;
			//correct z
			//ray down
			m_result = m_world->ray_test_closest(kinematicPos + m_deltaRayOrig,
					kinematicPos + m_deltaRayDown, m_rayMask);
			if (m_result.has_hit())
			{
				//check if hit a triangle mesh
				BulletShape* shape =
				DCAST(BulletRigidBodyNode, m_result.get_node())->get_shape(0);
				if (shape->is_of_type(BulletTriangleMeshShape::get_class_type()))
				{
					//physic mesh is under recast mesh
					kinematicPos.set_z(m_result.get_hit_pos().get_z());
				}
			}
			m_pandaNP.set_pos(kinematicPos);
			break;
			case RIGID:
			DCAST(BulletSphericalConstraint, m_Cs)->set_pivot_b(pos);
			break;
			default:
			break;
		}
		//
		LPoint3f lookAtPos = m_pandaNP.get_pos() - vel * 100000;
		m_pandaNP.heads_up(lookAtPos);
		//get current vel
		LVector3f currentVel = (m_pandaNP.get_pos() - m_oldPos) / dt;
		m_anims->get_anim(0)->set_play_rate(currentVel.length() / rateFactor);
		m_oldPos = m_pandaNP.get_pos();
//		m_anims->get_anim(0)->set_play_rate(vel.length() / rateFactor);
		if (not m_anims->get_anim(0)->is_playing())
		{
			m_anims->get_anim(0)->loop(true);
		}
	}
	else
	{
		if (m_anims->get_anim(0)->is_playing())
		{
//			m_anims->get_anim(0)->pose(0);
			m_anims->get_anim(0)->stop();
		}
	}
}
#endif

*/

//TypedObject semantics: hardcoded
TypeHandle CrowdAgent::_type_handle;

}  // namespace ely

