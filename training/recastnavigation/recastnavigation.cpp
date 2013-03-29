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
#include <bulletWorld.h>
#include <bulletTriangleMesh.h>
#include <bulletTriangleMeshShape.h>
#include <bulletSphericalConstraint.h>
#include <bulletClosestHitRayResult.h>
#include <bulletRigidBodyNode.h>
#include <mouseWatcher.h>

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

//Picker
class Raycaster: public Singleton<Raycaster>
{
public:
	Raycaster(PandaFramework* app, WindowFramework* window, SMARTPTR(BulletWorld)world,
	const std::string& pickKeyOn, const std::string& pickKeyOff);
	virtual ~Raycaster();

	void setHitCallback(void (*callback)(Raycaster*, void*), void* data)
	{
		mCallback = callback;
		mData = data;
	}
	std::string getHitNode(){return mHitNode;}
	LPoint3f getHitPos(){return mHitPos;}
	LPoint3f getFromPos(){return mFromPos;}
	LPoint3f getToPos(){return mToPos;}
	LVector3f getHitNormal(){return mHitNormal;}
	float getHitFraction(){return mHitFraction;}

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();

private:
	///Panda framework.
	PandaFramework* mApp;
	///Window framework.
	WindowFramework* mWindow;
	///Render, camera node paths.
	NodePath mRender, mCamera;
	///Camera lens reference.
	SMARTPTR(Lens) mCamLens;
	///Bullet world.
	SMARTPTR(BulletWorld) mWorld;
	///Hit results.
	std::string mHitNode;
	LPoint3f mHitPos, mFromPos, mToPos;
	LVector3f mHitNormal;
	float mHitFraction;
	///Hit callback.
	void (*mCallback)(Raycaster*, void*);
	void* mData;
	/**
	 * \name Hit body event callback data.
	 */
	///@{
	SMARTPTR(EventCallbackInterface<Raycaster>::EventCallbackData) mPickBodyData;
	void hitBody(const Event* event);
	std::string mPickKeyOn, mPickKeyOff;
	///@}
	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};
Raycaster::Raycaster(PandaFramework* app, WindowFramework* window,
		SMARTPTR(BulletWorld)world,
		const std::string& pickKeyOn, const std::string& pickKeyOff) :
		mApp(app), mWindow(window), mWorld(world),  mCallback(NULL)
{
	//get render, camera node paths
	mRender = window->get_render();
	mCamera = window->get_camera_group();
	mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
	// setup event callback for picking body
	mPickKeyOn = pickKeyOn;
	mPickKeyOff = pickKeyOff;
	mPickBodyData = new EventCallbackInterface<Raycaster>::EventCallbackData(this,
			&Raycaster::hitBody);
	mApp->define_key(mPickKeyOn, "pickBody",
			&EventCallbackInterface<Raycaster>::eventCallbackFunction,
			reinterpret_cast<void*>(mPickBodyData.p()));
	mApp->define_key(mPickKeyOff, "pickBodyUp",
			&EventCallbackInterface<Raycaster>::eventCallbackFunction,
			reinterpret_cast<void*>(mPickBodyData.p()));
}

Raycaster::~Raycaster()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mApp)
	{
		// remove event callback for picking body
		mApp->get_event_handler().remove_hooks_with(
				reinterpret_cast<void*>(mPickBodyData.p()));
	}
}

void Raycaster::hitBody(const Event* event)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	// handle body picking
	if (mCallback and event->get_name() == mPickKeyOn)
	{
		//get the mouse watcher
		SMARTPTR(MouseWatcher)mwatcher =
		DCAST(MouseWatcher, mWindow->get_mouse().node());
		if (mwatcher->has_mouse())
		{
			// Get to and from pos in camera coordinates
			LPoint2f pMouse = mwatcher->get_mouse();
			//
			LPoint3f pFrom, pTo;
			if (mCamLens->extrude(pMouse, pFrom, pTo))
			{
				//Transform to global coordinates
				pFrom = mRender.get_relative_point(mCamera, pFrom);
				pTo = mRender.get_relative_point(mCamera, pTo);
				//cast a ray to detect a body
				BulletClosestHitRayResult result = mWorld->ray_test_closest(pFrom, pTo,
						BitMask32::all_on());
				//
				if (result.has_hit())
				{
					//possible hit objects:
					//- BulletRigidBodyNode
					//- BulletCharacterControllerNode
					//- BulletVehicle
					//- BulletConstraint
					//- BulletSoftBodyNode
					//- BulletGhostNode

					mHitNode = result.get_node()->get_name();
					mHitPos = result.get_hit_pos();
					mHitNormal = result.get_hit_normal();
					mHitFraction = result.get_hit_fraction();
					mFromPos = result.get_from_pos();
					mToPos = result.get_to_pos();
					mCallback(this, mData);
				}
			}
		}
	}
}

ReMutex& Raycaster::getMutex()
{
	return mMutex;
}

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection rn_anim_collection;
AsyncTask::DoneStatus rn_check_playing(GenericAsyncTask* task, void* data);
AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);

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
LPoint3f targetPos(-18.1919556, 0.9224129, -2.37020111);

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
	AnimControlCollection* m_anims;
public:
	Agent(int agentIdx, NodePath pandaNP, AnimControlCollection* anims) :
			m_agentIdx(agentIdx), m_pandaNP(pandaNP), m_anims(anims)
	{
	}
	int getIdx()
	{
		return m_agentIdx;
	}
	void updatePosDir(const float* p, const float* v);
};

void Agent::updatePosDir(const float* p, const float* v)
{
	LVecBase3f vel = recastToLVecBase3f(v);
	if (vel.length_squared() > 0.1)
	{
		m_pandaNP.set_pos(recastToLVecBase3f(p));
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

class RN
{
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
	void addAgent(NodePath pandaNP, LPoint3f pos, float agentSpeed,
			AnimControlCollection* anims = NULL);
	void setTarget(LPoint3f pos);
	static AsyncTask::DoneStatus ai_update(GenericAsyncTask* task, void* data);
};

RN::RN(NodePath render) :
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

void RN::addAgent(NodePath pandaNP, LPoint3f pos, float agentSpeed,
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

void reTarget(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	rn->setTarget(raycaster->getHitPos());
	std::cout <<
			"| panda node: " << raycaster->getHitNode() <<
			"| hit pos: " << raycaster->getHitPos() <<
			"| hit normal: " << raycaster->getHitNormal() <<
			"| hit fraction: " << raycaster->getHitFraction() <<
			"| from pos: " << raycaster->getFromPos() <<
			"| to pos: " << raycaster->getToPos() << std::endl;
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
	//physics
	SMARTPTR(BulletWorld)mBulletWorld = new BulletWorld();
	mBulletWorld->set_gravity(0.0, 0.0, -9.81);

	//set camera pos
	window->get_camera_group().set_pos(60, -60, 50);
	window->get_camera_group().look_at(0, 0, 0);

	//Load world mesh
	NodePath worldMesh = window->load_model(window->get_render(),
			rnDir + meshNameEgg);
	worldMesh.set_pos(0.0, 0.0, 0.0);
	//attach bullet body
	//see: https://www.panda3d.org/forums/viewtopic.php?t=13981
	BulletTriangleMesh* triMesh = new BulletTriangleMesh();
	//add geoms from geomNodes to the mesh
	NodePathCollection geomNodes = worldMesh.find_all_matches("**/+GeomNode");
	for (int i = 0; i < geomNodes.get_num_paths(); ++i)
	{
		SMARTPTR(GeomNode)geomNode = DCAST(GeomNode,
				geomNodes.get_path(i).node());
		CSMARTPTR(TransformState) ts = geomNode->get_transform();
		GeomNode::Geoms geoms = geomNode->get_geoms();
		for (int j = 0; j < geoms.get_num_geoms(); ++j)
		{
			triMesh->add_geom(geoms.get_geom(j), true, ts.p());
		}
	}
	SMARTPTR(BulletShape)collisionShape =
	new BulletTriangleMeshShape(triMesh, false);
	collisionShape->set_local_scale(worldMesh.get_scale());
	SMARTPTR(BulletRigidBodyNode)mRigidBodyNode =
	new BulletRigidBodyNode((worldMesh.get_name()+"_physics").c_str());
	mRigidBodyNode->add_shape(collisionShape);
	mRigidBodyNode->set_mass(0.0);
	mRigidBodyNode->set_kinematic(false);
	mRigidBodyNode->set_static(true);
	mRigidBodyNode->set_deactivation_enabled(true);
	mRigidBodyNode->set_active(false);
	mBulletWorld->attach(mRigidBodyNode);
	NodePath mRigidBodyNodePath = NodePath(mRigidBodyNode);
	mRigidBodyNodePath.set_collide_mask(BitMask32::all_on());
	//attach to scene
	worldMesh.reparent_to(mRigidBodyNodePath);
	mRigidBodyNodePath.reparent_to(window->get_render());

	//physics debug
	NodePath mBulletDebugNodePath = NodePath(new BulletDebugNode("Debug"));
	mBulletDebugNodePath.reparent_to(window->get_render());
	SMARTPTR(BulletDebugNode)bulletDebugNode =
	DCAST(BulletDebugNode,mBulletDebugNodePath.node());
	mBulletWorld->set_debug_node(bulletDebugNode);
	bulletDebugNode->show_wireframe(true);
	bulletDebugNode->show_constraints(true);
	bulletDebugNode->show_bounding_boxes(false);
	bulletDebugNode->show_normals(false);
	mBulletDebugNodePath.hide();

	//physics: advance the simulation state
	AsyncTask* task = new GenericAsyncTask("update physics", &update_physics,
			reinterpret_cast<void*>(mBulletWorld.p()));
	panda.get_task_mgr().add(task);

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
//	pbundle->set_anim_blend_flag(true);
//	pbundle->set_control_effect(rn_anim_collection.get_anim(0), 0.5);
//	pbundle->set_control_effect(rn_anim_collection.get_anim(1), 0.5);
//	int actualAnim = 0;
	//switch among animations
//	task = new GenericAsyncTask("recastnavigation playing",
//			&rn_check_playing, reinterpret_cast<void*>(&actualAnim));
//	task->set_delay(3);
//	panda.get_task_mgr().add(task);
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
	rn.addAgent(Actor, agentPos, agentMaxSpeed, &rn_anim_collection);
	//set target
	rn.setTarget(targetPos);
	//set ai update task
	task = new GenericAsyncTask("ai update", &RN::ai_update,
			reinterpret_cast<void*>(&rn));
//	task->set_delay(5);
	panda.get_task_mgr().add(task);

	//add a raycaster
	new Raycaster(&panda, window, mBulletWorld, "shift-mouse1", "mouse1-up");
	//re-target
	Raycaster::GetSingletonPtr()->setHitCallback(reTarget,
			reinterpret_cast<void*>(&rn));

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
//	delete objectTmplMgr;
//	delete componentTmplMgr;
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

AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	BulletWorld* mBulletWorld = reinterpret_cast<BulletWorld*>(data);

	float dt = ClockObject::get_global_clock()->get_dt();

	int maxSubSteps;

	// do physics step simulation
	// timeStep < maxSubSteps * fixedTimeStep (=1/60.0=0.016666667) -->
	// supposing a minimum of 6,666666667 fps, we have a maximum
	// timeStep of 0.15 secs so: maxSubSteps <= 60 * 0.15 = 9
	if (dt < 0.016666667)
	{
		maxSubSteps = 1;
	}
	else if (dt < 0.033333333)
	{
		maxSubSteps = 2;
	}
	else if (dt < 0.05)
	{
		maxSubSteps = 3;
	}
	else if (dt < 0.066666668)
	{
		maxSubSteps = 4;
	}
	else if (dt < 0.083333335)
	{
		maxSubSteps = 5;
	}
	else if (dt < 0.100000002)
	{
		maxSubSteps = 6;
	}
	else if (dt < 0.116666669)
	{
		maxSubSteps = 7;
	}
	else if (dt < 0.133333336)
	{
		maxSubSteps = 8;
	}
	else
	{
		maxSubSteps = 9;
	}
	mBulletWorld->do_physics(dt, maxSubSteps);
	//
	return AsyncTask::DS_cont;
}
