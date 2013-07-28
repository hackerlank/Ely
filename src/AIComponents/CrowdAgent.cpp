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
	//get NavMesh from owner object if any
	mNavMeshObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			mNavMeshObjectId);

	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void CrowdAgent::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set original referenceNP
	mReferenceNP = mOwnerObject->getNodePath().get_parent();
	//add to nav mesh
	addToNavMesh();
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

void CrowdAgent::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

}

//TypedObject semantics: hardcoded
TypeHandle CrowdAgent::_type_handle;

}  // namespace ely

