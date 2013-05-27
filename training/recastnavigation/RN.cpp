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
		DCAST(BulletCharacterControllerNode, m_pandaNP.node())->set_angular_movement(deltaAngle);
//		LPoint3f lookAtPos = RecastToLVecBase3f(p) - m_pandaNP.get_pos() - m_vel * 100000;
//		m_pandaNP.heads_up(lookAtPos);
		//get current vel
		LVector3f currentVel = (m_pandaNP.get_pos() - m_oldPos) / dt;
		m_anims->get_anim(0)->set_play_rate(currentVel.length() / rateFactor);
		m_oldPos = m_pandaNP.get_pos();
//		m_anims->get_anim(0)->set_play_rate(m_vel.length() / rateFactor);
		if (not m_anims->get_anim(0)->is_playing())
		{
			m_anims->get_anim(0)->loop(false);
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
		float scale)
{
	bool result = true;
	m_geom = new InputGeom;
	m_meshName = meshName;
	if (not m_geom->loadMesh(m_ctx, (path + meshName).c_str(), scale))
	{
		delete m_geom;
		m_geom = NULL;
		m_ctx->dumpLog("Geom load log %s:", meshName.c_str());
		result = false;
	}
	return result;
}

void RN::createGeomMesh(Sample* currentSample, SAMPLETYPE sampleType)
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
		LVecBase3fToRecast((*iter)->getPos(), crowd->getAgent((*iter)->getIdx())->npos);
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
	return AsyncTask::DS_again;
}
#endif

void RN::setConvexVolumeTool(NodePath renderDebug)
{
	//set ConvexVolumeTool
	m_convexVolumeTool = new ConvexVolumeTool(renderDebug);
	m_currentSample->setTool(m_convexVolumeTool);
}

void RN::setCrowdTool()
{
	//set CrowdTool
	m_crowdTool = new CrowdTool;
	m_currentSample->setTool(m_crowdTool);
}

int RN::addCrowdAgent(MOVTYPE movType, NodePath pandaNP, LPoint3f pos,
		float agentMaxSpeed, AnimControlCollection* anims, BulletConstraint* cs,
		BulletWorld* world,
		float maxError, float radius, float height)
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
	Agent* agent = new Agent(agentIdx, movType, pandaNP, anims,
			cs, world, maxError, radius);
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

void RN::setCrowdTarget(LPoint3f pos)
{
	float p[3];
	LVecBase3fToRecast(pos, p);
	m_crowdTool->getState()->setMoveTarget(p, false);
}

///
///Obstacles data
std::string obstacleName("box.egg");
NodePath obstacleNP;
std::map<NodePath, TempObstacle*> obstacleTable;

//CALLBACKS
const int CALLBACKSNUM = 5;
//
//void addConvexVolume(Raycaster* raycaster, void* data);
const int ADD_CONVEX_VOLUME_Idx = 0;
std::string ADD_CONVEX_VOLUME_Key("shift-mouse3");
//void removeConvexVolume(Raycaster* raycaster, void* data);
const int REMOVE_CONVEX_VOLUME_Idx = 1;
std::string REMOVE_CONVEX_VOLUME_Key("shift-alt-mouse3");
//
//void continueCallback(const Event* event, void* data);
//
//void setCrowdTarget(Raycaster* raycaster, void* data);
const int SET_CROWD_TARGET_Idx = 0;
std::string SET_CROWD_TARGET_Key("shift-mouse1");
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
	rn->getConvexVolumeTool()->handleRender();
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
	rn->getConvexVolumeTool()->handleRender();
#endif
}

void App::setContinueCallback(const std::string& event)
{
	myData = new EventCallbackInterface<App>::
	  			EventCallbackData(this, &App::continueCallback);
	panda->define_key(event, "continue app",
			  		&EventCallbackInterface<App>::eventCallbackFunction,
			  			reinterpret_cast<void*>(myData.p()));
}

void App::continueCallback(const Event* event)
{
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
	rn->getSample()->handleRender();
#endif

	//set ai update task
	AsyncTask* task;
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

	///Crowd tool
	//set crowd tool
	rn->setCrowdTool();
	//add agent
//	float maxError = rn->getSample()->getConfig().detailSampleMaxError;
	float maxError = characterHeight;
	int agentIdx = rn->addCrowdAgent(movType, character, agentPos,
			agentMaxSpeed, &rn_anim_collection, cs, mBulletWorld.p(), maxError,
			characterRadius, characterHeight);
	//add a crowd raycaster
//	Raycaster::GetSingletonPtr()->setHitCallback(0, allOffButZeroMask,
//			reinterpret_cast<void*>(rn), "shift-mouse1", BitMask32::all_on());
//	Raycaster::GetSingletonPtr()->setHitCallback(0, allOnButZeroMask,
//			reinterpret_cast<void*>(rn), "shift-mouse1", BitMask32::all_on());
	//crowd re-target
	Raycaster::GetSingletonPtr()->setHitCallback(SET_CROWD_TARGET_Idx,
			setCrowdTarget, reinterpret_cast<void*>(rn), SET_CROWD_TARGET_Key,
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
	//remove hook
	panda->get_event_handler().remove_hooks_with(
			reinterpret_cast<void*>(myData.p()));
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
	rn->getSample()->handleRender();
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
	rn->getSample()->handleRender();
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
	rn->getSample()->handleRender();
#endif
}

void removeObstacle(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	NodePath hitNP = NodePath(const_cast<PandaNode*>(raycaster->getHitNode()));
	if(hitNP.get_name() == "TempObstacle")
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
		rn->getSample()->handleRender();
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

