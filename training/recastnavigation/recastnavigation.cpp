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
 * \file /Ely/training/recastnavigation.cpp
 *
 * \date 14/mar/2013 10:05:13
 * \author marco
 */

#include "Utilities/Tools.h"
#include <iostream>
#include <string>
#include <vector>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <character.h>
#include <animControlCollection.h>
#include <pandaFramework.h>

//rn
#include <cstring>
#include <cmath>
#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourNavMeshBuilder.h>
#include <DetourCommon.h>
#include "InputGeom.h"
#include "Sample_SoloMesh.h"
#include "CrowdTool.h"

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection rn_anim_collection;
AsyncTask::DoneStatus rn_check_playing(GenericAsyncTask* task, void* data);

//RN
std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/");
std::string rnDir(
		"/REPOSITORY/KProjects/WORKSPACE/recastnavigation/RecastDemo/Bin/Meshes/");
//convert obj to egg: obj2egg -TR 90,0,0 nav_test.obj -o nav_test_panda.egg
//triangulate nav_test_panda.egg and...
std::string meshNameEgg("nav_test_panda.egg");
//...(re)convert egg to obj:
//egg2obj -cs y-up -o nav_test_panda.obj nav_test_panda.egg
std::string meshNameObj("nav_test_panda.obj");

LPoint3f agentPos(20.2317238, 9.31323242, -2.36828613);
float agentMaxSpeed = 1.5;
LPoint3f targetPos(-15.1919556, 0.9224129, -2.37020111);

//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/z$20axis/recastnavigation/fMqEAqSBOBk/zwOzHmjRsj0J
inline void LVecBase3fToRecast(const LVecBase3f& v, float* p)
{
	p[0] = v.get_x();
	p[1] = v.get_z();
	p[2] = -v.get_y();
}
inline LVecBase3f recastToLVecBase3f(const float* p)
{
	return LVecBase3f(p[0], -p[2], p[1]);
}

class Agent
{
	int m_agentIdx;
	NodePath m_pandaNP;
	NodePath m_render;
public:
	Agent(int agentIdx, NodePath pandaNP, NodePath render) :
			m_agentIdx(agentIdx), m_pandaNP(pandaNP), m_render(render)
	{
	}
	int getIdx()
	{
		return m_agentIdx;
	}
	void updatePosDir(const float* p, const float* d);
};

void Agent::updatePosDir(const float* p, const float* d)
{
	m_pandaNP.set_pos(recastToLVecBase3f(p));
//	LVector3f dir = m_pandaNP.get_relative_vector(m_render,
//			recastToLVecBase3f(d));
//	LOrientationf orient(dir, 0.0);
//	m_pandaNP.set_quat(m_pandaNP, orient);
}

class RN
{
	NodePath m_render;
	std::string m_meshName;
	InputGeom* m_geom;
	BuildContext* m_ctx;

	Sample_SoloMesh* m_sampleSolo;

	CrowdTool* m_crowdTool;

	std::list<Agent*> m_agents;

public:
	RN(NodePath render);
	~RN();
	CrowdTool* getCrowdTool()
	{
		return m_crowdTool;
	}
	//
	bool loadMesh(const std::string& path, const std::string& meshName);
	void createSoloMeshCrowdSample();
	bool buildNavMesh();
	void addAgent(NodePath pandaNP, LPoint3f pos, float agentSpeed);
	void setTarget(LPoint3f pos);
	static AsyncTask::DoneStatus ai_update(GenericAsyncTask* task, void* data);
};

RN::RN(NodePath render) :
		m_render(render), m_geom(0)
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

void RN::createSoloMeshCrowdSample()
{
	//create sample
	m_sampleSolo = new Sample_SoloMesh();
	//set rcContext
	m_sampleSolo->setContext(m_ctx);
	//handle Mesh Changed
	m_sampleSolo->handleMeshChanged(m_geom);
	//set CrowdTool
	m_crowdTool = new CrowdTool;
	m_sampleSolo->setTool(m_crowdTool);
}

bool RN::buildNavMesh()
{
	m_ctx->resetLog();
	//build navigation mesh
	bool result = m_sampleSolo->handleBuild();
	m_ctx->dumpLog("Build log %s:", m_meshName.c_str());
	return result;
}

void RN::addAgent(NodePath pandaNP, LPoint3f pos, float agentSpeed)
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
	Agent* agent = new Agent(agentIdx, pandaNP, m_render);
	m_agents.push_back(agent);
}

void RN::setTarget(LPoint3f pos)
{
	float p[3];
	LVecBase3fToRecast(pos, p);
	m_crowdTool->getState()->setMoveTarget(p, false);
}

AsyncTask::DoneStatus RN::ai_update(GenericAsyncTask* task, void* data)
{
	float dt = ClockObject::get_global_clock()->get_dt();

	RN* thisInst = reinterpret_cast<RN*>(data);

	thisInst->m_sampleSolo->handleUpdate(dt);

	dtCrowd* crowd = thisInst->m_crowdTool->getState()->getCrowd();
	std::list<Agent*>::iterator iter;
	for (iter = thisInst->m_agents.begin(); iter != thisInst->m_agents.end();
			++iter)
	{
		const float* pos = crowd->getAgent((*iter)->getIdx())->npos;
		const float* vel = crowd->getAgent((*iter)->getIdx())->vel;
		(*iter)->updatePosDir(pos, vel);
	}
	return AsyncTask::DS_again;
}

int main(int argc, char **argv)
{
	///setup
	// Load your configuration

	load_prc_file_data("", "model-path" + baseDir + "data/models");
	load_prc_file_data("", "model-path" + baseDir + "data/shaders");
	load_prc_file_data("", "model-path" + baseDir + "data/sounds");
	load_prc_file_data("", "model-path" + baseDir + "data/textures");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "lock-to-one-cpu 0");
	load_prc_file_data("", "support-threads 1");
	load_prc_file_data("", "audio-buffering-seconds 5");
	load_prc_file_data("", "audio-preload-threshold 2000000");
	load_prc_file_data("", "sync-video #t");

	PandaFramework panda = PandaFramework();
	panda.open_framework(argc, argv);
	panda.set_window_title("recastnavigation training");
	WindowFramework* window = panda.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}

	//set camera pos
	window->get_camera_group().set_pos(60, -60, 50);
	window->get_camera_group().look_at(0, 0, 0);

	//Load world mesh
	NodePath worldMesh = window->load_model(window->get_render(),
			rnDir + meshNameEgg);
	//attach to scene
	worldMesh.reparent_to(window->get_render());
	worldMesh.set_pos(0.0, 0.0, 0.0);

	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			baseDir + "data/models/eve.bam");
	SMARTPTR(Character)character =
	DCAST(Character, Actor.find("**/+Character").node());
	SMARTPTR(PartBundle)pbundle = character->get_bundle(0);
	//Load Animations
	std::vector<std::string> animations;
	animations.push_back(std::string(baseDir + "data/models/eve-run.bam"));
	animations.push_back(std::string(baseDir + "data/models/eve-walk.bam"));
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, animations[i]);
	}
	auto_bind(Actor.node(), rn_anim_collection);
	pbundle->set_anim_blend_flag(true);
	pbundle->set_control_effect(rn_anim_collection.get_anim(0), 0.5);
	pbundle->set_control_effect(rn_anim_collection.get_anim(1), 0.5);
	int actualAnim = 0;
	//switch among animations
	AsyncTask* task = new GenericAsyncTask("recastnavigation playing",
			&rn_check_playing, reinterpret_cast<void*>(&actualAnim));
	task->set_delay(3);
	panda.get_task_mgr().add(task);
	//attach to scene
	Actor.reparent_to(window->get_render());
	Actor.set_scale(0.3);

	//RN
	RN rn(window->get_render());
	//load mesh
	rn.loadMesh(rnDir, meshNameObj);
	//create solo mesh crowd sample
	rn.createSoloMeshCrowdSample();
	//build navigation mesh
	rn.buildNavMesh();
	//add agent
	rn.addAgent(Actor, agentPos, agentMaxSpeed);
	//set target
	rn.setTarget(targetPos);
	//set ai update task
	task = new GenericAsyncTask("ai update", &RN::ai_update,
			reinterpret_cast<void*>(&rn));
//	task->set_delay(5);
	panda.get_task_mgr().add(task);

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus rn_check_playing(GenericAsyncTask* task, void* data)
{
	//Control the Animations
	double time = ClockObject::get_global_clock()->get_real_time();
	int *actualAnim = reinterpret_cast<int*>(data);
	int num = *actualAnim % 3;
	if (num == 0)
	{
		std::cout << time << " - Blending" << std::endl;
		if (not rn_anim_collection.get_anim(0)->is_playing())
		{
			rn_anim_collection.get_anim(0)->play();
		}
		if (not rn_anim_collection.get_anim(1)->is_playing())
		{
			rn_anim_collection.get_anim(1)->play();
		}
	}
	else if (num == 1)
	{
		std::cout << time << " - Playing: "
				<< rn_anim_collection.get_anim_name(0) << std::endl;
		if (not rn_anim_collection.get_anim(0)->is_playing())
		{
			rn_anim_collection.get_anim(0)->play();
		}
		if (rn_anim_collection.get_anim(1)->is_playing())
		{
			rn_anim_collection.get_anim(1)->stop();
		}
	}
	else
	{
		std::cout << time << " - Playing: "
				<< rn_anim_collection.get_anim_name(1) << std::endl;
		rn_anim_collection.get_anim(1)->play();
		if (rn_anim_collection.get_anim(0)->is_playing())
		{
			rn_anim_collection.get_anim(0)->stop();
		}
		if (not rn_anim_collection.get_anim(1)->is_playing())
		{
			rn_anim_collection.get_anim(1)->play();
		}
	}
	*actualAnim += 1;
	return AsyncTask::DS_again;
}
