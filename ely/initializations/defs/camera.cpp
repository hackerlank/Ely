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
 * \file /Ely/ely/initializations/callback_defs.cpp
 *
 * \date 05/set/2012 (20:07:54)
 * \author consultit
 */

#include "../common_configs.h"
#include "ControlComponents/Driver.h"
#include "ControlComponents/Chaser.h"
#include "Support/Picker.h"

///shared between camera, picker, actor1
bool controlGrabbed = false;

///camera related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION camera_initialization;

#ifdef __cplusplus
}
#endif

namespace
{
void toggleCameraControl(const Event* event, void* data)
{
	SMARTPTR(Object)camera = reinterpret_cast<Object*>(data);
	SMARTPTR(Component) compControl = camera->getComponent(
			ComponentFamilyType("Control"));
	///<DEFAULT CAMERA CONTROL>
//	WindowFramework* gameWindow = GameManager::GetSingletonPtr()->windowFramework();
	///</DEFAULT CAMERA CONTROL>
	if (compControl->is_of_type(Driver::get_class_type()))
	{
		SMARTPTR(Driver)cameraControl = DCAST(Driver, camera->getComponent(
						ComponentFamilyType("Control")));
		if (cameraControl->isEnabled())
		{
			//enabled: then disable it
			//disable
			cameraControl->disable();

			///<DEFAULT CAMERA CONTROL>
//			//reset trackball transform
//			LMatrix4 cameraMat = gameWindow->get_camera_group().get_transform()->get_mat();
//			cameraMat.invert_in_place();
//			SMARTPTR(Trackball)trackBall =
//			DCAST(Trackball, gameWindow->get_mouse().find("**/+Trackball").node());
//			trackBall->set_mat(cameraMat);
//			//(re)enable trackball
//			GameManager::GetSingletonPtr()->enable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//
			controlGrabbed = false;
		}
		else if (not controlGrabbed)
		{
			//disabled: then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			GameManager::GetSingletonPtr()->disable_mouse();
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
//			LMatrix4 cameraMat = gameWindow->get_camera_group().get_transform()->get_mat();
//			cameraMat.invert_in_place();
//			SMARTPTR(Trackball)trackBall =
//			DCAST(Trackball, gameWindow->get_mouse().find("**/+Trackball").node());
//			trackBall->set_mat(cameraMat);
//			//(re)enable trackball
//			GameManager::GetSingletonPtr()->enable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//
			controlGrabbed = false;
		}
		else if (not controlGrabbed)
		{
			//if disabled then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			GameManager::GetSingletonPtr()->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			cameraControl->enable();
			//
			controlGrabbed = true;
		}
	}
}
static void togglePicker(const Event* event, void* data)
{
	SMARTPTR(Object)camera = reinterpret_cast<Object*>(data);
	if (Picker::GetSingletonPtr())
	{
		//picker on: remove
		delete Picker::GetSingletonPtr();
		//
		controlGrabbed = false;
	}
	else if (not controlGrabbed)
	{
		//picker off: add
		new Picker(camera->objectTmpl()->pandaFramework(),
				camera->objectTmpl()->windowFramework(),
				"shift-mouse1", "mouse1-up");
		//
		controlGrabbed = true;
	}
}
}
void camera_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//camera
	//enable/disable camera control by event
	pandaFramework->define_key("c", "toggleCameraControl", &toggleCameraControl,
			static_cast<void*>(object));
	//enable/disable a picker
	pandaFramework->define_key("x", "togglePicker", &togglePicker,
			static_cast<void*>(object));

}

void cameraInit()
{
}

void cameraEnd()
{
}


