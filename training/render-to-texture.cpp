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
 * \file /Ely/training/manual_example.cpp
 *
 * \date 10/mar/2013 (15:39:11)
 * \author consultit
 */

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <animControlCollection.h>
#include <auto_bind.h>
#include <cardMaker.h>
#include <textureStage.h>
#include <texturePool.h>
#include <orthographicLens.h>
#include <nodePathCollection.h>

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");
PandaFramework framework;
WindowFramework *window;
NodePath loadAnims(PandaFramework& framework, WindowFramework* window,
		const std::string& actorName, std::vector<std::string>& animNames,
		AnimControlCollection& animCollection);

void renderToTexure(const Event * event, void * data);
PT(GraphicsOutput)mybuffer;
NodePath newRend;
//
NodePath actor, mirror;
AnimControlCollection anims;
int changeActor = 0;
float xDim = 20, yDim = 20;

int render_to_texture_main(int argc, char *argv[])
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
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	window = framework.open_window();
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

	mybuffer.clear();

	//mirror
	CardMaker mirrorCard("mirror");
	mirrorCard.set_frame(-xDim / 2.0, xDim / 2.0, -yDim / 2.0, yDim / 2.0);
	mirrorCard.set_uv_range(LTexCoord(0.0, 0.0), LTexCoord(1.0, 0.0),
			LTexCoord(1.0, 1.0), LTexCoord(0.0, 1.0));
	mirror = NodePath(mirrorCard.generate());
	PT(TextureStage)texStage0 = new TextureStage("texStage0");
	//tex0
	PT(Texture)tex0 = TexturePool::load_texture(
			Filename(baseDir + "data/textures/terrain.png"));
	mirror.set_tex_scale(texStage0, 1.0, 1.0);
	mirror.set_texture(texStage0, tex0, 0);
	//
	mirror.set_scale(1.0);
	mirror.set_hpr(0, 0, 0);
	mirror.set_pos(0, 0, 0);
	mirror.reparent_to(window->get_render());

	//set event callback
	EventHandler::get_global_event_handler()->add_hook("g", &renderToTexure,
			static_cast<void*>(NULL));

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

NodePath loadAnims(PandaFramework& framework, WindowFramework* window,
		const std::string& actorName, std::vector<std::string>& animNames,
		AnimControlCollection& animCollection)
{
	NodePath actor = window->load_model(framework.get_models(), actorName);
	//load animations
	for (unsigned int i = 0; i < animNames.size(); ++i)
	{
		window->load_model(actor, animNames[i]);
	}
	if (animNames.size())
	{
		//bind animations
		auto_bind(actor.node(), animCollection,
				PartGroup::HMF_ok_wrong_root_name | PartGroup::HMF_ok_part_extra
						| PartGroup::HMF_ok_anim_extra);
	}
	return actor;
}

void renderToTexure(const Event * event, void * data)
{
	if (mybuffer)
	{
		//remove all child
		NodePathCollection children = newRend.get_children();
		for (int i = 0; i < children.get_num_paths(); ++i)
		{
			if (children[i].node()->is_of_type(Camera::get_class_type()))
			{
				continue;
			}
			children[i].remove_node();
		}
		mybuffer->set_one_shot(true);
	}
	//initialization once
	else
	{
		NodePath mycameraNP;
		newRend = NodePath("newRend");
		mycameraNP = NodePath(new Camera("my camera"));
		mycameraNP.reparent_to(newRend);
		//tex1: rendered texture
		mybuffer = window->get_graphics_output()->make_texture_buffer(
				"My Buffer", 512, 512);
		mybuffer->set_one_shot(true);
		PT(DisplayRegion)region = mybuffer->make_display_region();
		region->set_clear_color_active(true);
		region->set_clear_color(LColorf(1, 1, 1, 1));
		DCAST(Camera, mycameraNP.node())->set_lens(new OrthographicLens());
		DCAST(Camera, mycameraNP.node())->get_lens()->set_film_size(6.0, 6.0);
		DCAST(Camera, mycameraNP.node())->get_lens()->set_near_far(-1000.0,
				1000.0);
		region->set_camera(mycameraNP);
		mycameraNP.set_pos(0, 0, 2.5);
//		mycameraNP.set_hpr(0, -90, 0);
		mycameraNP.set_hpr(0, 0, 0);
		//tex1
		PT(TextureStage)texStage1 = new TextureStage("texStage1");
		//	texStage1->set_mode(TextureStage::M_blend);
		//	texStage1->set_mode(TextureStage::M_replace);
		//	texStage1->set_mode(TextureStage::M_add);
		texStage1->set_mode(TextureStage::M_modulate);
		//	tex1->set_wrap_u(Texture::WM_repeat);
		//	tex1->set_wrap_v(Texture::WM_repeat);
		mirror.set_texture(texStage1, mybuffer->get_texture(), 1);
	}

	float xCentMap, yCentMap, xDimMap, yDimMap;
	changeActor = changeActor % 2;
	std::string model, anim;
	switch (changeActor)
	{
	case 0:
		model = "eve.bam";
		anim = "eve-walk.bam";
		xDimMap = 4;
		yDimMap = 4;
		xCentMap = 3;
		yCentMap = 18;
		break;
	case 1:
		model = "panda.bam";
		anim = "panda-walk.bam";
		xDimMap = 8;
		yDimMap = 8;
		xCentMap = 13;
		yCentMap = 9;
		break;
	default:
		break;
	}
	++changeActor;

	//actors
	anims.clear_anims();
	std::vector<std::string> animNames;
	animNames.push_back(anim);
	actor = loadAnims(framework, window, model, animNames, anims);
	actor.set_scale(1.0);
	actor.set_pos(0, 0, 0);
	anims.get_anim(0)->loop(true);
	//
	LPoint3f minP, maxP;
	actor.calc_tight_bounds(minP, maxP);
	LVecBase3f dims = maxP - minP;
	float filmSize = max(dims.get_x(), dims.get_z()) + 0.2;
	DCAST(Camera, mybuffer->get_active_display_region(0)->get_camera().node())->get_lens(
			0)->set_film_size(filmSize, filmSize);
	//texturing
	actor.reparent_to(newRend);
	//tex1 transform
	float uOff, vOff;
	float uScale, vScale;
	uScale = xDim / xDimMap;
	vScale = yDim / yDimMap;
	uOff = xCentMap / xDim * uScale - 0.5;
	vOff = yCentMap / yDim * vScale - 0.5;
	PT(TextureStage)texStage1 = mirror.find_texture_stage("texStage1");
	mirror.set_tex_scale(texStage1, uScale, vScale);
	mirror.set_tex_offset(texStage1, -uOff, -vOff);
	mirror.set_tex_rotate(texStage1, 0);
}
