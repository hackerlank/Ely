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
 * \file /Ely/training/recastnavigation/RN.cpp
 *
 * \date 29/mar/2013 (18:59:28)
 * \author consultit
 */

#include "RN.h"

#ifndef WITHCHARACTER
void Agent::updatePosDir(float dt, const float* p, const float* v)
{
	LVector3f vel = RecastToLVecBase3f(v);
	LPoint3f pos = RecastToLVecBase3f(p);
	//only for kinematic case
	//raycast in the near of recast mesh:
	//float rcConfig::detailSampleMaxError
	LPoint3f kinematicPos;
	if (vel.length_squared() > 0.1)
	{
		switch (m_movType)
		{
			case RECAST:
			m_pandaNP.set_pos(pos);
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
#else
void Agent::updateVel(float dt, const float* p, const float* v)
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
#endif

////////////////////////////////////////////////////////////////////

RN::RN(NodePath render, SMARTPTR(BulletWorld)bulletWorld) :
m_render(render),
m_bulletWorld(bulletWorld),
m_meshName(""),
m_geom(0),
m_ctx(new BuildContext),
m_currentSample(0),
m_crowdTool(0),
m_agents()
{
}

RN::~RN()
{
	while (m_agents.size() > 0)
	{
		delete m_agents.front();
		m_agents.pop_front();
	}
	delete m_ctx;
}

bool RN::loadGeomMesh(const std::string& path, const std::string& meshName,
		float scale, LVector3f translation)
{
	bool result = true;
	m_geom = new InputGeom;
	m_meshName = meshName;
	//convert egg to obj: see panda3d/pandatool/src/objprogs/eggToObj{.h,.cxx}
	//input egg file name
	unsigned int inputNameLength = (path + meshName).length() + 1;
	char * inputFileName = new char[inputNameLength];
	strncpy(inputFileName, (path + meshName).c_str(), inputNameLength);
	//out obj file name
	char tmpOutFileName[L_tmpnam];
	tmpnam (tmpOutFileName);
	//egg2obj -C -cs y-up -o nav_test_panda.obj nav_test_panda.egg
	int argc = 7;
	char *argv[7];
	argv[0] = "egg2obj";
	argv[1] = "-C";
	argv[2] = "-cs";
	argv[3] = "y-up";
	argv[4] = "-o";
	argv[5] = tmpOutFileName;
	argv[6] = inputFileName;
	EggToObj prog;
	prog.parse_command_line(argc, argv);
	prog.run();
	delete[] inputFileName;
	//
	float translationRecast[3];
	LVecBase3fToRecast(translation, translationRecast);
	if (not m_geom->loadMesh(m_ctx, tmpOutFileName, NodePath(), scale, translationRecast))
	{
		delete m_geom;
		m_geom = NULL;
		m_ctx->dumpLog("Geom load log %s:", meshName.c_str());
		result = false;
	}
	//remove tmpOutFile
	VirtualFileSystem *vfs = VirtualFileSystem::get_global_ptr();
	Filename obj_filename = Filename::text_filename(std::string(tmpOutFileName));
	vfs->delete_file(obj_filename);
	return result;
}

bool RN::loadGeomMesh(NodePath model)
{
	bool result = true;
	m_geom = new InputGeom;
	m_meshName = model.get_name();
	//
	if (not m_geom->loadMesh(m_ctx, NULL, model))
	{
		delete m_geom;
		m_geom = NULL;
		m_ctx->dumpLog("Geom load log %s:", m_meshName.c_str());
		result = false;
	}
	return result;
}

void RN::setupNavMesh(Sample* currentSample, SAMPLETYPE sampleType)
{
	//set sample
	m_currentSample = currentSample;
	m_sampleType = sampleType;
	//set rcContext
	m_currentSample->setContext(m_ctx);
	//handle Mesh Changed
	m_currentSample->handleMeshChanged(m_geom);
}

bool RN::buildNavMesh()
{
	m_ctx->resetLog();
	//build navigation mesh
	bool result = m_currentSample->handleBuild();
	m_ctx->dumpLog("Build log %s:", m_meshName.c_str());
	return result;
}

#ifndef WITHCHARACTER
AsyncTask::DoneStatus RN::ai_update(GenericAsyncTask* task, void* data)
{
	float dt = ClockObject::get_global_clock()->get_dt();

	RN* thisInst = reinterpret_cast<RN*>(data);

	if(thisInst->m_crowdTool)
	{
		std::list<Agent*>::iterator iter;
		dtCrowd* crowd = thisInst->m_crowdTool->getState()->getCrowd();

		//update crowd agents' pos/vel
		thisInst->m_currentSample->handleUpdate(dt);

		//post-update all agent positions
		for (iter = thisInst->m_agents.begin(); iter != thisInst->m_agents.end();
				++iter)
		{
			const float* vel = crowd->getAgent((*iter)->getIdx())->vel;
			const float* pos = crowd->getAgent((*iter)->getIdx())->npos;
			(*iter)->updatePosDir(dt, pos, vel);
		}
	}
	//
#ifdef DEBUG_DRAW
	thisInst->getApp()->ddM->reset();
	thisInst->m_currentSample->renderToolStates(*(thisInst->getApp()->ddM));
#endif
	//
	return AsyncTask::DS_again;
}
#else
AsyncTask::DoneStatus RN::ai_updateCHARACTER(GenericAsyncTask* task, void* data)
{
	float dt = ClockObject::get_global_clock()->get_dt();

	RN* thisInst = reinterpret_cast<RN*>(data);

	std::list<Agent*>::iterator iter;
	dtCrowd* crowd = thisInst->m_crowdTool->getState()->getCrowd();

	//pre-update all crowd agents' positions
	for (iter = thisInst->m_agents.begin(); iter != thisInst->m_agents.end();
			++iter)
	{
		LVecBase3fToRecast((*iter)->getPos(),
				crowd->getAgent((*iter)->getIdx())->npos);
	}

	//update crowd agents' pos/vel
	thisInst->m_currentSample->handleUpdate(dt);

	//post-update all agent positions
	for (iter = thisInst->m_agents.begin(); iter != thisInst->m_agents.end();
			++iter)
	{
		const float* pos = crowd->getAgent((*iter)->getIdx())->npos;
		const float* vel = crowd->getAgent((*iter)->getIdx())->vel;
		(*iter)->updateVel(dt, pos, vel);
	}
	//
#ifdef DEBUG_DRAW
	thisInst->getApp()->ddM->reset();
	thisInst->m_currentSample->renderToolStates(*(thisInst->getApp()->ddM));
#endif
	//
	return AsyncTask::DS_again;
}
#endif

void RN::setConvexVolumeTool(NodePath renderDebug)
{
	//set ConvexVolumeTool
	m_convexVolumeTool = new ConvexVolumeTool();
	m_currentSample->setTool(m_convexVolumeTool);
}

void RN::setOffMeshConnectionTool(NodePath renderDebug)
{
	//set OffMeshConnectionTool
	m_offMeshConnectionTool = new OffMeshConnectionTool();
	m_currentSample->setTool(m_offMeshConnectionTool);
}

void RN::setCrowdTool()
{
	//set CrowdTool
	m_crowdTool = new CrowdTool;
	m_currentSample->setTool(m_crowdTool);
}

int RN::addCrowdAgent(MOVTYPE movType, NodePath pandaNP, LPoint3f pos,
		float agentMaxSpeed, AnimControlCollection* anims, BulletConstraint* cs,
		BulletWorld* world, float maxError, float radius, float height,
		BitMask32 rayMask)
{
	//get recast p (y-up)
	float p[3];
	LVecBase3fToRecast(pos, p);
	//add recast agent
	int agentIdx = m_crowdTool->getState()->addAgent(p);
	//set its desired speed
	dtCrowdAgentParams ap = m_crowdTool->getState()->getCrowd()->getAgent(
			agentIdx)->params;
	ap.maxSpeed = agentMaxSpeed;
	m_crowdTool->getState()->getCrowd()->updateAgentParameters(agentIdx, &ap);
	//add Agent
	Agent* agent = new Agent(agentIdx, movType, pandaNP, anims, cs, world,
			maxError, radius, rayMask);
	if (cs)
	{
		DCAST(BulletSphericalConstraint, cs)->set_pivot_b(pos);
	}
	else
	{
		//set Agent pos
#ifndef WITHCHARACTER
		pandaNP.set_pos(pos);
#else
		pandaNP.set_pos(pos + LVector3f(0, 0, (height / 2.0) * 1.1));
#endif
	}
	//add Agent to list
	m_agents.push_back(agent);

	return agentIdx;
}

Agent* RN::getCrowdAgent(int idx)
{
	Agent* agent = NULL;
	std::list<Agent*>::iterator iter = find_if(m_agents.begin(), m_agents.end(),
			CompareIdx(idx));
	if (iter != m_agents.end())
	{
		agent = (*iter);
	}
	return agent;
}

void RN::setSettings(const SampleSettings& settings)
{
	m_currentSample->setSampleSettings(settings);
}

SampleSettings RN::getSettings()
{
	return m_currentSample->getSampleSettings();
}

void RN::setCrowdAreaCost(SamplePolyAreas area, float cost)
{
	dtQueryFilter* filter =
			m_crowdTool->getState()->getCrowd()->getEditableFilter();
	filter->setAreaCost(area, cost);
}

void RN::setCrowdIncludeFlag(int flag)
{
	m_crowdTool->getState()->getCrowd()->getEditableFilter()->setIncludeFlags(
			flag);
}

int RN::getCrowdIncludeFlag()
{
	return m_crowdTool->getState()->getCrowd()->getFilter()->getIncludeFlags();
}

void RN::setCrowdExcludeFlag(int flag)
{
	m_crowdTool->getState()->getCrowd()->getEditableFilter()->setExcludeFlags(
			flag);
}

int RN::getCrowdExcludeFlag()
{
	return m_crowdTool->getState()->getCrowd()->getFilter()->getExcludeFlags();
}

void RN::setCrowdTarget(LPoint3f pos)
{
	float p[3];
	LVecBase3fToRecast(pos, p);
	m_crowdTool->getState()->setMoveTarget(p, false);
}

void RN::setCrowdVelocity(LPoint3f pos)
{
	float p[3];
	LVecBase3fToRecast(pos, p);
	m_crowdTool->getState()->setMoveTarget(p, true);
}

///
///Obstacles data
std::string obstacleName("box.egg");
NodePath obstacleNP;
std::map<NodePath, TempObstacle*> obstacleTable;

//CALLBACKS
const int CALLBACKSNUM = 7;
//
//void addConvexVolume(Raycaster* raycaster, void* data);
const int ADD_CONVEX_VOLUME_Idx = 0;
std::string ADD_CONVEX_VOLUME_Key("shift-mouse2");
//void removeConvexVolume(Raycaster* raycaster, void* data);
const int REMOVE_CONVEX_VOLUME_Idx = 1;
std::string REMOVE_CONVEX_VOLUME_Key("shift-alt-mouse2");
//
//void addOffMeshConnection(Raycaster* raycaster, void* data);
const int ADD_OFF_MESH_CONNECTION_Idx = 0;
std::string ADD_OFF_MESH_CONNECTION_Key("shift-mouse2");
//void removeOffMeshConnection(Raycaster* raycaster, void* data);
const int REMOVE_OFF_MESH_CONNECTION_Idx = 1;
std::string REMOVE_OFF_MESH_CONNECTION_Key("shift-alt-mouse2");
//
//void continueCallback(const Event* event, void* data);
//
//void setCrowdTarget(Raycaster* raycaster, void* data);
const int SET_CROWD_TARGET_Idx = 0;
std::string SET_CROWD_TARGET_Key("shift-mouse1");
//
//void setCrowdVelocity(Raycaster* raycaster, void* data);
const int SET_CROWD_VELOCITY_Idx = 6;
std::string SET_CROWD_VELOCITY_Key("control-mouse1");
//
//void buildTile(Raycaster* raycaster, void* data);
const int BUILD_TILE_Idx = 1;
std::string BUILD_TILE_Key("shift-mouse3");
//void removeTile(Raycaster* raycaster, void* data);
const int REMOVE_TILE_Idx = 2;
std::string REMOVE_TILE_Key("shift-alt-mouse3");
//
//void addObstacle(Raycaster* raycaster, void* data);
const int ADD_OBSTACLE_Idx = 3;
std::string ADD_OBSTACLE_Key("shift-mouse2");
//void removeObstacle(Raycaster* raycaster, void* data);
const int REMOVE_OBSTACLE_Idx = 4;
std::string REMOVE_OBSTACLE_Key("shift-alt-mouse2");
//
//void switchDoor(Raycaster* raycaster, void* data);
const int SET_SWITCH_DOOR_Idx = 5;
std::string SET_SWITCH_DOOR_Key("d");
//

void addConvexVolume(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	rn->getConvexVolumeTool()->handleClick(NULL, m_hitPos, false);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getConvexVolumeTool()->handleRender(*(rn->getApp()->dd));
	//not yet Off Mesh Connections
#endif
}

void removeConvexVolume(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	rn->getConvexVolumeTool()->handleClick(NULL, m_hitPos, true);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getConvexVolumeTool()->handleRender(*(rn->getApp()->dd));
	//not yet Off Mesh Connections
#endif
}

void addOffMeshConnection(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	rn->getOffMeshConnectionTool()->handleClick(NULL, m_hitPos, false);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getSample()->getInputGeom()->drawConvexVolumes(rn->getApp()->dd);
	rn->getOffMeshConnectionTool()->handleRender(*(rn->getApp()->dd));
#endif
}

void removeOffMeshConnection(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	rn->getOffMeshConnectionTool()->handleClick(NULL, m_hitPos, true);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getSample()->getInputGeom()->drawConvexVolumes(rn->getApp()->dd);
	rn->getOffMeshConnectionTool()->handleRender(*(rn->getApp()->dd));
#endif
}

void App::setAreaTypeCallback(const std::string& event)
{
	myDataAreaType = new EventCallbackInterface<App>::EventCallbackData(this,
			&App::areaTypeCallback);
	panda->define_key(event, "toggle area type",
			&EventCallbackInterface<App>::eventCallbackFunction,
			reinterpret_cast<void*>(myDataAreaType.p()));
}

float m_agentMaxSlope = 60.0;
float m_agentMaxClimb = 5.0;
float m_cellSize = 0.3;
float m_cellHeight = 0.2;

static std::string areaTypes[] =
		{ "SAMPLE_POLYAREA_GROUND", "SAMPLE_POLYAREA_WATER",
				"SAMPLE_POLYAREA_ROAD", "SAMPLE_POLYAREA_DOOR",
				"SAMPLE_POLYAREA_GRASS", "SAMPLE_POLYAREA_JUMP", };

void App::areaTypeCallback(const Event* event)
{
	//round robin types
	int currentType = (int) rn->getConvexVolumeTool()->getAreaType();
	++currentType;
	currentType = currentType % (int) SAMPLE_POLYAREA_END;
	rn->getConvexVolumeTool()->setAreaType((SamplePolyAreas) currentType);
	//print current area type
	std::cout << "\tCurrent area type: " << areaTypes[(int) currentType]
			<< std::endl;
}

void App::setContinueCallback(const std::string& event)
{
	myDataContinue = new EventCallbackInterface<App>::EventCallbackData(this,
			&App::continueCallback);
	panda->define_key(event, "continue app",
			&EventCallbackInterface<App>::eventCallbackFunction,
			reinterpret_cast<void*>(myDataContinue.p()));
}

void App::continueCallback(const Event* event)
{
	static int contValue = 0;
	if (contValue == 0)
	{
		setConvexVolumeTool();
		++contValue;
	}
	else if (contValue == 1)
	{
		setOffMeshConnectionTool();
		++contValue;
	}
	else
	{
		doFinalWork();
		++contValue;
	}
}

void App::setConvexVolumeTool()
{
	std::cout << "Set Convex Volumes ..." << std::endl;
	//set convex volume tool
	rn->setConvexVolumeTool(renderDebug);
	//set convex volume add/remove callbacks
	Raycaster::GetSingletonPtr()->setHitCallback(ADD_CONVEX_VOLUME_Idx,
			addConvexVolume, reinterpret_cast<void*>(rn), ADD_CONVEX_VOLUME_Key,
			BitMask32::all_on());
	Raycaster::GetSingletonPtr()->setHitCallback(REMOVE_CONVEX_VOLUME_Idx,
			removeConvexVolume, reinterpret_cast<void*>(rn), REMOVE_CONVEX_VOLUME_Key,
			BitMask32::all_on());
	//set convex volume set area type callbacks
	setAreaTypeCallback("a");
}

void App::setOffMeshConnectionTool()
{
	std::cout << "Set Off Mesh Connections ..." << std::endl;
	//set off mesh connection tool
	rn->setOffMeshConnectionTool(renderDebug);
	//set convex volume add/remove callbacks
	Raycaster::GetSingletonPtr()->setHitCallback(ADD_OFF_MESH_CONNECTION_Idx,
			addOffMeshConnection, reinterpret_cast<void*>(rn), ADD_OFF_MESH_CONNECTION_Key,
			BitMask32::all_on());
	Raycaster::GetSingletonPtr()->setHitCallback(REMOVE_OFF_MESH_CONNECTION_Idx,
			removeOffMeshConnection, reinterpret_cast<void*>(rn), REMOVE_OFF_MESH_CONNECTION_Key,
			BitMask32::all_on());
}

void App::doFinalWork()
{
	std::cout << "... Do Final Work!" << std::endl;
	//build navigation mesh
	rn->buildNavMesh();

	//set callbacks
	switch (sampleType)
	{
	case TILE:
	{
		//build first tile around agent pos
		float pos[3];
		LVecBase3fToRecast(agentPos, pos);
		dynamic_cast<Sample_TileMesh*>(rn->getSample())->buildTile(pos);
		//set buildTile/removeTile callbacks
		Raycaster::GetSingletonPtr()->setHitCallback(BUILD_TILE_Idx, buildTile,
				reinterpret_cast<void*>(rn), BUILD_TILE_Key,
				BitMask32::all_on());
		Raycaster::GetSingletonPtr()->setHitCallback(REMOVE_TILE_Idx,
				removeTile, reinterpret_cast<void*>(rn), REMOVE_TILE_Key,
				BitMask32::all_on());
	}
		break;
	case OBSTACLE:
	{
		//load an obstacle node path
		obstacleNP = window->load_model(window->get_render(), obstacleName);
		obstacleTable.clear();
		//set addObstacle/removeObstacle callbacks
		Raycaster::GetSingletonPtr()->setHitCallback(ADD_OBSTACLE_Idx,
				addObstacle, reinterpret_cast<void*>(rn), ADD_OBSTACLE_Key,
				BitMask32::all_on());
		Raycaster::GetSingletonPtr()->setHitCallback(REMOVE_OBSTACLE_Idx,
				removeObstacle, reinterpret_cast<void*>(rn),
				REMOVE_OBSTACLE_Key, BitMask32::all_on());
	}
		break;
	case SOLO:
	default:
		break;
	}

#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getSample()->handleRender(*(rn->getApp()->dd));
	rn->getSample()->getInputGeom()->drawConvexVolumes(rn->getApp()->dd);
	rn->getSample()->getInputGeom()->drawOffMeshConnections(rn->getApp()->dd, true);
#endif

	//set ai update task
	switch (movType)
	{
#ifndef WITHCHARACTER
	case RECAST:
	case KINEMATIC:
	case RIGID:
		task = new GenericAsyncTask("ai update", &RN::ai_update,
				reinterpret_cast<void*>(rn));
	break;
#else
	case CHARACTER:
		task = new GenericAsyncTask("ai update", &RN::ai_updateCHARACTER,
				reinterpret_cast<void*>(rn));
		break;
#endif
	default:
		break;
	}
	task->set_sort(AI_TASK_SORT);
	panda->get_task_mgr().add(task);

	character.show();
	///Crowd tool
	//set crowd tool
	rn->setCrowdTool();
	//set area costs ...
	rn->setCrowdAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	rn->setCrowdAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	rn->setCrowdAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	rn->setCrowdAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	rn->setCrowdAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	rn->setCrowdAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);
	//... and flags
	rn->setCrowdIncludeFlag((SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED));
	rn->setCrowdExcludeFlag(SAMPLE_POLYFLAGS_DISABLED);

	//add agent
//	float maxError = rn->getSample()->getConfig().detailSampleMaxError;
	float maxError = characterHeight;
	int agentIdx = rn->addCrowdAgent(movType, character, agentPos,
			characterMaxSpeed, &rn_anim_collection, cs, mBulletWorld.p(), maxError,
			characterRadius, characterHeight, allOnButZeroMask);
	//add a crowd raycaster
//	Raycaster::GetSingletonPtr()->setHitCallback(0, allOffButZeroMask,
//			reinterpret_cast<void*>(rn), "shift-mouse1", BitMask32::all_on());
//	Raycaster::GetSingletonPtr()->setHitCallback(0, allOnButZeroMask,
//			reinterpret_cast<void*>(rn), "shift-mouse1", BitMask32::all_on());
	//crowd set target
	Raycaster::GetSingletonPtr()->setHitCallback(SET_CROWD_TARGET_Idx,
			setCrowdTarget, reinterpret_cast<void*>(rn), SET_CROWD_TARGET_Key,
			BitMask32::all_on());
	//crowd set velocity
	Raycaster::GetSingletonPtr()->setHitCallback(SET_CROWD_VELOCITY_Idx,
			setCrowdVelocity, reinterpret_cast<void*>(rn), SET_CROWD_VELOCITY_Key,
			BitMask32::all_on());

	//Switch doors
	Raycaster::GetSingletonPtr()->setHitCallback(SET_SWITCH_DOOR_Idx,
			switchDoor, reinterpret_cast<void*>(rn), SET_SWITCH_DOOR_Key,
			BitMask32::all_on());

#ifdef TESTANOMALIES
	AsyncTask::DoneStatus print_data(GenericAsyncTask* task, void* data);
	AgentData *agentData;
	//print_data post ai
	agentData = new AgentData;
	agentData->msg = std::string("POST AI: ");
	agentData->agent = rn->getCrowdAgent(agentIdx);
	agentData->member = AgentData::VEL;
	task = new GenericAsyncTask("POST AI", &print_data,
			reinterpret_cast<void*>(agentData));
	task->set_sort(AI_TASK_SORT + 1);
	panda->get_task_mgr().add(task);
	//print_data pre physics
	agentData = new AgentData;
	agentData->msg = std::string("PRE PHYSICS: ");
	agentData->agent = rn->getCrowdAgent(agentIdx);
	agentData->member = AgentData::POS;
	task = new GenericAsyncTask("PRE PHYSICS", &print_data,
			reinterpret_cast<void*>(agentData));
	task->set_sort(PHYSICS_TASK_SORT - 1);
	panda->get_task_mgr().add(task);
	//print_data post physics
	agentData = new AgentData;
	agentData->msg = std::string("POST PHYSICS: ");
	agentData->agent = rn->getCrowdAgent(agentIdx);
	agentData->member = AgentData::POS;
	task = new GenericAsyncTask("POST PHYSICS", &print_data,
			reinterpret_cast<void*>(agentData));
	task->set_sort(PHYSICS_TASK_SORT + 1);
	panda->get_task_mgr().add(task);
	//
	task = new GenericAsyncTask("DELIMITER", &print_data,
			reinterpret_cast<void*>(NULL));
	task->set_sort(PHYSICS_TASK_SORT + 2);
	panda->get_task_mgr().add(task);
#endif
	//remove hooks
	panda->get_event_handler().remove_hooks_with(
			reinterpret_cast<void*>(myDataAreaType.p()));
	panda->get_event_handler().remove_hooks_with(
			reinterpret_cast<void*>(myDataContinue.p()));
}

void setCrowdTarget(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	rn->setCrowdTarget(raycaster->getHitPos());
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
}

void setCrowdVelocity(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	rn->setCrowdVelocity(raycaster->getHitPos());
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
}

void buildTile(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	dynamic_cast<Sample_TileMesh*>(rn->getSample())->buildTile(m_hitPos);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getSample()->handleRender(*(rn->getApp()->dd));
	rn->getSample()->getInputGeom()->drawConvexVolumes(rn->getApp()->dd);
	rn->getSample()->getInputGeom()->drawOffMeshConnections(rn->getApp()->dd, true);
#endif
}

void removeTile(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	dynamic_cast<Sample_TileMesh*>(rn->getSample())->removeTile(m_hitPos);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getSample()->handleRender(*(rn->getApp()->dd));
	rn->getSample()->getInputGeom()->drawConvexVolumes(rn->getApp()->dd);
	rn->getSample()->getInputGeom()->drawOffMeshConnections(rn->getApp()->dd, true);
#endif
}

void addObstacle(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	TempObstacle* tempObstacle = new TempObstacle(obstacleNP, 0.5,
			rn->getBulletWorld());
	tempObstacle->m_nodePath.reparent_to(rn->getRender());
	tempObstacle->m_nodePath.set_pos(raycaster->getHitPos());
	//add to obstacle table
	obstacleTable[tempObstacle->m_nodePath] = tempObstacle;
	//add detour obstacle
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	Sample_TempObstacles* sample =
			dynamic_cast<Sample_TempObstacles*>(rn->getSample());
	dtTileCache* tileCache = sample->getTileCache();
	tileCache->addObstacle(m_hitPos, tempObstacle->m_radius,
			tempObstacle->m_heigth, &tempObstacle->m_ref);
	tempObstacle->m_tileCache = tileCache;
	//update tile cache
	tileCache->update(0, sample->getNavMesh());
	//
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getSample()->handleRender(*(rn->getApp()->dd));
	rn->getSample()->getInputGeom()->drawConvexVolumes(rn->getApp()->dd);
	rn->getSample()->getInputGeom()->drawOffMeshConnections(rn->getApp()->dd, true);
#endif
}

void removeObstacle(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	NodePath hitNP = NodePath(const_cast<PandaNode*>(raycaster->getHitNode()));
	if (hitNP.get_name() == "TempObstacle")
	{
		//delete TempObstacle
		delete obstacleTable[hitNP];
		//remove from obstacle table
		obstacleTable.erase(hitNP);
		Sample_TempObstacles* sample =
				dynamic_cast<Sample_TempObstacles*>(rn->getSample());
		//update tile cache
		sample->getTileCache()->update(0, sample->getNavMesh());

#ifdef DEBUG_DRAW
		rn->getApp()->dd->reset();
		rn->getSample()->handleRender(*(rn->getApp()->dd));
		rn->getSample()->getInputGeom()->drawConvexVolumes(rn->getApp()->dd);
		rn->getSample()->getInputGeom()->drawOffMeshConnections(rn->getApp()->dd, true);
#endif
	}
	//
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
}

static int pointInPoly(int nvert, const float* verts, const float* p)
{
	int i, j, c = 0;
	for (i = 0, j = nvert-1; i < nvert; j = i++)
	{
		const float* vi = &verts[i*3];
		const float* vj = &verts[j*3];
		if (((vi[2] > p[2]) != (vj[2] > p[2])) &&
			(p[0] < (vj[0]-vi[0]) * (p[2]-vi[2]) / (vj[2]-vi[2]) + vi[0]) )
			c = !c;
	}
	return c;
}

static void reverseVector(float* verts, const int nverts)
{
	float temp[3];
	for (int i = 0; i < nverts/2; i++)
	{
		dtVcopy(temp, &verts[i*3]);
		dtVcopy(&verts[i*3], &verts[(nverts - i - 1)*3]);
		dtVcopy(&verts[(nverts - i - 1)*3], temp);
	}
}

void switchDoor(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	//get hit point
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	//check if a convex volume was hit (see: Delete case of ConvexVolumeTool::handleClick)
	int m_convexVolumeID = -1;
	const ConvexVolume* vols =
			rn->getSample()->getInputGeom()->getConvexVolumes();
	for (int i = 0; i < rn->getSample()->getInputGeom()->getConvexVolumeCount();
			++i)
	{
		if (pointInPoly(vols[i].nverts, vols[i].verts, m_hitPos) &&
				m_hitPos[1] >= vols[i].hmin && m_hitPos[1] <= vols[i].hmax)
		{
			m_convexVolumeID = i;
			break;
		}
	}
	// Check if the end point is close enough into a convex volume.
	if (m_convexVolumeID != -1)
	{
		///https://groups.google.com/forum/?fromgroups#!searchin/recastnavigation/door/recastnavigation/K2C44OCpxGE/a2Zn6nu0dIIJ
		const float *m_queryPolyPtr =
				rn->getSample()->getInputGeom()->getConvexVolumes()[m_convexVolumeID].verts;
		int nverts =
				rn->getSample()->getInputGeom()->getConvexVolumes()[m_convexVolumeID].nverts;

		float *m_queryPoly = new float[nverts * 3];

		for (int i = 0; i < nverts * 3; ++i)
		{
			m_queryPoly[i] = m_queryPolyPtr[i];
		}

		reverseVector(m_queryPoly, nverts);

		float m_centerPos[3];
		m_centerPos[0] = m_centerPos[1] = m_centerPos[2] = 0;
		for (int i = 0; i < nverts; ++i)
		{
			dtVadd(m_centerPos, m_centerPos, &m_queryPoly[i * 3]);
		}
		dtVscale(m_centerPos, m_centerPos, 1.0f / nverts);

		dtQueryFilter m_filter;
		m_filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL);
		m_filter.setExcludeFlags(0);
		dtPolyRef m_startRef;
		float m_polyPickExt[3] = { 2, 4, 2 };
		dtStatus status;
		status = rn->getSample()->getNavMeshQuery()->findNearestPoly(
				m_centerPos, m_polyPickExt, &m_filter, &m_startRef, 0);
		if (!dtStatusSucceed(status))
			return;

		static const int MAX_POLYS = 256;
		dtPolyRef m_polys[MAX_POLYS];
		dtPolyRef m_parent[MAX_POLYS];
		int m_npolys;

		rn->getSample()->getNavMeshQuery()->findPolysAroundShape(m_startRef,
				m_queryPoly, nverts, &m_filter, m_polys, m_parent, 0, &m_npolys,
				MAX_POLYS);

		for (int i = 0; i < m_npolys; ++i)
		{
			if (rn->getSample()->getNavMeshQuery()->isValidPolyRef(m_polys[i],
					&m_filter))
			{
				unsigned char area = 1;
				unsigned short flags;

				if (dtStatusSucceed(
						rn->getSample()->getNavMesh()->getPolyArea(m_polys[i],
								&area)))
				{
					//check if area is a door
					if (area == SAMPLE_POLYAREA_DOOR)
					{
						if (dtStatusSucceed(
								rn->getSample()->getNavMesh()->getPolyFlags(
										m_polys[i], &flags)))
						{
							flags ^= SAMPLE_POLYFLAGS_DISABLED;
							rn->getSample()->getNavMesh()->setPolyFlags(
									m_polys[i], flags);
						}
					}
				}
			}
		}
		delete[] m_queryPoly;
	}
	//
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getApp()->dd->reset();
	rn->getSample()->handleRender(*(rn->getApp()->dd));
#endif
}

#ifdef TESTANOMALIES
AsyncTask::DoneStatus print_data(GenericAsyncTask* task, void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	AgentData* agentData = (AgentData*)data;
	if (agentData)
	{
		switch (agentData->member)
		{
			case AgentData::POS:
			std::cout << agentData->msg << " POS " << agentData->agent->getPos() << std::endl;
			break;
			case AgentData::VEL:
			std::cout << agentData->msg << " VEL " << agentData->agent->getVel() << std::endl;
			break;
			default:
			break;
		}
	}
	else
	{
		std::cout << std::endl;
	}
	//
	return AsyncTask::DS_cont;
}
#endif

