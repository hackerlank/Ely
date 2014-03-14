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
#include <Rocket/Core.h>

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

//locals
namespace
{
//common text writing
NodePath textNode;
void writeText(const std::string& text, float scale, const LVecBase4& color,
		const LVector3f& location)
{
	textNode = NodePath(new TextNode("CommonTextNode"));
	textNode.reparent_to(
			ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(
					ObjectId("render2d"))->getNodePath());
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
	if (compControl->componentType() == ComponentType("Driver"))
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
	else if (compControl->componentType() == ComponentType("Chaser"))
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
	if(controlComp->componentType() == ComponentType("Chaser"))
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
////////////////////////////////////////////////////////////////////////////////
///libRocket

//externs
extern Rocket::Core::Context *gRocketContext;
extern Rocket::Core::ElementDocument *gRocketMainMenu;
extern std::vector<void (*)(Rocket::Core::ElementDocument *)> gRocketAddElementsFunctions;
extern std::map<Rocket::Core::String,
		void (*)(const Rocket::Core::String&, Rocket::Core::Event&)> gRocketEventHandlers;
extern std::vector<void (*)()> gRocketPresetFunctions;
extern std::vector<void (*)()> gRocketCommitFunctions;
extern std::string rocketBaseDir;

//locals
namespace
{
enum CameraType
{
	free_view_camera, object_picker, none
} cameraType = none;
Rocket::Core::ElementDocument *cameraOptionsMenu = NULL;
SMARTPTR(Object)camera;
ParameterTable cameraDriverParams, cameraChaserParams;

//add elements (tags) function for main menu
void rocketAddElements(Rocket::Core::ElementDocument * mainMenu)
{
	//<button onclick="camera::options">Camera options</button><br/>
	Rocket::Core::Element* content = mainMenu->GetElementById("content");
	Rocket::Core::Element* exit = mainMenu->GetElementById(
			"main::button::exit");
	if (content)
	{
		//create input element
		Rocket::Core::Dictionary params;
		params.Set("onclick", "camera::options");
		Rocket::Core::Element* input = Rocket::Core::Factory::InstanceElement(
		NULL, "button", "button", params);
		Rocket::Core::Factory::InstanceElementText(input, "Camera options");
		//create br element
		params.Clear();
		params.Set("id", "br");
		Rocket::Core::Element* br = Rocket::Core::Factory::InstanceElement(
		NULL, "br", "br", params);
		//inster elements
		content->InsertBefore(input, exit);
		input->RemoveReference();
		content->InsertBefore(br, exit);
		br->RemoveReference();
	}
}

//helper
inline void setElementValue(const std::string& param)
{
	float valueFloat;
	valueFloat =
			strtof(
					(*cameraDriverParams.find(param.c_str())).second.c_str(),
					NULL);
	cameraOptionsMenu->GetElementById(param.c_str())->SetAttribute<float>("value",
			(valueFloat >= 0.0 ? valueFloat : -valueFloat));
}

//event handler added to the main one
void rocketEventHandler(const Rocket::Core::String& value,
		Rocket::Core::Event& event)
{
	if (value == "camera::options")
	{
		//hide main menu
		gRocketMainMenu->Hide();
		// Load and show the camera options document.
		cameraOptionsMenu = gRocketContext->LoadDocument(
				(rocketBaseDir + "misc/ely-camera-options.rml").c_str());
		if (cameraOptionsMenu != NULL)
		{
			cameraOptionsMenu->GetElementById("title")->SetInnerRML(
					cameraOptionsMenu->GetTitle());
			///update radio buttons
			//camera type
			switch (cameraType)
			{
			case free_view_camera:
				cameraOptionsMenu->GetElementById("free_view_camera")->SetAttribute(
						"checked", true);
				break;
			case object_picker:
				cameraOptionsMenu->GetElementById("object_picker")->SetAttribute(
						"checked", true);
				break;
			case none:
				cameraOptionsMenu->GetElementById("none")->SetAttribute(
						"checked", true);
				break;
			default:
				break;
			}
			//
			cameraOptionsMenu->Show();
			cameraOptionsMenu->RemoveReference();
		}
	}
	else if (value == "camera::body::load_logo")
	{
	}
	else if (value == "camera::form::submit_options")
	{
		Rocket::Core::String paramValue;
		//check if ok or cancel
		paramValue = event.GetParameter<Rocket::Core::String>("submit",	"cancel");
		if (paramValue == "ok")
		{
			//set new camera type
			paramValue = event.GetParameter<Rocket::Core::String>("camera", "none");
			if (paramValue == "free_view_camera")
			{
				cameraType = free_view_camera;
			}
			else if (paramValue == "object_picker")
			{
				cameraType = object_picker;
			}
			else
			{
				//default
				cameraType = none;
			}
		}
		//close (i.e. unload) the camera options menu and set as closed..
		cameraOptionsMenu->Close();
		cameraOptionsMenu = NULL;
		//return to main menu.
		gRocketMainMenu->Show();
	}
	else if (value == "camera::free_view_camera::options")
	{
		// This event is sent from the "onchange" of the "free_view_camera"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* free_view_camera_options = options_body->GetElementById(
				"free_view_camera_options");
		if (free_view_camera_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				free_view_camera_options->SetProperty("display", "none");
			}
			else
			{
				free_view_camera_options->SetProperty("display", "block");
				//set options values
				//max linear speed
				setElementValue("max_linear_speed");
				//max angular speed
				setElementValue("max_angular_speed");
				//linear accel
				setElementValue("linear_accel");
				//angular accel
				setElementValue("angular_accel");
				//linear friction
				setElementValue("linear_friction");
				//angular friction
				setElementValue("angular_friction");
				//fast factor
				setElementValue("fast_factor");
				//sens_x
				setElementValue("sens_x");
				//sens_y
				setElementValue("sens_y");
			}
		}
	}
}

//helper
inline void setCameraType(const CameraType& newType, const CameraType& actualType)
{
	//return if no camera type change is needed
	RETURN_ON_COND(newType == actualType,)

	//unset actual camera type
	if (actualType == free_view_camera)
	{
		SMARTPTR(Driver)cameraControl = DCAST(Driver, camera->getComponent(
						ComponentFamilyType("Control")));
		if (cameraControl->isEnabled())
		{
			//enabled: then disable it
			//disable
			RETURN_ON_COND(cameraControl->disable() != Driver::Result::OK,)

			///<DEFAULT CAMERA CONTROL>
//			WindowFramework* gameWindow =
//				GameManager::GetSingletonPtr()->windowFramework();
//			//reset trackball transform
//			LMatrix4 cameraMat = gameWindow->get_camera_group().get_transform()->get_mat();
//			cameraMat.invert_in_place();
//			SMARTPTR(Trackball)trackBall =
//			DCAST(Trackball, gameWindow->get_mouse().find("**/+Trackball").node());
//			trackBall->set_mat(cameraMat);
//			//(re)enable trackball
//			GameManager::GetSingletonPtr()->enable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//remove text
			textNode.remove_node();
		}
	}
	else if(actualType == object_picker)
	{
		if (Picker::GetSingletonPtr())
		{
			//picker on: remove
			delete Picker::GetSingletonPtr();
			//remove text
			textNode.remove_node();
		}
	}
	//set new type
	if (newType == free_view_camera)
	{
		SMARTPTR(Driver)cameraControl = DCAST(Driver, camera->getComponent(
						ComponentFamilyType("Control")));
		if (not cameraControl->isEnabled())
		{
			//disabled: then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			GameManager::GetSingletonPtr()->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			RETURN_ON_COND(cameraControl->enable() != Driver::Result::OK,)
			//write text
			writeText("Free View Camera", 0.05,
					LVecBase4(1.0, 1.0, 0.0, 1.0), LVector3f(-1.0, 0, -0.9));
		}
	}
	else if(newType == object_picker)
	{
		if (not Picker::GetSingletonPtr())
		{
			//picker off: add
			new Picker(camera->objectTmpl()->pandaFramework(),
					camera->objectTmpl()->windowFramework(),
					"shift-mouse1", "mouse1-up");
			//write text
			writeText("Object Picker Active", 0.05,
					LVecBase4(1.0, 1.0, 0.0, 1.0), LVector3f(-1.0, 0, -0.9));
		}
	}
}

//preset function calle from main menu
void rocketPreset()
{
	setCameraType(none, cameraType);
}

//commit function called main menu
void rocketCommit()
{
	setCameraType(cameraType, none);
}

} // namespace

void camera_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//camera
	//enable/disable camera control by event
//	pandaFramework->define_key("c", "toggleCameraControl", &toggleCameraControl,
//	static_cast<void*>(object));
//	//enable/disable a picker
//	pandaFramework->define_key("x", "togglePicker", &togglePicker,
//	static_cast<void*>(object));
//	//toggle chased object
//	pandaFramework->define_key("z", "toggleChasedObject", &toggleChasedObject,
//	static_cast<void*>(object));

	///libRocket
	camera = object;
	//get Driver and Chaser component template parameters
	cameraDriverParams = ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject(ObjectId("cameraDriverTmp"))->getStoredCompTmplParams()["Driver"];
	cameraChaserParams =  ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject(ObjectId("cameraChaserTmp"))->getStoredCompTmplParams()["Driver"];
	//destroy no more needed cameraDriver and cameraChaser objects
	ObjectTemplateManager::GetSingletonPtr()->destroyObject(
			ObjectId("cameraDriverTmp"));
	ObjectTemplateManager::GetSingletonPtr()->destroyObject(
			ObjectId("cameraChaserTmp"));

	///XXX for testing
	//add the cameraDriver by default to camera
	ObjectTemplateManager::GetSingletonPtr()->addComponentToObject(
			ObjectId("camera"), ComponentType("Driver"), cameraDriverParams);
	///XXX

	//register the add element function to main menu
	gRocketAddElementsFunctions.push_back(&rocketAddElements);
	//register the event handler to main menu for each event value
	gRocketEventHandlers["camera::options"] = &rocketEventHandler;
	gRocketEventHandlers["camera::body::load_logo"] = &rocketEventHandler;
	gRocketEventHandlers["camera::form::submit_options"] = &rocketEventHandler;
	gRocketEventHandlers["camera::free_view_camera::options"] = &rocketEventHandler;
	//register the preset function to main menu
	gRocketPresetFunctions.push_back(&rocketPreset);
	//register the commit function to main menu
	gRocketCommitFunctions.push_back(&rocketCommit);
}

void cameraInit()
{
	chasedDataPtr = new std::vector<ChasedObjectData>();
	chasedDataPtr->push_back(
	{ 25.0, 18.0, 8.0, 5.0, ObjectId("auto1") });
	chasedDataPtr->push_back(
	{ 15.0, 8.0, 6.0, 3.0, ObjectId("player0") });
	chasedListSize = chasedDataPtr->size();
}

void cameraEnd()
{
	delete chasedDataPtr;
}

