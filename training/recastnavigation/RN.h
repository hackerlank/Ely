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
#include <bulletBoxShape.h>
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
#include <RecastDebugDraw.h>
#include <DetourDebugDraw.h>
#include <DetourTileCache.h>
#include "InputGeom.h"
#include "Sample_SoloMesh.h"
#include "Sample_TileMesh.h"
#include "Sample_TempObstacles.h"
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
inline LVecBase3f Recast3fToLVecBase3f(const float x, const float y, const float z)
{
	return LVecBase3f(x, -z, y);
}

extern const float agentMaxSpeed;
extern const float rateFactor;
extern BitMask32 allOnButZeroMask;
extern BitMask32 allOffButZeroMask;

//Sample type
enum SAMPLETYPE
{
	SOLO, TILE, OBSTACLE
};

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
	NodePath m_render;
	SMARTPTR(BulletWorld) m_bulletWorld;

	std::string m_meshName;
	InputGeom* m_geom;
	BuildContext* m_ctx;

	SAMPLETYPE m_sampleType;
	Sample* m_currentSample;

	CrowdTool* m_crowdTool;

	std::list<Agent*> m_agents;

public:
	RN(NodePath render, SMARTPTR(BulletWorld)bulletWorld = NULL);
	~RN();
	CrowdTool* getCrowdTool()
	{
		return m_crowdTool;
	}
	Sample* getSample()
	{
		return m_currentSample;
	}
	SAMPLETYPE getSampleType()
	{
		return m_sampleType;
	}
	NodePath getRender()
	{
		return m_render;
	}
	SMARTPTR(BulletWorld) getBulletWorld()
	{
		return m_bulletWorld;
	}

	//common
	bool loadGeomMesh(const std::string& path, const std::string& meshName);
	bool buildNavMesh();
	void createGeomMesh(Sample* currentSample, SAMPLETYPE sampleType=SOLO);
	void createTileMesh();
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
			float maxError = 0.0, float radius = 1.0, float height = 1.0);
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

struct TempObstacle
{
	NodePath m_nodePath;
	dtObstacleRef m_ref;
	float m_radius, m_heigth;
	SMARTPTR(BulletWorld) m_bulletWorld;
	dtTileCache* m_tileCache;
	//
	TempObstacle(NodePath baseModel, float scale,
			SMARTPTR(BulletWorld)bulletWorld):
				m_bulletWorld(bulletWorld)
	{
		NodePath modelInst("TempObstacleInst");
		baseModel.instance_to(modelInst);
		//
		modelInst.set_scale(scale);
		LPoint3f min_point, max_point;
		modelInst.calc_tight_bounds(min_point, max_point);
		float dX = max_point.get_x() - min_point.get_x();
		float dY = max_point.get_y() - min_point.get_y();
		m_radius = sqrt(pow(dX,2) + pow(dY,2)) / 2.0;
		m_heigth = max_point.get_z() - min_point.get_z();
		//
		m_nodePath = NodePath(new BulletRigidBodyNode("TempObstacleStatic"));
		DCAST(BulletRigidBodyNode, m_nodePath.node())->
		add_shape(new BulletBoxShape(LVecBase3f(dX / 2.0, dY / 2.0, m_heigth / 2.0)),
				TransformState::make_pos(LPoint3f(0, 0, m_heigth / 2.0)));
		m_nodePath.set_collide_mask(BitMask32::all_on());
		DCAST(BulletRigidBodyNode, m_nodePath.node())->set_mass(0.0);
		DCAST(BulletRigidBodyNode, m_nodePath.node())->set_kinematic(false);
		DCAST(BulletRigidBodyNode, m_nodePath.node())->set_static(true);
		DCAST(BulletRigidBodyNode, m_nodePath.node())->set_deactivation_enabled(true);
		DCAST(BulletRigidBodyNode, m_nodePath.node())->set_active(false);
		m_bulletWorld->attach(m_nodePath.node());
		//
		modelInst.reparent_to(m_nodePath);
		modelInst.set_pos(LVecBase3f(-dX / 2.0, -dY / 2.0, 0));
		//rename
		m_nodePath.set_name("TempObstacle");
	}
	virtual ~TempObstacle()
	{
		//remove panda obstacle
		m_bulletWorld->remove(m_nodePath.node());
		m_nodePath.remove_node();
		//remove recast obstacle
		m_tileCache->removeObstacle(m_ref);
	}
};

#endif /* RN_H_ */
