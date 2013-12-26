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
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"
#include <throw_event.h>
#include <bulletTriangleMesh.h>
#include <bulletTriangleMeshShape.h>

namespace
{
const std::string STARTEVENT("OnStartCrowdAgent");
const std::string STOPEVENT("OnStopCrowdAgent");
}  // namespace

namespace ely
{

CrowdAgent::CrowdAgent():
		mHitResult(BulletClosestHitRayResult::empty())
{
}

CrowdAgent::CrowdAgent(SMARTPTR(CrowdAgentTemplate)tmpl):
				mHitResult(BulletClosestHitRayResult::empty())
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"CrowdAgent::CrowdAgent: invalid GameAIManager")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"CrowdAgent::CrowdAgent: invalid GamePhysicsManager")

	mTmpl = tmpl;
	mNavMesh.clear();
	reset();
}

CrowdAgent::~CrowdAgent()
{
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
	mNavMeshObjectId = ObjectId(
			mTmpl->parameter(std::string("add_to_navmesh")));
	//
	std::string param;
	unsigned int idx, valueNum;
	std::vector<std::string> paramValuesStr;
	//move target
	param = mTmpl->parameter(std::string("move_target"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 3)
	{
		paramValuesStr.resize(3, "0.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mMoveTarget[idx] = strtof(paramValuesStr[idx].c_str(), NULL);
	}
	//move velocity
	param = mTmpl->parameter(std::string("move_velocity"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 3)
	{
		paramValuesStr.resize(3, "0.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mMoveVelocity[idx] = strtof(paramValuesStr[idx].c_str(), NULL);
	}
	//
	float value;
	int valueInt;
	//max acceleration
	value = strtof(mTmpl->parameter(std::string("max_acceleration")).c_str(),
	NULL);
	mAgentParams.maxAcceleration = (value >= 0.0 ? value : -value);
	//max speed
	value = strtof(mTmpl->parameter(std::string("max_speed")).c_str(), NULL);
	mAgentParams.maxSpeed = (value >= 0.0 ? value : -value);
	//collision query range
	value = strtof(
			mTmpl->parameter(std::string("collision_query_range")).c_str(),
			NULL);
	mAgentParams.collisionQueryRange = (value >= 0.0 ? value : -value);
	//path optimization range
	value = strtof(
			mTmpl->parameter(std::string("path_optimization_range")).c_str(),
			NULL);
	mAgentParams.pathOptimizationRange = (value >= 0.0 ? value : -value);
	//separation weight
	value = strtof(mTmpl->parameter(std::string("separation_weight")).c_str(),
	NULL);
	mAgentParams.separationWeight = (value >= 0.0 ? value : -value);
	//update flags
	valueInt = strtol(mTmpl->parameter(std::string("update_flags")).c_str(),
			NULL, 0);
	mAgentParams.updateFlags = (valueInt >= 0.0 ? valueInt : -valueInt);
	//obstacle avoidance type
	valueInt = strtol(
			mTmpl->parameter(std::string("obstacle_avoidance_type")).c_str(),
			NULL, 0);
	mAgentParams.obstacleAvoidanceType =
			(valueInt >= 0.0 ? valueInt : -valueInt);
	//ray mask
	std::string rayMask = mTmpl->parameter(std::string("ray_mask"));
	if (rayMask == std::string("all_on"))
	{
		mRayMask = BitMask32::all_on();
	}
	else if (rayMask == std::string("all_off"))
	{
		mRayMask = BitMask32::all_off();
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(rayMask.c_str(), NULL, 0);
		mRayMask.set_word(mask);
	}
	//
	return result;
}

void CrowdAgent::onAddToObjectSetup()
{
	//set the bullet physics
	mBulletWorld = GamePhysicsManager::GetSingletonPtr()->bulletWorld();

#ifdef ELY_THREAD
	//add the task chain on which addCrowdAgentAsync() will be running
	mTaskChainName = mComponentId + "-taskChain";
	AsyncTaskManager::get_global_ptr()->make_task_chain(mTaskChainName);
	AsyncTaskManager::get_global_ptr()->
			find_task_chain(mTaskChainName)->set_num_threads(1);
	AsyncTaskManager::get_global_ptr()->
				find_task_chain(mTaskChainName)->set_frame_sync(false);
#endif
}

void CrowdAgent::onRemoveFromObjectCleanup()
{
#ifdef ELY_THREAD
	//remove the task chain on which addCrowdAgentAsync() has run
	AsyncTaskManager::get_global_ptr()->remove_task_chain(mTaskChainName);
#endif
	//
	mNavMesh.clear();
	reset();
}

void CrowdAgent::onAddToSceneSetup()
{
	///1: get the input from xml
	///2: add settings for CrowdAgent
	///set params: already done
	///set NavMesh object (if any)
	SMARTPTR(Object) navMeshObject = ObjectTemplateManager::
			GetSingleton().getCreatedObject(mNavMeshObjectId);

	///3: add to NavMesh update
	if(navMeshObject)
	{
		mStartNavMesh = DCAST(NavMesh, navMeshObject->getComponent(familyType()));
		//
		if(mStartNavMesh)
		{
			//create the task for executing addCrowdAgentAsync()
			//the task is executed only once and then removed from AsyncTaskManager
			mAddData.clear();
			mAddTask.clear();
			mAddData = new TaskInterface<CrowdAgent>::TaskData(this,
					&CrowdAgent::addCrowdAgentAsync);
			mAddTask = new GenericAsyncTask(mComponentId + "CrowdAgent::addCrowdAgentAsync",
					&TaskInterface<CrowdAgent>::taskFunction,
					reinterpret_cast<void*>(mAddData.p()));
			//set sort/priority
			mAddTask->set_sort(0);
			mAddTask->set_priority(0);
			//Add the task for updating the controlled object
#ifdef ELY_THREAD
			//add the task to the task chain.
			mAddTask->set_task_chain(mTaskChainName);
#endif
			//Adds mAddTask to the active queue.
			AsyncTaskManager::get_global_ptr()->add(mAddTask);
		}
	}
}

AsyncTask::DoneStatus CrowdAgent::addCrowdAgentAsync(GenericAsyncTask* task)
{
	mStartNavMesh->addCrowdAgent(this);
	//
	return AsyncTask::DS_done;
}

void CrowdAgent::onRemoveFromSceneCleanup()
{
	//lock (guard) the CrowdAgent NavMesh mutex
	HOLD_REMUTEX(mNavMeshMutex)

	///Remove from NavMesh update (if previously added)
	//mNavMesh will be cleared during removing, so
	//remove through a temporary pointer
	SMARTPTR(NavMesh) navMesh = mNavMesh;
	if(navMesh)
	{
#ifdef ELY_THREAD
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//removeCrowdAgent will return if mDestroying, so
		//disable it and re-enable afterwards
		mDestroying = false;
#endif
		navMesh->removeCrowdAgent(this);
#ifdef ELY_THREAD
		mDestroying = true;
#endif
	}
}

void CrowdAgent::setParams(const dtCrowdAgentParams& agentParams)
{
	//lock (guard) the CrowdAgent NavMesh mutex
	HOLD_REMUTEX(mNavMeshMutex)

	//return if crowdAgent doesn't belong to any mesh
	RETURN_ON_COND(not mNavMesh,)

	//request NavMesh to update move target for this CrowdAgent
	mNavMesh->setCrowdAgentParams(this, agentParams);
}

void CrowdAgent::setMoveTarget(const LPoint3f& pos)
{
	//lock (guard) the CrowdAgent NavMesh mutex
	HOLD_REMUTEX(mNavMeshMutex)

	//return if crowdAgent doesn't belong to any mesh
	RETURN_ON_COND(not mNavMesh,)

	//request NavMesh to update move target for this CrowdAgent
	mNavMesh->setCrowdAgentTarget(this, pos);
}

void CrowdAgent::setMoveVelocity(const LVector3f& vel)
{
	//lock (guard) the CrowdAgent NavMesh mutex
	HOLD_REMUTEX(mNavMeshMutex)

	//return if crowdAgent doesn't belong to any mesh
	RETURN_ON_COND(not mNavMesh,)

	//request NavMesh to update move target for this CrowdAgent
	mNavMesh->setCrowdAgentVelocity(this, vel);
}

void CrowdAgent::doUpdatePosDir(float dt, const LPoint3f& pos, const LVector3f& vel)
{
	if (vel.length_squared() > 0.0)
	{
		NodePath ownerObjectNP = mOwnerObject->getNodePath();
		LPoint3f updatedPos = pos;
		switch (mMovType)
		{
		case RECAST:
			break;
		case RECAST_KINEMATIC:
			//correct updatedPos.z (if needed)
			//ray down
			mHitResult = mBulletWorld->ray_test_closest(
					updatedPos + mDeltaRayOrig, updatedPos + mDeltaRayDown,
					mRayMask);
			if (mHitResult.has_hit())
			{
				//check if hit a triangle mesh
				BulletShape* shape =
				DCAST(BulletRigidBodyNode, mHitResult.get_node())->get_shape(
						0);
				if (shape
						and shape->is_of_type(
								BulletTriangleMeshShape::get_class_type()))
				{
					//physic mesh is under recast mesh
					updatedPos.set_z(mHitResult.get_hit_pos().get_z() + mCorrectHeightRigidBody);
				}
			}
			break;
		default:
			break;
		}
		//update node path pos
		ownerObjectNP.set_pos(updatedPos);
		//update node path dir
		ownerObjectNP.heads_up(updatedPos - vel);

		//throw CrowdAgentStart event (if enabled)
		if (mThrowEvents and (not mCrowdAgentStartSent))
		{
			throw_event(STARTEVENT, EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mCrowdAgentStartSent = true;
			mCrowdAgentStopSent = false;
		}
	}
	else
	{
		//throw CrowdAgentStop event (if enabled)
		if (mThrowEvents and (not mCrowdAgentStopSent))
		{
			throw_event(STOPEVENT,
					EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mCrowdAgentStopSent = true;
			mCrowdAgentStartSent = false;
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle CrowdAgent::_type_handle;

}  // namespace ely

