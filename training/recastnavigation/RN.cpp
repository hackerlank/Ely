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
 * \author marco
 */

#include "RN.h"

#ifdef NO_CHARACTER
void Agent::updatePosDir(const float* p, const float* v)
{
	LVecBase3f vel = RecastToLVecBase3f(v);
	if (vel.length_squared() > 0.1)
	{
		m_pandaNP.set_pos(RecastToLVecBase3f(p));
		LPoint3f lookAtPos = m_pandaNP.get_pos() - vel * 100000;
		m_pandaNP.heads_up(lookAtPos);
		if (not m_anims->get_anim(1)->is_playing())
		{
			m_anims->get_anim(1)->loop(false);
		}
	}
	else
	{
		if (m_anims->get_anim(1)->is_playing())
		{
			m_anims->get_anim(1)->pose(0);
			m_anims->get_anim(1)->stop();
		}
	}
}
#else
void Agent::updateVel(const float* v)
{
	LVecBase3f vel = RecastToLVecBase3f(v);
	if (vel.length_squared() > 0.1)
	{
		DCAST(BulletCharacterControllerNode, m_pandaNP.node())->set_linear_movement(
				vel, false);
		LPoint3f lookAtPos = m_pandaNP.get_pos() - vel * 100000;
		m_pandaNP.heads_up(lookAtPos);
		if (not m_anims->get_anim(1)->is_playing())
		{
			m_anims->get_anim(1)->loop(false);
		}
	}
	else
	{
		DCAST(BulletCharacterControllerNode, m_pandaNP.node())->set_linear_movement(
				LVector3f::zero(), false);
		if (m_anims->get_anim(1)->is_playing())
		{
			m_anims->get_anim(1)->pose(0);
			m_anims->get_anim(1)->stop();
		}
	}
}
#endif

////////////////////////////////////////////////////////////////////

RN::RN() :
		m_geom(0)
{
	//Sample
	m_ctx = new BuildContext;
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

bool RN::loadMesh(const std::string& path, const std::string& meshName)
{
	bool result = true;
	m_geom = new InputGeom;
	m_meshName = meshName;
	if (not m_geom->loadMesh(m_ctx, (path + meshName).c_str()))
	{
		delete m_geom;
		m_geom = NULL;
		m_ctx->dumpLog("Geom load log %s:", meshName.c_str());
		result = false;
	}
	return result;
}

void RN::createSoloMesh()
{
	//create sample
	m_sampleSolo = new Sample_SoloMesh();
	//set rcContext
	m_sampleSolo->setContext(m_ctx);
	//handle Mesh Changed
	m_sampleSolo->handleMeshChanged(m_geom);
}

bool RN::buildNavMesh()
{
	m_ctx->resetLog();
	//build navigation mesh
	bool result = m_sampleSolo->handleBuild();
	m_ctx->dumpLog("Build log %s:", m_meshName.c_str());
	return result;
}

#ifdef NO_CHARACTER
AsyncTask::DoneStatus RN::ai_update(GenericAsyncTask* task, void* data)
{
	float dt = ClockObject::get_global_clock()->get_dt();

	RN* thisInst = reinterpret_cast<RN*>(data);

	std::list<Agent*>::iterator iter;
	dtCrowd* crowd = thisInst->m_crowdTool->getState()->getCrowd();

	//update crowd agents' pos/vel
	thisInst->m_sampleSolo->handleUpdate(dt);

	//post-update all agent positions
	for (iter = thisInst->m_agents.begin(); iter != thisInst->m_agents.end();
			++iter)
	{
		const float* vel = crowd->getAgent((*iter)->getIdx())->vel;
		const float* pos = crowd->getAgent((*iter)->getIdx())->npos;
		(*iter)->updatePosDir(pos, vel);
	}
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
	thisInst->m_sampleSolo->handleUpdate(dt);

	//post-update all agent positions
	for (iter = thisInst->m_agents.begin(); iter != thisInst->m_agents.end();
			++iter)
	{
		const float* vel = crowd->getAgent((*iter)->getIdx())->vel;
		(*iter)->updateVel(vel);
	}
	return AsyncTask::DS_again;
}
#endif

void RN::setCrowdTool()
{
	//set CrowdTool
	m_crowdTool = new CrowdTool;
	m_sampleSolo->setTool(m_crowdTool);
}

void RN::addCrowdAgent(NodePath pandaNP, LPoint3f pos, float agentMaxSpeed,
		AnimControlCollection* anims)
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
	Agent* agent = new Agent(agentIdx, pandaNP, anims);
	//set Agent pos
	pandaNP.set_pos(pos);
	//add Agent to list
	m_agents.push_back(agent);
}

void RN::setSettings(const SampleSettings& settings)
{
	m_sampleSolo->setSampleSettings(settings);
}

SampleSettings RN::getSettings()
{
	return m_sampleSolo->getSampleSettings();
}

void RN::setCrowdTarget(LPoint3f pos)
{
	float p[3];
	LVecBase3fToRecast(pos, p);
	m_crowdTool->getState()->setMoveTarget(p, false);
}
