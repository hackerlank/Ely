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
 * \file /Ely/initializations/callback_defs.cpp
 *
 * \date 05/set/2012 (20:07:54)
 * \author marco
 */

#include "../common_configs.h"
#include "Utilities/ComponentSuite.h"
#include "Utilities/Tools.h"

///common
static bool controlGrabbed = false;

///camera related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION camera_initialization;

#ifdef __cplusplus
}
#endif

static void toggleCameraControl(const Event* event, void* data)
{
	SMARTPTR(Object)camera = reinterpret_cast<Object*>(data);
	SMARTPTR(Component) compControl = camera->getComponent(
			ComponentFamilyType("Control"));
	if (compControl->is_of_type(Driver::get_class_type()))
	{
		SMARTPTR(Driver)cameraControl = DCAST(Driver, camera->getComponent(
						ComponentFamilyType("Control")));
		if (cameraControl->isEnabled())
		{
			//if enabled then disable it
			//disable
			cameraControl->disable();

			///<DEFAULT CAMERA CONTROL>
//			//reset trackball transform
//			LMatrix4 cameraMat = mWindow->get_camera_group().get_transform()->get_mat();
//			cameraMat.invert_in_place();
//			SMARTPTR(Trackball)trackBall =
//			DCAST(Trackball, gameManager->mTrackBall.node());
//			trackBall->set_mat(cameraMat);
//			//(re)enable trackball
//			gameManager->enable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//
			controlGrabbed = false;
		}
		else if (not controlGrabbed)
		{
			//if disabled then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			gameManager->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			cameraControl->enable();
			//
			controlGrabbed = true;
		}

	}
	else if (compControl->is_of_type(Chaser::get_class_type()))
	{
		SMARTPTR(Chaser)cameraControl = DCAST(Chaser, camera->getComponent(
						ComponentFamilyType("Control")));

		if (cameraControl->isEnabled())
		{
			//if enabled then disable it
			//disable
			cameraControl->disable();

			///<DEFAULT CAMERA CONTROL>
//			//reset trackball transform
//			LMatrix4 cameraMat = mWindow->get_camera_group().get_transform()->get_mat();
//			cameraMat.invert_in_place();
//			SMARTPTR(Trackball)trackBall =
//			DCAST(Trackball, gameManager->mTrackBall.node());
//			trackBall->set_mat(cameraMat);
//			//(re)enable trackball
//			gameManager->enable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//
			controlGrabbed = false;
		}
		else if (not controlGrabbed)
		{
			//if disabled then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			gameManager->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			cameraControl->enable();
			//
			controlGrabbed = true;
		}
	}
}
void camera_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//camera
		object->getNodePath().look_at(50, 200, 10);
	//enable/disable camera control by event
		pandaFramework->define_key("c", "enableCameraControl", &toggleCameraControl,
				static_cast<void*>(object));
	}

void cameraInit()
{
}

void cameraEnd()
{
}

///Actor1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Actor1_initialization;

#ifdef __cplusplus
}
#endif

static void toggleActor1Control(const Event* event, void* data)
{
	SMARTPTR(Object)actor1 = reinterpret_cast<Object*>(data);
	SMARTPTR(Driver)actor1Control = DCAST(Driver, actor1->getComponent(
					ComponentFamilyType("Control")));

	if (actor1Control->isEnabled())
	{
		//if enabled then disable it
		//disable
		actor1Control->disable();

		///<DEFAULT CAMERA CONTROL>
//		//reset trackball transform
//		LMatrix4 cameraMat = mWindow->get_camera_group().get_transform()->get_mat();
//		cameraMat.invert_in_place();
//		SMARTPTR(Trackball)trackBall =
//		DCAST(Trackball, gameManager->mTrackBall.node());
//		trackBall->set_mat(cameraMat);
//		//(re)enable trackball
//		gameManager->enable_mouse();
		///</DEFAULT CAMERA CONTROL>

		//
		controlGrabbed = false;
	}
	else if (not controlGrabbed)
	{
		//if disabled then enable it

		///<DEFAULT CAMERA CONTROL>
//		//disable the trackball
//		gameManager->disable_mouse();
		///</DEFAULT CAMERA CONTROL>

		//enable
		actor1Control->enable();
		//
		controlGrabbed = true;
	}
}
void Actor1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Actor1
	//play animation
//	SMARTPTR(Model) actor1Model = DCAST(Model, object->getComponent(
//					ComponentFamilyType("Scene")));
//	actor1Model->animations().loop("panda-walk", false);
		//play sound
		SMARTPTR(Sound3d) actor1Sound3d = DCAST(Sound3d, object->getComponent(
						ComponentFamilyType("Audio")));
		actor1Sound3d->getSound("audio/sfx/GUI_rollover.wav")->set_loop(true);
		actor1Sound3d->getSound("audio/sfx/GUI_rollover.wav")->play();

		//enable/disable Actor1 control by event
		pandaFramework->define_key("v", "enableActor1Control", &toggleActor1Control,
				static_cast<void*>(object));
	}

void Actor1Init()
{
}
void Actor1End()
{
}

///Plane1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Plane1_initialization;

#ifdef __cplusplus
}
#endif

///Plane1 related
void Plane1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Plane1
		SMARTPTR(Model) plane1Model = DCAST(Model, object->getComponent(
						ComponentFamilyType("Scene")));
		SMARTPTR(TextureStage) planeTS0 = new TextureStage("planeTS0");
		SMARTPTR(Texture) planeTex = TexturePool::load_texture("maps/envir-ground.jpg");
		plane1Model->getNodePath().set_texture(planeTS0, planeTex, 1);
		plane1Model->getNodePath().set_tex_scale(planeTS0, 100, 100);
	}

void Plane1Init()
{
}
void Plane1End()
{
}

///Terrain1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Terrain1_initialization;

#ifdef __cplusplus
}
#endif

///Terrain1 related
void Terrain1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Terrain1
//	object->getNodePath().set_render_mode_wireframe(1);
	}

void Terrain1Init()
{
}
void Terrain1End()
{
}

///NPC1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION NPC1_initialization;

#ifdef __cplusplus
}
#endif

void NPC1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//NPC1
		fsm& npc1FSM = (fsm&) (*DCAST(Activity, object->getComponent(
								ComponentFamilyType("Behavior"))));
		npc1FSM.request("idle");
	//play sound
		SMARTPTR(Sound3d) npc1Sound3d = DCAST(Sound3d, object->getComponent(
						ComponentFamilyType("Audio")));
		npc1Sound3d->getSound("models/audio/sfx/GUI_click.wav")->set_loop(true);
		npc1Sound3d->getSound("models/audio/sfx/GUI_click.wav")->play();
	}

void NPC1Init()
{
}
void NPC1End()
{
}

///Seeker1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Seeker1_initialization;

#ifdef __cplusplus
}
#endif

static bool aiEnabled = false;
static void toggleSeeker1Control(const Event* event, void* data)
{
	SMARTPTR(Object) seeker1 = reinterpret_cast<Object*>(data);
	SMARTPTR(Steering)actor1AI = DCAST(Steering, seeker1->getComponent(
					ComponentFamilyType("AI")));

	if (actor1AI->isEnabled())
	{
		//if enabled then disable it
		actor1AI->disable();
		//
		aiEnabled = false;
	}
	else if (not aiEnabled)
	{
		//if disabled then enable it
		actor1AI->enable();
		//
		aiEnabled = true;
	}
}
void Seeker1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Seeker1
	//enable/disable Seeker1 control by event
	pandaFramework->define_key("b", "enableSeeker1Control", &toggleSeeker1Control,
			static_cast<void*>(object));
}

void Seeker1Init()
{
}
void Seeker1End()
{
}
