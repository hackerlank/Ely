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
#include <orthographicLens.h>

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");

NodePath loadActorAndAnims(PandaFramework& framework, WindowFramework* window,
		const std::string& actorName, std::vector<std::string>& animNames,
		AnimControlCollection& animCollection);

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
	PT(GraphicsOutput)mybuffer = window->get_graphics_output()->make_texture_buffer("My Buffer",
			512, 512);
	mybuffer->set_one_shot(true);
	PT(Texture)mytexture = mybuffer->get_texture();
	PT(DisplayRegion)region = mybuffer->make_display_region();
	PT(Camera)mycamera = new Camera("my camera");
	PT(Lens)lens = new OrthographicLens();
	lens->set_film_size(6.0, 6.0);
	lens->set_near_far(-1000.0, 1000.0);
	mycamera->set_lens(lens);
	NodePath mycameraNP = NodePath(mycamera);
	region->set_camera(mycameraNP);
	mycameraNP.set_pos(0, 0, 2.5);
	NodePath newRend("newRend");
	mycameraNP.reparent_to(newRend);

	//actor
	AnimControlCollection anims;
	std::vector<std::string> animNames;
	animNames.push_back("eve-walk.bam");
	NodePath ely = loadActorAndAnims(framework, window, "eve.bam", animNames,
			anims);
	ely.set_scale(1.0);
	ely.set_pos(0, 0, 0);
	anims.get_anim(0)->loop(true);
	//texturing
	ely.reparent_to(newRend);

	//mirror
	CardMaker mirrorCard("mirror");
	mirrorCard.set_frame(-10.0, 10.0, -10.0, 10.0);
	mirrorCard.set_uv_range(LTexCoord(0.0, 0.0), LTexCoord(1.0, 0.0),
			LTexCoord(1.0, 1.0), LTexCoord(0.0, 1.0));
	NodePath mirror = NodePath(mirrorCard.generate());
	PT(TextureStage)textureStage0 = new TextureStage("mirror_TextureStage0");
	mirror.set_tex_scale(textureStage0, 2.0, 2.0);
	mytexture->set_wrap_u(Texture::WM_repeat);
	mytexture->set_wrap_v(Texture::WM_repeat);
	mirror.set_texture(textureStage0, mytexture, 1);
	mirror.set_scale(1.0);
	mirror.set_hpr(0, 0, 0);
	mirror.set_pos(0, 0, 0);
	mirror.reparent_to(window->get_render());

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

NodePath loadActorAndAnims(PandaFramework& framework, WindowFramework* window,
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
