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
 * \file /Ely/include/AIComponents/CrowdAgent.h
 *
 * \date 06/giu/2013 (19:27:21)
 * \author consultit
 */

#ifndef CROWDAGENT_H_
#define CROWDAGENT_H_

#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include <DetourCrowd.h>
#include <bulletWorld.h>
#include <bulletClosestHitRayResult.h>

namespace ely
{

class CrowdAgentTemplate;
class NavMesh;

///Agent movement type.
enum AgentMovTypeEnum
{
	RECAST,
	RECAST_KINEMATIC,
	AgentMovType_NONE
};

/**
 * \brief Component implementing dtCrowdAgent from Recast Navigation library.
 *
 * \see https://code.google.com/p/recastnavigation
 * 		http://digestingduck.blogspot.it
 * 		https://groups.google.com/forum/?fromgroups#!forum/recastnavigation
 *
 * This component should be associated to a "Scene" component.\n
 * It could be associated to a kinematic rigid body, if the associated NavMesh
 * movement type is "kinematic".\n
 * If enabled, this component will throw an event on starting to move
 * ("OnStartCrowdAgent"), and an event on stopping to move
 * ("OnStopCrowdAgent"). The second argument of both is a reference
 * to the owner object.\n
 * \note the owner object of this component will be reparented (if necessary)
 * when added to a NavMesh, to the same reference node (i.e. parent) of
 * the NavMesh owner object.
 *
 * XML Param(s):
 * - "throw_events"						|single|"false"
 * - "add_to_navmesh"					|single|""
 * - "move_target";						|single|"0.0,0.0,0.0"
 * - "move_velocity";					|single|"0.0,0.0,0.0"
 * - "max_acceleration";				|single|"8.0"
 * - "max_speed"						|single|"3.5"
 * - "collision_query_range"			|single|"12.0" (* NavMesh::agent_radius)
 * - "path_optimization_range"			|single|"30.0" (* NavMesh::agent_radius)
 * - "separation_weight" 				|single|"2.0"
 * - "update_flags"						|single|"0x1b"
 * - "obstacle_avoidance_type"			|single|"3" (values: 0|1|2|3)
 * - "ray_mask"							|single|"all_on"
 *
 * \note parts inside [] are optional.\n
 */
class CrowdAgent: public Component
{
protected:
	friend class CrowdAgentTemplate;
	friend class NavMesh;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	CrowdAgent();
	CrowdAgent(SMARTPTR(CrowdAgentTemplate)tmpl);
	virtual ~CrowdAgent();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \name Helper getters/setters of CrowdAgent data.
	 *
	 * \note Getters/setters from NavMesh should be used for
	 * performance and detailed return result.
	 */
	///@{
	void setParams(const dtCrowdAgentParams& agentParams);
	dtCrowdAgentParams getParams();
	void setMoveTarget(const LPoint3f& pos);
	LPoint3f getMoveTarget();
	void setMoveVelocity(const LVector3f& vel);
	LVector3f getMoveVelocity();
	///@}

	/**
	 * \brief Get the NavMesh object reference mutex.
	 * @return The NavMesh mutex.
	 */
	ReMutex& getNavMeshMutex();

private:

	///The NavMesh owner object.
	SMARTPTR(NavMesh) mNavMesh;
	ObjectId mNavMeshObjectId;
	///The movement type.
	AgentMovTypeEnum mMovType;
	///The CrowdAgent index.
	int mAgentIdx;
	///The associated dtCrowdAgent data.
	///@{
	dtCrowdAgentParams mAgentParams;
	LPoint3f mMoveTarget;
	LVector3f mMoveVelocity;
	///@}
	/**
	 * \brief Physics data.
	 */
	///@{
	SMARTPTR(BulletWorld) mBulletWorld;
	float mMaxError;
	LVector3f mDeltaRayDown, mDeltaRayOrig;
	BulletClosestHitRayResult mHitResult;
	BitMask32 mRayMask;
	///@}
	/**
	 * \brief Updates position/velocity/orientation of the controlled object.
	 *
	 * Will be called automatically by the NavMesh update.
	 * @param dt The delta frame time.
	 * @param pos The new position.
	 * @param vel The new velocity.
	 */
	void doUpdatePosDir(float dt, const LPoint3f& pos, const LVector3f& vel);

	///Throwing events.
	bool mThrowEvents, mCrowdAgentStartSent, mCrowdAgentStopSent;

	///@{
	///A task data to do asynchronous adding to NavMesh during creation.
	SMARTPTR(TaskInterface<CrowdAgent>::TaskData) mAddData;
	SMARTPTR(AsyncTask) mAddTask;
	AsyncTask::DoneStatus addCrowdAgentAsync(GenericAsyncTask* task);
	SMARTPTR(NavMesh) mStartNavMesh;
#ifdef ELY_THREAD
	std::string mTaskChainName;
#endif
	///@}

	///Protects the NavMesh object reference (mNavMesh).
	ReMutex mNavMeshMutex;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "CrowdAgent", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

///inline definitions

inline void CrowdAgent::reset()
{
	//
	mNavMeshObjectId = ObjectId();
	mMovType = RECAST;
	mAgentIdx = -1;
	mAgentParams = dtCrowdAgentParams();
	mMoveTarget = LPoint3f::zero();
	mMoveVelocity = LVector3f::zero();
	mBulletWorld.clear();
	mMaxError = 0.0;
	mDeltaRayDown = mDeltaRayOrig = LVector3f::zero();
	mHitResult = BulletClosestHitRayResult::empty();
	mRayMask = BitMask32::all_off();
	mThrowEvents = mCrowdAgentStartSent = mCrowdAgentStopSent = false;
}

inline dtCrowdAgentParams CrowdAgent::getParams()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAgentParams;
}

inline LPoint3f CrowdAgent::getMoveTarget()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMoveTarget;
}

inline LVector3f CrowdAgent::getMoveVelocity()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMoveVelocity;
}

inline ReMutex& CrowdAgent::getNavMeshMutex()
{
	return mNavMeshMutex;
}

}  // namespace ely

#endif /* CROWDAGENT_H_ */
