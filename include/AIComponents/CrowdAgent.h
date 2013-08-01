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
 * \note this component should be associated to a "Scene" component, and
 * should be created after it.
 * \note this component object should have the same reference node
 * (i.e. parent) of the NavMesh object to which it is added.
 *
 * XML Param(s):
 * - "throw_events"						|single|"false"
 * - "register_to_navmesh"				|single|""
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
public:
	CrowdAgent();
	CrowdAgent(SMARTPTR(CrowdAgentTemplate)tmpl);
	virtual ~CrowdAgent();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \name NavMesh & Crowd agent data
	 * \brief Gets/sets the associated NavMesh & crowd agent data.
	 */
	///@{
	dtCrowdAgent* getDtAgent();
	int getIdx();
	void setIdx(int idx);
	void setMovType(AgentMovType movType);
	void setParams(const dtCrowdAgentParams& agentParams);
	dtCrowdAgentParams getParams();
	void setMoveTarget(const LPoint3f& pos);
	LPoint3f getMoveTarget();
	void setMoveVelocity(const LVector3f& vel);
	LVector3f getMoveVelocity();
	void setNavMeshObject(SMARTPTR(Object) navMeshObject);
	SMARTPTR(Object) getNavMeshObject();
	///@}

	/**
	 * \brief Adds/removes this object to the NavMesh handling
	 */
	///@{
	void addToNavMesh();
	void removeFromNavMesh();
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
	///The associated recast dtCrowdAgent data.
	///@{
	dtCrowdAgent* mAgent;
	dtCrowdAgentParams mAgentParams;
	LPoint3f mCurrentTarget;
	LVector3f mCurrentVelocity;
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
}  // namespace ely

#endif /* CROWDAGENT_H_ */
