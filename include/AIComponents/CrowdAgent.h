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

namespace ely
{

class CrowdAgentTemplate;
class NavMesh;

///Agent movement type.
enum AgentMovType
{
#ifndef WITHCHARACTER
	RECAST, KINEMATIC
#else
	CHARACTER
#endif
};

/**
 * \brief Component implementing dtCrowdAgent from Recast Navigation library.
 *
 * \see https://code.google.com/p/recastnavigation
 * 		http://digestingduck.blogspot.it
 * 		https://groups.google.com/forum/?fromgroups#!forum/recastnavigation
 *
 * This component should be associated to a "Scene" component.\n
 * \note the owner object of this component will be reparented, if necessary,
 * to the same reference node (i.e. parent) of the NavMesh object
 * to which it is added.
 *
 * XML Param(s):
 * - "throw_events"						|single|"false"
 * - "add_to_navmesh"					|single|""
 * - "max_acceleration";				|single|"8.0"
 * - "max_speed"						|single|"3.5"
 * - "collision_query_range"			|single|"12.0" (* NavMesh::agent_radius)
 * - "path_optimization_range"			|single|"30.0" (* NavMesh::agent_radius)
 * - "separation_weight" 				|single|"2.0"
 * - "update_flags"						|single|"0x1b"
 * - "obstacle_avoidance_type"			|single|"3" (0,1,2,3)
 */
class CrowdAgent: public Component
{
protected:
	friend class Object;
	friend class CrowdAgentTemplate;
	friend class NavMesh;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \name Private getters/setters of NavMesh data.
	 *
	 * Called only by NavMesh methods: there is no need
	 * to hold component mutex.
	 */
	///@{
	void setMovType(AgentMovType movType);
	void setIdx(int idx);
	int getIdx();
	void setNavMeshObject(SMARTPTR(Object) navMeshObject);
	///@}

public:
	CrowdAgent();
	CrowdAgent(SMARTPTR(CrowdAgentTemplate)tmpl);
	virtual ~CrowdAgent();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \name Getters/setters of CrowdAgent data.
	 */
	///@{
	void setParams(const dtCrowdAgentParams& agentParams);
	dtCrowdAgentParams getParams();
	void setMoveTarget(const LPoint3f& pos);
	LPoint3f getMoveTarget();
	void setMoveVelocity(const LVector3f& vel);
	LVector3f getMoveVelocity();
	//update flags
	bool paramsRequestUpdate(dtCrowdAgentParams& agentParams);
	bool targetRequestUpdate(LPoint3f& pos);
	bool velocityRequestUpdate(LVector3f& vel);
	///@}

private:

	///Throwing events.
	bool mThrowEvents;
	///The NavMesh owner object.
	SMARTPTR(Object) mNavMeshObject;
	std::string mNavMeshObjectId;
	///The movement type.
	AgentMovType mMovType;
	///The CrowdAgent index.
	int mAgentIdx;
	///The associated dtCrowdAgent data.
	///@{
	dtCrowdAgentParams mAgentParams;
	bool mParamsNeedsUpdate;
	LPoint3f mMoveTarget;
	bool mTargetNeedsUpdate;
	LVector3f mMoveVelocity;
	bool mVelocityNeedsUpdate;
	///@}

	///NavMesh is a friend only for calling these methods.
	friend NavMesh;
	/**
	 * \brief Physics data.
	 */
	///@{
//	BulletWorld* m_world;
	float m_maxError;
	LVector3f m_deltaRayDown, m_deltaRayOrig;
//	BulletClosestHitRayResult m_result;
	BitMask32 m_rayMask;
	///@}
	/**
	 * \brief Updates position/velocity/orientation of the controlled object.
	 *
	 * Will be called automatically by the NavMesh update.
	 * @param dt The delta frame time.
	 * @param pos The new position.
	 * @param vel The new velocity.
	 */
#ifdef WITHCHARACTER
	void updateVel(float dt, const LPoint3f& pos, const LVector3f& vel);
#else
	void updatePosDir(float dt, const LPoint3f& pos, const LVector3f& vel);
#endif

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

inline int CrowdAgent::getIdx()
{
	return mAgentIdx;
}

inline void CrowdAgent::setIdx(int idx)
{
	mAgentIdx = idx;
}

inline void CrowdAgent::setMovType(AgentMovType movType)
{
	mMovType = movType;
}

inline void CrowdAgent::setNavMeshObject(SMARTPTR(Object)navMeshObject)
{
	mNavMeshObject = navMeshObject;
}

inline dtCrowdAgentParams CrowdAgent::getParams()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAgentParams;
}

inline LPoint3f CrowdAgent::getMoveTarget()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mMoveTarget;
}

inline LVector3f CrowdAgent::getMoveVelocity()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mMoveVelocity;
}

inline bool CrowdAgent::paramsRequestUpdate(dtCrowdAgentParams& agentParams)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	///\see Manning.CPP.Concurrency.in.Action.Feb.2012
	if(not mParamsNeedsUpdate)
	{
		return false;
	}
	//update needed
	agentParams = mAgentParams;
	//reset request
	mParamsNeedsUpdate = false;
	return true;
}

inline bool CrowdAgent::targetRequestUpdate(LPoint3f& pos)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	///\see Manning.CPP.Concurrency.in.Action.Feb.2012
	if(not mTargetNeedsUpdate)
	{
		return false;
	}
	//update needed
	pos = mMoveTarget;
	//reset request
	mTargetNeedsUpdate = false;
	return true;
}

inline bool CrowdAgent::velocityRequestUpdate(LVector3f& vel)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	///\see Manning.CPP.Concurrency.in.Action.Feb.2012
	if(not mVelocityNeedsUpdate)
	{
		return false;
	}
	//update needed
	vel = mMoveVelocity;
	//reset request
	mVelocityNeedsUpdate = false;
	return true;
}

}  // namespace ely

#endif /* CROWDAGENT_H_ */
