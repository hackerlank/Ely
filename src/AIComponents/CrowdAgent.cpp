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
#include "AIComponents/NavMesh.h"
#include "Support/RecastNavigationLocal/CrowdTool.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"
#include <bulletTriangleMesh.h>
#include <bulletTriangleMeshShape.h>

namespace ely
{

CrowdAgent::CrowdAgent(SMARTPTR(CrowdAgentTemplate)tmpl):
				mHitResult(BulletClosestHitRayResult::empty())
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"CrowdAgent::CrowdAgent: invalid GameAIManager")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"CrowdAgent::CrowdAgent: invalid GamePhysicsManager")

	mTmpl = tmpl;
	mNavMesh.clear();
	mStartNavMesh.clear();
	reset();
}

CrowdAgent::~CrowdAgent()
{
}

bool CrowdAgent::initialize()
{
	bool result = true;
	//set CrowdAgent parameters
	//register to navmesh objectId
	mNavMeshObjectId = ObjectId(
			mTmpl->parameter(std::string("add_to_navmesh")));
	//mov type
	std::string movType = mTmpl->parameter(std::string("mov_type"));
	if (movType == std::string("kinematic"))
	{
		CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
				"NavMesh::initialize: invalid GamePhysicsManager")
		mMovType = RECAST_KINEMATIC;
	}
	else
	{
		mMovType = RECAST;
	}
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
	//thrown events
	mThrownEventsParam = mTmpl->parameter(std::string("thrown_events"));
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

	//set thrown events if any
	unsigned int idx1, valueNum1;
	std::vector<std::string> paramValuesStr1, paramValuesStr2;
	if(mThrownEventsParam != std::string(""))
	{
		//events specified
		//event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]
		paramValuesStr1 = parseCompoundString(mThrownEventsParam, ':');
		valueNum1 = paramValuesStr1.size();
		for (idx1 = 0; idx1 < valueNum1; ++idx1)
		{
			//eventX@[event_nameX]@[frequencyX]
			paramValuesStr2 = parseCompoundString(paramValuesStr1[idx1], '@');
			if (paramValuesStr2.size() >= 3)
			{
				EventThrown event;
				ThrowEventData eventData;
				//get default name prefix
				std::string objectType = std::string(
						mOwnerObject->objectTmpl()->objectType());
				//get name
				std::string name = paramValuesStr2[1];
				//get frequency
				float frequency = strtof(paramValuesStr2[2].c_str(), NULL);
				if (frequency <= 0.0)
				{
					frequency = 30.0;
				}
				//get event
				if (paramValuesStr2[0] == "move")
				{
					event = MOVEEVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_CrowdAgent_Move";
					}
				}
				else if (paramValuesStr2[0] == "steady")
				{
					event = STEADYEVENT;
					//check name
					if (name == "")
					{
						//set default name
						name = objectType + "_CrowdAgent_Steady";
					}
				}
				else
				{
					//paramValuesStr2[0] is not a suitable event:
					//continue with the next event
					continue;
				}
				//set event data
				eventData.mEnable = true;
				eventData.mEventName = name;
				eventData.mTimeElapsed = 0;
				eventData.mFrequency = frequency;
				//enable the event
				doEnableCrowdAgentEvent(event, eventData);
			}
		}
	}
	//clear all no more needed "Param" variables
	mThrownEventsParam.clear();
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
	//1: get the input from xml
	//2: add settings for CrowdAgent
	//set params: already done
	//set NavMesh object (if any)
	SMARTPTR(Object) navMeshObject = ObjectTemplateManager::
			GetSingleton().getCreatedObject(mNavMeshObjectId);

	//3: add to NavMesh update
	if(navMeshObject)
	{
		SMARTPTR(Component) aiComp = navMeshObject->getComponent(componentFamilyType());
		//
		if(aiComp and (aiComp->componentType() == ComponentType("NavMesh")))
		{
			mStartNavMesh = DCAST(NavMesh, aiComp);
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

	//Remove from NavMesh update (if previously added)
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

SMARTPTR(NavMesh) CrowdAgent::getNavMesh() const
{
	//lock (guard) the CrowdAgent NavMesh mutex
	HOLD_REMUTEX(mNavMeshMutex)

	return mNavMesh;
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
		{
			//correct updatedPos.z (if needed)
			HOLD_REMUTEX(GamePhysicsManager::GetSingletonPtr()->getMutex())
			{
				//ray down
				mHitResult = mBulletWorld->ray_test_closest(
						updatedPos + mDeltaRayOrig, updatedPos + mDeltaRayDown,
						mRayMask);
			}
			if (mHitResult.has_hit())
			{
				//updatedPos.z needs correction
				updatedPos.set_z(mHitResult.get_hit_pos().get_z());
			}
		}
			break;
		default:
			break;
		}
		//correct z if there is a kinematic rigid body
		updatedPos.set_z(updatedPos.get_z() + mCorrectHeightRigidBody);
		//update node path pos
		ownerObjectNP.set_pos(updatedPos);
		//update node path dir
		ownerObjectNP.heads_up(updatedPos - vel);

		//handle events
		//throw Move event (if enabled)
		if (mMove.mEnable)
		{
			doThrowEvent(mMove);
		}
		//reset Steady event (if enabled and if thrown)
		if (mSteady.mEnable and mSteady.mThrown)
		{
			mSteady.mThrown = false;
			mSteady.mTimeElapsed = 0.0;
		}
	}
	else
	{
		//handle events
		//vel.length_squared() == 0.0
		//reset Move event (if enabled and if thrown)
		if (mMove.mEnable and mMove.mThrown)
		{
			mMove.mThrown = false;
			mMove.mTimeElapsed = 0.0;
		}
		//throw Steady event (if enabled)
		if (mSteady.mEnable)
		{
			doThrowEvent(mSteady);
		}
	}
}


void CrowdAgent::doEnableCrowdAgentEvent(EventThrown event, ThrowEventData eventData)
{
	//some checks
	RETURN_ON_COND(eventData.mEventName == std::string(""),)
	if (eventData.mFrequency <= 0.0)
	{
		eventData.mFrequency = 30.0;
	}

	switch (event)
	{
	case MOVEEVENT:
		if(mMove.mEnable != eventData.mEnable)
		{
			mMove = eventData;
			mMove.mTimeElapsed = 0;
		}
		break;
	case STEADYEVENT:
		if(mSteady.mEnable != eventData.mEnable)
		{
			mSteady = eventData;
			mSteady.mTimeElapsed = 0;
		}
		break;
	default:
		break;
	}
}

//TypedObject semantics: hardcoded
TypeHandle CrowdAgent::_type_handle;

///Template

CrowdAgentTemplate::CrowdAgentTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid GameAIManager")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"CrowdAgentTemplate::CrowdAgentTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

CrowdAgentTemplate::~CrowdAgentTemplate()
{
	
}

ComponentType CrowdAgentTemplate::componentType() const
{
	return ComponentType(CrowdAgent::get_class_type().get_name());
}

ComponentFamilyType CrowdAgentTemplate::componentFamilyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)CrowdAgentTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(CrowdAgent) newCrowdAgent = new CrowdAgent(this);
	newCrowdAgent->setComponentId(compId);
	if (not newCrowdAgent->initialize())
	{
		return NULL;
	}
	return newCrowdAgent.p();
}

void CrowdAgentTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("add_to_navmesh", ""));
	mParameterTable.insert(ParameterNameValue("mov_type", "recast"));
	mParameterTable.insert(ParameterNameValue("move_target", "0.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("move_velocity", "0.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("max_acceleration", "8.0"));
	mParameterTable.insert(ParameterNameValue("max_speed", "3.5"));
	mParameterTable.insert(ParameterNameValue("collision_query_range", "12.0"));
	mParameterTable.insert(ParameterNameValue("path_optimization_range", "30.0"));
	mParameterTable.insert(ParameterNameValue("separation_weight", "2.0"));
	mParameterTable.insert(ParameterNameValue("update_flags", "0x1b"));
	mParameterTable.insert(ParameterNameValue("obstacle_avoidance_type", "3"));
	mParameterTable.insert(ParameterNameValue("ray_mask", "all_on"));
}

//TypedObject semantics: hardcoded
TypeHandle CrowdAgentTemplate::_type_handle;

}  // namespace ely
