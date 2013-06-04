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
 * \author consultit
 */

#ifndef RN_H_
#define RN_H_

#include "recastnavigation_data.h"

//rn
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
#include "ConvexVolumeTool.h"
#include "eggToObj.h"
#include <cstdio>
#include <cstring>
#include <virtualFileSystem.h>
#include <filename.h>

//#define TESTANOMALIES
#define DEBUG_DRAW

//RN
//Sample type
enum SAMPLETYPE
{
	SOLO, TILE, OBSTACLE
};

class Agent
{
	int m_agentIdx;
	MOVTYPE m_movType;
	NodePath m_pandaNP;
	LVector3f m_vel;
	LPoint3f m_oldPos;
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
			float maxError = 0.0, float radius = 1.0,
			BitMask32 rayMask=BitMask32::all_on()) :
			m_agentIdx(agentIdx),
			m_movType(movType),
			m_pandaNP(pandaNP),
			m_anims(anims),
			m_Cs(cs),
			m_world(world),
			m_maxError(maxError),
			m_radius(radius),
			m_result(BulletClosestHitRayResult::empty()),
			m_rayMask(rayMask)
	{
		m_deltaRayOrig = LVector3f(0, 0, m_maxError);
		m_deltaRayDown = LVector3f(0, 0, -10*m_maxError);
	}
	~Agent()
	{
	}
	int getIdx()
	{
		return m_agentIdx;
	}
#ifndef WITHCHARACTER
	void updatePosDir(float dt, const float* p, const float* v);
#else
	void updateVel(float dt, const float* p, const float* v);
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

	ConvexVolumeTool* m_convexVolumeTool;
	CrowdTool* m_crowdTool;

	std::list<Agent*> m_agents;

public:
	RN(NodePath render, SMARTPTR(BulletWorld)bulletWorld = NULL);
	~RN();
	ConvexVolumeTool* getConvexVolumeTool()
	{
		return m_convexVolumeTool;
	}
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
	bool loadGeomMesh(const std::string& path, const std::string& meshName,
			float scale=1.0);
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

	//convex volume tool
	void setConvexVolumeTool(NodePath renderDebug);

	//crowd tool
	void setCrowdTool();
	int addCrowdAgent(MOVTYPE movType,NodePath pandaNP, LPoint3f pos, float agentSpeed,
			AnimControlCollection* anims = NULL,
			BulletConstraint* cs = NULL, BulletWorld* world = NULL,
			float maxError = 0.0, float radius = 1.0, float height = 1.0,
			BitMask32 rayMask=BitMask32::all_on());
	//area costs and flags used by crowd
	void setCrowdAreaCost(SamplePolyAreas area, float cost);
	void setCrowdIncludeFlag(int flag);
	int getCrowdIncludeFlag();
	void setCrowdExcludeFlag(int flag);
	int getCrowdExcludeFlag();

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

///Obstacles data
extern std::string obstacleName;
extern NodePath obstacleNP;
extern std::map<NodePath, TempObstacle*> obstacleTable;

//CALLBACKS
extern const int CALLBACKSNUM;
//
void addConvexVolume(Raycaster* raycaster, void* data);
extern const int ADD_CONVEX_VOLUME_Idx;
extern std::string ADD_CONVEX_VOLUME_Key;
void removeConvexVolume(Raycaster* raycaster, void* data);
extern const int REMOVE_CONVEX_VOLUME_Idx;
extern std::string REMOVE_CONVEX_VOLUME_Key;
//
void setCrowdTarget(Raycaster* raycaster, void* data);
extern const int SET_CROWD_TARGET_Idx;
extern std::string SET_CROWD_TARGET_Key;
//
void buildTile(Raycaster* raycaster, void* data);
extern const int BUILD_TILE_Idx;
extern std::string BUILD_TILE_Key;
void removeTile(Raycaster* raycaster, void* data);
extern const int REMOVE_TILE_Idx;
extern std::string REMOVE_TILE_Key;
//
void addObstacle(Raycaster* raycaster, void* data);
extern const int ADD_OBSTACLE_Idx;
extern std::string ADD_OBSTACLE_Key;
void removeObstacle(Raycaster* raycaster, void* data);
extern const int REMOVE_OBSTACLE_Idx;
extern std::string REMOVE_OBSTACLE_Key;
//
void switchDoor(Raycaster* raycaster, void* data);
extern const int SET_SWITCH_DOOR_Idx;
extern std::string SET_SWITCH_DOOR_Key;
//
extern float m_agentMaxSlope;
extern float m_agentMaxClimb;
extern float m_cellSize;
extern float m_cellHeight;

struct App
{
	BitMask32 allOnButZeroMask;
	BitMask32 allOffButZeroMask;
	//
	SAMPLETYPE sampleType;
	MOVTYPE movType;
	LPoint3f agentPos;
	bool debugPhysics;
	PandaFramework* panda;
	WindowFramework* window;
	SMARTPTR(BulletWorld)mBulletWorld;
#ifdef DEBUG_DRAW
	NodePath renderDebug;
#endif
	NodePath worldMesh;
	float meshScale;
	float characterRadius, characterHeight;
	BulletConstraint* cs;
	NodePath character;
	RN* rn;
	TileSettings tileSettings;
	SampleSettings settings;
	//continue callback
	SMARTPTR(EventCallbackInterface<App>::EventCallbackData) myDataContinue;
	void continueCallback(const Event* event);
	void setContinueCallback(const std::string& event);
	//set convex volume area type callback
	SMARTPTR(EventCallbackInterface<App>::EventCallbackData) myDataAreaType;
	void areaTypeCallback(const Event* event);
	void setAreaTypeCallback(const std::string& event);
};

#endif /* RN_H_ */
