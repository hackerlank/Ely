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
#include "Utilities/Tools.h"

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/elygame/");

///Memory Pool
#include "memory_pool/MemoryPool.h"
#include "memory_pool/MemoryMacros.h"

///GameObject methods' declaration
class GameObject
{
	float mPos[3] __attribute__ ((__aligned__(16)));
	float mDir[3] __attribute__ ((__aligned__(16)));
public:
	GameObject()
	{
	}
	~GameObject()
	{
	}

	const float* getDir() const;
	void setDir(float* dir);

	const float* getPos() const;
	void setPos(float* pos);

GCC_MEMORYPOOL_DECLARATION(0)

}__attribute__ ((__packed__));

int memory_pool_main(int argc, char *argv[])
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
	const int NUM = 10;
	GameObject* go[NUM];
	for (int i = 0; i < NUM; ++i)
	{
		go[i] = new GameObject();
	}
	for (int i = 0; i < NUM; ++i)
	{
		delete go[i];
	}

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

///GameObject methods' definitions
GCC_MEMORYPOOL_DEFINITION(GameObject);
GCC_MEMORYPOOL_AUTOINIT(GameObject, 128);

const float* GameObject::getDir() const
{
	return mDir;
}

void GameObject::setDir(float* dir)
{
	this->mDir[0] = *dir;
	this->mDir[1] = *(dir + 1);
	this->mDir[2] = *(dir + 2);
}

const float* GameObject::getPos() const
{
	return mPos;
}

void GameObject::setPos(float* pos)
{
	this->mPos[0] = *pos;
	this->mPos[1] = *(pos + 1);
	this->mPos[2] = *(pos + 2);
}
