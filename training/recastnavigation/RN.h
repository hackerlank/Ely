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
 * \file /Ely/training/recastnavigation/RN.h
 *
 * \date 29/mar/2013 (18:59:07)
 * \author marco
 */

#ifndef RN_H_
#define RN_H_

#include <lvector3.h>
#include <nodePath.h>
#include <animControlCollection.h>
#include <genericAsyncTask.h>
#include <bulletWorld.h>
#include <bulletCharacterControllerNode.h>
#include <bulletRigidBodyNode.h>
#include <bulletSphericalConstraint.h>
#include <bulletTriangleMeshShape.h>
#include <bulletTriangleMesh.h>

//rn
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourNavMeshBuilder.h>
#include <DetourCommon.h>
#include "InputGeom.h"
#include "Sample_SoloMesh.h"
#include "CrowdTool.h"

//#define TESTANOMALIES

//RN
//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/z$20axis/recastnavigation/fMqEAqSBOBk/zwOzHmjRsj0J
inline void LVecBase3fToRecast(const LVecBase3f& v, float* p)
{
	p[0] = v.get_x();
	p[1] = v.get_z();
	p[2] = -v.get_y();
}
inline LVecBase3f RecastToLVecBase3f(const float* p)
{
	return LVecBase3f(p[0], -p[2], p[1]);
}

extern const float agentMaxSpeed;
extern float playrate;
extern BitMask32 allOnButZeroMask;
extern BitMask32 allOffButZeroMask;

//Movement type
enum MOVTYPE
{
#ifndef WITHCHARACTER
	RECAST, KINEMATIC, RIGID
#else
	CHARACTER
#endif
};

class Agent
{
	int m_agentIdx;
	MOVTYPE m_movType;
	NodePath m_pandaNP;
	LVector3f m_vel;
	AnimControlCollection* m_anims;
	BulletConstraint* m_Cs;
	BulletWorld* m_world;
	float m_maxError, m_radius;
	LVector3f m_deltaRayDown, m_deltaRayOrig;
	BulletClosestHitRayResult m_result;
	BitMask32 m_rayMask;
public:
	Agent(int agentIdx, MOVTYPE movType, NodePath pandaNP, AnimControlCollection* anims,
			BulletConstraint* cs = NULL,
			BulletWorld* world=NULL,
			float maxError = 0.0, float radius = 1.0) :
			m_agentIdx(agentIdx),
			m_movType(movType),
			m_pandaNP(pandaNP),
			m_anims(anims),
			m_Cs(cs),
			m_world(world),
			m_maxError(maxError),
			m_radius(radius),
			m_result(BulletClosestHitRayResult::empty())
	{
		m_deltaRayOrig = LVector3f(0, 0, m_maxError);
		m_deltaRayDown = LVector3f(0, 0, -10*m_maxError);
		m_rayMask = allOnButZeroMask;
	}
	~Agent()
	{
	}
	int getIdx()
	{
		return m_agentIdx;
	}
#ifndef WITHCHARACTER
	void updatePosDir(const float* p, const float* v);
#else
	void updateVel(const float* p, const float* v);
#endif
	LPoint3f getPos()
	{
		return m_pandaNP.get_pos();
	}
	LVector3f getVel()
	{
		return m_vel;
	}
};

#ifdef TESTANOMALIES
struct AgentData{
	enum MEMBER
	{
		POS,
		VEL
	};
	Agent* agent;
	MEMBER member;
	std::string msg;
};
#endif

class RN
{
	std::string m_meshName;
	InputGeom* m_geom;
	BuildContext* m_ctx;

	Sample_SoloMesh* m_sampleSolo;

	CrowdTool* m_crowdTool;

	std::list<Agent*> m_agents;

public:
	RN();
	~RN();
	CrowdTool* getNavMeshTool()
	{
		return m_crowdTool;
	}
	Sample_SoloMesh* getSampleSolo()
	{
		return m_sampleSolo;
	}
	//common
	bool loadMesh(const std::string& path, const std::string& meshName);
	bool buildNavMesh();
	void createSoloMesh();
	void setSettings(const SampleSettings& settings);
	SampleSettings getSettings();
	//ai update functions
#ifndef WITHCHARACTER
	static AsyncTask::DoneStatus ai_update(GenericAsyncTask* task, void* data);
#else
	static AsyncTask::DoneStatus ai_updateCHARACTER(GenericAsyncTask* task, void* data);
#endif

	//crowd tool
	void setCrowdTool();
	int addCrowdAgent(MOVTYPE movType,NodePath pandaNP, LPoint3f pos, float agentSpeed,
			AnimControlCollection* anims = NULL,
			BulletConstraint* cs = NULL, BulletWorld* world = NULL,
			float maxError = 0.0, float radius = 1.0);
	class CompareIdx
	{
		int m_idx;
	public:
		CompareIdx(int idx)
		{
			m_idx=idx;
		}
		bool operator()(Agent* ag)
		{
			return m_idx == ag->getIdx();
		}
	};
	Agent* getCrowdAgent(int idx);
	void setCrowdTarget(LPoint3f pos);

};

#endif /* RN_H_ */
