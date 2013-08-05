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
	mMovType = RECAST;
}

CrowdAgent::~CrowdAgent()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	removeFromNavMesh();
}

ComponentFamilyType CrowdAgent::familyType() const
{
	return mTmpl->familyType();
}

ComponentType CrowdAgent::componentType() const
{
	return mTmpl->componentType();
}

bool CrowdAgent::initialize()
{
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
	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void CrowdAgent::onAddToSceneSetup()
{
	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	///1: get the input from xml
	///2: add settings for CrowdAgent
	///set params: already done
	///set NavMesh object (if any)
	mNavMeshObject = ObjectTemplateManager::
			GetSingleton().getCreatedObject(mNavMeshObjectId);

	if(mNavMeshObject)
	{
		///3: add to the NavMesh
		addToNavMesh();
	}

	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void CrowdAgent::setParams(const dtCrowdAgentParams& agentParams)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mAgentParams = agentParams;
	if(mNavMeshObject)
	{
//		//get nav mesh component
//		SMARTPTR(NavMesh) navMesh =
//				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
//		navMesh->updateParams(mOwnerObject, agentParams);
	}
}

void CrowdAgent::setMoveTarget(const LPoint3f& pos)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mCurrentTarget = pos;
	if(mNavMeshObject)
	{
//		//get nav mesh component
//		SMARTPTR(NavMesh) navMesh =
//				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
//		navMesh->updateMoveTarget(mOwnerObject, pos);
	}
}

void CrowdAgent::setMoveVelocity(const LVector3f& vel)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mCurrentVelocity = vel;
	if(mNavMeshObject)
	{
//		//get nav mesh component
//		SMARTPTR(NavMesh) navMesh =
//				DCAST(NavMesh, mNavMeshObject->getComponent(componentType()));
//		navMesh->updateMoveVelocity(mOwnerObject, vel);
	}
}

void CrowdAgent::addToNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if(mNavMeshObject)
	{
		///get NavMesh component
		SMARTPTR(NavMesh) navMesh = DCAST(NavMesh,
				mNavMeshObject->getComponent(componentType()));

		///NavMesh object updates pos/vel wrt its reference node path
		LPoint3f pos;
		NodePath navMeshRefNP = navMesh->getReferenceNP();
		NodePath ownerObjectRefNP = mOwnerObject->getNodePath().get_parent();
		if(navMeshRefNP != ownerObjectRefNP)
		{
			//the owner object is reparented to the NavMesh
			//object reference node path
			pos = mOwnerObject->getNodePath().get_pos(navMeshRefNP);
			mOwnerObject->getNodePath().reparent_to(navMeshRefNP);
			mOwnerObject->getNodePath().set_pos(pos);
		}
		else
		{
			pos = mOwnerObject->getNodePath().get_pos();
		}

		///set the mov type of the crowd agent
		mMovType = navMesh->getMovType();

		///add to NavMesh and set the id of CrowdAgent
		mAgentIdx = navMesh->addCrowdAgent(this, pos, mAgentParams);
	}
}

void CrowdAgent::removeFromNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if(mNavMeshObject)
	{
		///get NavMesh component
		SMARTPTR(NavMesh) navMesh = DCAST(NavMesh,
				getNavMeshObject()->getComponent(componentType()));

		///remove from NavMesh
		navMesh->removeCrowdAgent(this, getIdx());

		///set the mov type of the crowd agent to default (RECAST)
		setMovType(RECAST);
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

