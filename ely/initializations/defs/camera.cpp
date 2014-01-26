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
#include "ObjectModel/ObjectTemplateManager.h"
#include "Support/Picker.h"

///camera related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION camera_initialization;

#ifdef __cplusplus
}
#endif

///shared between camera, picker, actor1
bool controlGrabbed = false;

namespace
{
//common text writing
NodePath textNode;
void writeText(const std::string& text, float scale,
		const LVecBase4& color, const LVector3f& location)
{
	textNode = NodePath(new TextNode("CommonTextNode"));
	textNode.reparent_to(ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject(ObjectId("render2d"))->getNodePath());
	textNode.set_bin("fixed", 50);
	textNode.set_depth_write(false);
	textNode.set_depth_test(false);
	textNode.set_billboard_point_eye();
	DCAST(TextNode, textNode.node())->set_text(text);
	textNode.set_scale(scale);
	textNode.set_color(color);
	textNode.set_pos(location);
}

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
			RETURN_ON_COND(cameraControl->disable() != Driver::Result::OK,)

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
			//remove text
			textNode.remove_node();
		}
		else if (not controlGrabbed)
		{
			//disabled: then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			GameManager::GetSingletonPtr()->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			RETURN_ON_COND(cameraControl->enable() != Driver::Result::OK,)
			//
			controlGrabbed = true;
			//write text
			writeText("Free View Camera", 0.05,
					LVecBase4(1.0, 1.0, 0.0, 1.0), LVector3f(-1.0, 0, -0.9));
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
			RETURN_ON_COND(cameraControl->disable() != Chaser::Result::OK,)

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
			//remove text
			textNode.remove_node();
		}
		else if (not controlGrabbed)
		{
			//if disabled then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			GameManager::GetSingletonPtr()->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			RETURN_ON_COND(cameraControl->enable() != Chaser::Result::OK,)
			//
			controlGrabbed = true;
			//write text
			writeText("Camera Chasing " + cameraControl->getChasedObject(), 0.05,
					LVecBase4(1.0, 1.0, 0.0, 1.0), LVector3f(-1.0, 0, -0.9));
		}
	}
}
void togglePicker(const Event* event, void* data)
{
	SMARTPTR(Object)camera = reinterpret_cast<Object*>(data);
	if (Picker::GetSingletonPtr())
	{
		//picker on: remove
		delete Picker::GetSingletonPtr();
		//
		controlGrabbed = false;
		//remove text
		textNode.remove_node();
	}
	else if (not controlGrabbed)
	{
		//picker off: add
		new Picker(camera->objectTmpl()->pandaFramework(),
				camera->objectTmpl()->windowFramework(),
				"shift-mouse1", "mouse1-up");
		//
		controlGrabbed = true;
		//write text
		writeText("Object Picker Active", 0.05,
				LVecBase4(1.0, 1.0, 0.0, 1.0), LVector3f(-1.0, 0, -0.9));
	}
}

struct ChasedObjectData
{
	float abs_max_distance;
	float abs_min_distance;
	float abs_max_height;
	float abs_min_height;
	ObjectId id;
};
std::vector<ChasedObjectData> *chasedDataPtr;
unsigned int idx = 0, chasedListSize;
void toggleChasedObject(const Event* event, void* data)
{
	SMARTPTR(Object)camera= reinterpret_cast<Object*>(data);
	SMARTPTR(Component) controlComp = camera->getComponent(
					ComponentFamilyType("Control"));
	if(controlComp->is_of_type(Chaser::get_class_type()))
	{
		SMARTPTR(Chaser) chaserComp = DCAST(Chaser, controlComp);

		unsigned int count = 0;
		do
		{
			++count;
			++idx;
			idx = idx % chasedListSize;
		}while ((not ObjectTemplateManager::GetSingletonPtr()->getCreatedObject((*chasedDataPtr)[idx].id))
				or (count > chasedListSize));
		//return if empty list
		RETURN_ON_COND(count > chasedListSize,)

		PRINT_DEBUG("Chased object: " << (*chasedDataPtr)[idx].id);
		HOLD_REMUTEX(chaserComp->getMutex())

		chaserComp->setChasedObject((*chasedDataPtr)[idx].id);
		chaserComp->setAbsMaxDistance((*chasedDataPtr)[idx].abs_max_distance);
		chaserComp->setAbsMinDistance((*chasedDataPtr)[idx].abs_min_distance);
		chaserComp->setAbsMaxHeight((*chasedDataPtr)[idx].abs_max_height);
		chaserComp->setAbsMinHeight((*chasedDataPtr)[idx].abs_min_height);
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
	//toggle chased object
	pandaFramework->define_key("z", "toggleChasedObject", &toggleChasedObject,
			static_cast<void*>(object));
}

void cameraInit()
{
	chasedDataPtr = new std::vector<ChasedObjectData>();
	chasedDataPtr->push_back({25.0,18.0,8.0,5.0,ObjectId("auto1")});
	chasedDataPtr->push_back({15.0,8.0,6.0,3.0,ObjectId("player0")});
	chasedListSize = chasedDataPtr->size();
}

void cameraEnd()
{
	delete chasedDataPtr;
}


