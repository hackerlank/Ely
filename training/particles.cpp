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
#include "particles/ParticleEffect.h"
#include "particles/Particles.h"
#include "particles/SpriteParticleRendererExt.h"
#include "particles/ForceGroup.h"
#include <linearJitterForce.h>
#include <lineEmitter.h>
#include "particles/GameParticlesManager.h"

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/elygame/");

ParticleSystemManager* particle_sys_mgr;
PhysicsManager* physics_mgr;
void loadConfig(ely::ParticleEffect& particleEffect);
bool fireStarted;
void toggleFire(const Event* e, void* data);
void shoot(const Event* e, void* data);

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
	PT(Trackball)trackball = DCAST(Trackball, tballnp.node());
	trackball->set_pos(0, 200, 0);
	trackball->set_hpr(0, 15, 0);

	///here is room for your own code
	//setup particles managers
	ely::GameParticlesManager* gameParticlesMgr = new ely::GameParticlesManager(
			10);
	// Base Particle Renderer NodePath
	NodePath fireBPR = window->get_render().attach_new_node("fireBPR");
	// particle effect
	ely::ParticleEffect fire = ely::ParticleEffect();
	loadConfig(fire);
	fire.set_pos(0.0, 40.0, 5.0);
	fire.start(window->get_render(), fireBPR);
	fireStarted = true;
	// soft start/stop
	EventHandler::get_global_event_handler()->add_hook("x", &toggleFire,
			reinterpret_cast<void*>(&fire));
	// induce labor: force particles' birth
	EventHandler::get_global_event_handler()->add_hook("p", &shoot,
			reinterpret_cast<void*>(&fire));

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	//delete gameParticlesMgr
	delete gameParticlesMgr;
	//
	return (0);
}

void loadConfig(ely::ParticleEffect& particleEffect)
{
	particleEffect.reset();
	particleEffect.set_pos(0.000, 0.000, 0.000);
	particleEffect.set_hpr(0.000, 0.000, 0.000);
	particleEffect.set_scale(1.000, 1.000, 1.000);
	SMARTPTR(ely::Particles)p0 = new ely::Particles("particles-1");
	// Particles parameters
	p0->setFactory("PointParticleFactory");
	p0->setRenderer("SpriteParticleRenderer");
	p0->setEmitter("LineEmitter");
	p0->set_pool_size(1024);
	p0->set_birth_rate(86400.0000);
	p0->set_litter_size(256);
	p0->set_litter_spread(0);
	p0->set_system_lifespan(0.0000);
	p0->set_local_velocity_flag(1);
	p0->set_system_grows_older_flag(0);
	// Factory parameters
	SMARTPTR(PointParticleFactory)p0_getFactory = dynamic_cast<PointParticleFactory*>(p0->getFactory().p());
	p0_getFactory->set_lifespan_base(0.6000);
	p0_getFactory->set_lifespan_spread(0.0000);
	p0_getFactory->set_mass_base(1.0000);
	p0_getFactory->set_mass_spread(0.0000);
	p0_getFactory->set_terminal_velocity_base(400.0000);
	p0_getFactory->set_terminal_velocity_spread(0.0000);
	// Point factory parameters
	// Renderer parameters
	SMARTPTR(ely::SpriteParticleRendererExt)p0_getRenderer = dynamic_cast<ely::SpriteParticleRendererExt*>(p0->getRenderer().p());
	p0_getRenderer->set_alpha_mode(BaseParticleRenderer::PR_ALPHA_OUT);
	p0_getRenderer->set_user_alpha(1.00);
	// Sprite parameters
	p0_getRenderer->addTextureFromFile(
			baseDir + std::string("/data/textures/panda.jpg"));
	p0_getRenderer->set_color(LVector4f(1.00, 1.00, 1.00, 1.00));
	p0_getRenderer->set_x_scale_flag(1);
	p0_getRenderer->set_y_scale_flag(1);
	p0_getRenderer->set_anim_angle_flag(0);
	p0_getRenderer->set_initial_x_scale(0.25);
	p0_getRenderer->set_final_x_scale(1.25);
	p0_getRenderer->set_initial_y_scale(0.25);
	p0_getRenderer->set_final_y_scale(1.25);
	p0_getRenderer->set_nonanimated_theta(0.0000);
	p0_getRenderer->set_alpha_blend_method(
			BaseParticleRenderer::PP_BLEND_LINEAR);
	p0_getRenderer->set_alpha_disable(0);
	// Emitter parameters
	SMARTPTR(LineEmitter)p0_getEmitter = dynamic_cast<LineEmitter*>(p0->getEmitter().p());
	p0_getEmitter->set_emission_type(BaseParticleEmitter::ET_CUSTOM);
	p0_getEmitter->set_amplitude(0.0000);
	p0_getEmitter->set_amplitude_spread(0.0000);
	p0_getEmitter->set_offset_force(LVector3f(0.0000, 0.0000, 0.0000));
	p0_getEmitter->set_explicit_launch_vector(
			LVector3f(1.0000, 0.0000, 0.0000));
	p0_getEmitter->set_radiate_origin(LPoint3f(0.0000, 0.0000, 0.0000));
	// Line parameters
	p0_getEmitter->set_endpoint1(LPoint3f(-40.0000, 10.0000, 0.0000));
	p0_getEmitter->set_endpoint2(LPoint3f(40.0000, 10.0000, 0.0000));
	particleEffect.addParticles(p0);
	SMARTPTR(ely::ForceGroup)f0 = new ely::ForceGroup("noise1");
	// Force parameters
	SMARTPTR(LinearJitterForce)force0 = new LinearJitterForce(0.6000, 0);
	force0->set_vector_masks(1, 1, 1);
	force0->set_active(1);
	f0->addForce(force0.p());
	particleEffect.addForceGroup(f0);
}

void toggleFire(const Event* e, void* data)
{
	ely::ParticleEffect* fire = reinterpret_cast<ely::ParticleEffect*>(data);
	if (fireStarted)
	{
		fire->softStop();
		fireStarted = false;
	}
	else
	{
		fire->softStart();
		fireStarted = true;
	}
}

void shoot(const Event* e, void* data)
{
	ely::ParticleEffect* fire = reinterpret_cast<ely::ParticleEffect*>(data);

	std::list<SMARTPTR(ely::Particles)> l = fire->getParticlesList();
	for (std::list<SMARTPTR(ely::Particles)>::iterator it=l.begin(); it!=l.end();++it)
	{
		(*it)->induce_labor();
	}
}
