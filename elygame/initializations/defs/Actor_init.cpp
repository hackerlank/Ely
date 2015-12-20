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
 * \file /Ely/elygame/initializations/defs/Actor_init.cpp
 *
 * \date 2013-07-14 
 * \author consultit
 */

#include "../common_configs.h"
#include "ControlComponents/Driver.h"
#include "AudioComponents/Sound3d.h"

///shared between camera, picker, actor1
extern bool controlGrabbed;

///Actor1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Actor1_initialization;

#ifdef __cplusplus
}
#endif

namespace
{
void toggleActor1Control(const Event* event, void* data)
{
	SMARTPTR(Object)actor1= reinterpret_cast<Object*>(data);
	SMARTPTR(Driver)actor1Control = DCAST(Driver, actor1->getComponent(
					ComponentFamilyType("Control")));
	///<DEFAULT CAMERA CONTROL>
//	WindowFramework* gameWindow = GameManager::GetSingletonPtr()->windowFramework();
	///</DEFAULT CAMERA CONTROL>
	if (actor1Control->isEnabled())
	{
		//if enabled then disable it
		//disable
		actor1Control->disable();

		///<DEFAULT CAMERA CONTROL>
//		//reset trackball transform
//		LMatrix4 cameraMat = gameWindow->get_camera_group().get_transform()->get_mat();
//		cameraMat.invert_in_place();
//		SMARTPTR(Trackball)trackBall =
//		DCAST(Trackball, gameWindow->get_mouse().find("**/+Trackball").node());
//		trackBall->set_mat(cameraMat);
//		//(re)enable trackball
//		GameManager::GetSingletonPtr()->enable_mouse();
		///</DEFAULT CAMERA CONTROL>

		//
		controlGrabbed = false;
	}
	else if (not controlGrabbed)
	{
		//if disabled then enable it

		///<DEFAULT CAMERA CONTROL>
//		//disable the trackball
//		GameManager::GetSingletonPtr()->disable_mouse();
		///</DEFAULT CAMERA CONTROL>

		//enable
		actor1Control->enable();
		//
		controlGrabbed = true;
	}
}
}
void Actor1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Actor1
	//play animation
	//	SMARTPTR(Model) actor1Model = DCAST(Model, object->getComponent(
	//					ComponentFamilyType("Scene")));
	//	actor1Model->animations().loop("walk", false);
	//play sound
	SMARTPTR(Sound3d) actor1Sound3d = DCAST(Sound3d, object->getComponent(
			ComponentFamilyType("Audio")));
	actor1Sound3d->getSound("walk-sound")->set_loop(true);
	actor1Sound3d->getSound("walk-sound")->play();

	//enable/disable Actor1 control by event
	pandaFramework->define_key("v", "enableActor1Control", &toggleActor1Control,
	static_cast<void*>(object));
}

void Actor_initInit()
{
}

void Actor_initEnd()
{
}
