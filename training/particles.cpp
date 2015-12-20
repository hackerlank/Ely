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
 * \file /Ely/training/particles.cpp
 *
 * \date 2013-03-10 
 * \author consultit
 */

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <particleSystemManager.h>
#include <physicsManager.h>
#include <linearEulerIntegrator.h>
#include <pointParticleFactory.h>
#include <sphereVolumeEmitter.h>
#include <pointParticleRenderer.h>
#include <lvecBase4.h>

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/elygame/");

AsyncTask::DoneStatus update_managers(GenericAsyncTask* task, void* data);
ParticleSystemManager* particle_sys_mgr;
PhysicsManager* physics_mgr;

int particles_main(int argc, char *argv[])
{
	// Load your configuration
	load_prc_file_data("", "model-path " + baseDir + "data/models");
	load_prc_file_data("", "model-path " + baseDir + "data/shaders");
	load_prc_file_data("", "model-path " + baseDir + "data/sounds");
	load_prc_file_data("", "model-path " + baseDir + "data/textures");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "lock-to-one-cpu 0");
	load_prc_file_data("", "support-threads 1");
	load_prc_file_data("", "audio-buffering-seconds 5");
	load_prc_file_data("", "audio-preload-threshold 2000000");
	load_prc_file_data("", "sync-video #t");
	//open a new window framework
	PandaFramework framework;
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	WindowFramework *window = framework.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}
	//setup camera trackball (local coordinate)
	NodePath tballnp = window->get_mouse().find("**/+Trackball");
	PT(Trackball) trackball = DCAST(Trackball, tballnp.node());
	trackball->set_pos(0, 200, 0);
	trackball->set_hpr(0, 15, 0);

	///here is room for your own code
	//setup particles update
	particle_sys_mgr = new ParticleSystemManager();
    particle_sys_mgr->set_frame_stepping(1);
    physics_mgr = new PhysicsManager();
    PT(LinearEulerIntegrator) integrator = new LinearEulerIntegrator();
    physics_mgr->attach_linear_integrator(integrator);
	//update managers
	AsyncTask* task = new GenericAsyncTask("update terrain", &update_managers,
	NULL);
	framework.get_task_mgr().add(task);

	//Factory
	PT(PointParticleFactory) pt_particle_factory = new PointParticleFactory();
	pt_particle_factory->set_lifespan_base(0.5);
	pt_particle_factory->set_lifespan_spread(0);
	pt_particle_factory->set_mass_base(1.0);
	pt_particle_factory->set_mass_spread(0);
	pt_particle_factory->set_terminal_velocity_base(400);
	pt_particle_factory->set_terminal_velocity_spread(0);

	//Emitter
	PT(SphereVolumeEmitter) sphere_emitter = new SphereVolumeEmitter;
	sphere_emitter->set_emission_type(SphereVolumeEmitter::ET_RADIATE);
	sphere_emitter->set_radius(3.0);
	// negative values emit the particles toward the sphere center
	sphere_emitter->set_amplitude(1);
	sphere_emitter->set_amplitude_spread(0);
	sphere_emitter->set_offset_force(LVector3f(0, 0, 0));
	sphere_emitter->set_explicit_launch_vector(LVector3f(1, 0, 0));
	sphere_emitter->set_radiate_origin(LPoint3f(0, 0, 0));

	//Renderer
	PT(PointParticleRenderer) pt_particle_rend = new PointParticleRenderer();
	pt_particle_rend->set_alpha_mode(BaseParticleRenderer::PR_ALPHA_OUT);
	pt_particle_rend->set_user_alpha(1);
	pt_particle_rend->set_point_size(2.0);
	pt_particle_rend->set_start_color(LColorf(1, 0, 0, 1)); // alpha value is ignored
	pt_particle_rend->set_end_color(LColorf(1, 1, 0, 1));
	pt_particle_rend->set_blend_type(PointParticleRenderer::PP_BLEND_LIFE);
	pt_particle_rend->set_blend_method(BaseParticleRenderer::PP_BLEND_LINEAR);
	//pt_particle_rend->set_color_blend_mode(ColorBlendAttrib::Mode::M_inv_subtract);
	//pt_particle_rend->set_ignore_scale(false);

	//ParticleSystem
	PT(ParticleSystem) particle_sys = new ParticleSystem();
	particle_sys->set_pool_size(4);
	particle_sys->set_birth_rate(0.1);
	particle_sys->set_litter_size(10);
	particle_sys->set_litter_spread(0);
	particle_sys->set_local_velocity_flag(true);
//	particle_sys->set_spawn_on_death_flag(true); // this caused an exception!!
	particle_sys->set_system_grows_older_flag(true);
	particle_sys->set_system_lifespan(3600.0);
	particle_sys->set_active_system_flag(true);
	// use it to advance system age, or start at some age
	//particle_sys->set_system_age(5.0);
	// system_age is updated only when set_system_grows_older_flag(true);
	// get_system_age() returns 0 unless system_grows_older_flag is set
	//The particle factory, emitter and renderer should be attached to the ParticleSystem
	particle_sys->set_factory(pt_particle_factory);
	particle_sys->set_renderer(pt_particle_rend);
	particle_sys->set_emitter(sphere_emitter);

	//particle effect
	NodePath particleEffect("particleEffect");
	particleEffect.set_pos(-20.0, 0.0, 0.0);
	//add physical node
	PT(PhysicalNode) physicalNode = new PhysicalNode("physicalNode");
	NodePath physicalNodeNP = NodePath(physicalNode);
	particle_sys->set_render_parent(physicalNode);
	physicalNode->add_physical(particle_sys);
	//add particle
	physicalNodeNP.reparent_to(particleEffect);

	//render parent node
	NodePath renderParentNode = window->get_render().attach_new_node("renderParent");
	//start
	particle_sys->set_render_parent(renderParentNode.node());
	//ParticleSystemManager
	particle_sys_mgr->attach_particlesystem(particle_sys);
	physics_mgr->attach_physical(particle_sys);
	//
	particleEffect.reparent_to(window->get_render());

	//render parent node
//	renderParentNode.set_pos(0.0, 0.0, 0.0);
//	particle_sys->set_render_parent(renderParentNode);
	//render parent node instance 1
//	NodePath instance1 = window->get_render().attach_new_node("instance1");
//	instance1.set_pos(-20.0, 100.0, 0.0);
//	renderParentNode.instance_to(instance1);
	//render parent node instance 2
//	NodePath instance2 = window->get_render().attach_new_node("instance2");
//	instance2.set_pos(20.0, 100.0, 0.0);
//	renderParentNode.instance_to(instance2);

	//spawn node
	// if spawn and render parents should be different
//	NodePath spawnNode = window->get_render().attach_new_node("spawnNode");
//	spawnNode.set_pos(0.0, 0.0, 0.0);
//	particle_sys->set_spawn_render_node_path(spawnNode);

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

AsyncTask::DoneStatus update_managers(GenericAsyncTask* task, void* data)
{

	//physics
	float dt = ClockObject::get_global_clock()->get_dt();

    particle_sys_mgr->do_particles(dt);
    physics_mgr->do_physics(dt);
	//
	return AsyncTask::DS_cont;
}
