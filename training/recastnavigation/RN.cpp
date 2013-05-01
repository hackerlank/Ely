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
		//get actual vel
		LVector3f actualVel = (m_pandaNP.get_pos() - m_oldPos) / dt;
		m_anims->get_anim(0)->set_play_rate(actualVel.length() / rateFactor);
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
		//get actual vel
		LVector3f actualVel = (m_pandaNP.get_pos() - m_oldPos) / dt;
		m_anims->get_anim(0)->set_play_rate(actualVel.length() / rateFactor);
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
