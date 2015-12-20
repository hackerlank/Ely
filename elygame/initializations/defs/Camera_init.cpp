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
 * \file /Ely/elygame/initializations/defs/Camera_init.cpp
 *
 * \date 2012-09-05 
 * \author consultit
 */

#include "../common_configs.h"
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include "Game_init.h"
#include "Game/GameGUIManager.h"
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

//XXX to be removed
bool controlGrabbed = false;

//locals
namespace
{
//common text writing
NodePath textNode;

enum CameraType
{
	free_view_camera, chaser_camera, object_picker, none
} cameraType = none;
SMARTPTR(Object)camera;
ParameterTable cameraDriverParams, cameraChaserParams;
ObjectId chasedObject;
bool pickerCsIspherical = true;

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

//helpers
inline void setElementValue(const std::string& param,
		ParameterTable& paramsTable, Rocket::Core::ElementDocument *document)
{
	Rocket::Controls::ElementFormControlInput *inputElem =
			dynamic_cast<Rocket::Controls::ElementFormControlInput *>(document->GetElementById(
					param.c_str()));
	inputElem->SetValue((*paramsTable.find(param)).second.c_str());
}

inline void setElementChecked(const std::string& param,
		const std::string& checked, const std::string& unchecked,
		const std::string& defaultValue, ParameterTable& paramsTable,
		Rocket::Core::ElementDocument *document)
{
	Rocket::Controls::ElementFormControlInput *inputElem =
			dynamic_cast<Rocket::Controls::ElementFormControlInput *>(document->GetElementById(
					param.c_str()));
	std::string actualChecked = (*paramsTable.find(param)).second;
	if (actualChecked == checked)
	{
		inputElem->SetAttribute<Rocket::Core::String>("checked", "true");
	}
	else if (actualChecked == unchecked)
	{
		inputElem->RemoveAttribute("checked");
	}
	else
	{
		defaultValue == checked ?
				inputElem->SetAttribute<Rocket::Core::String>("checked",
						"true") :
				inputElem->RemoveAttribute("checked");
	}
}

inline void setOptionValue(Rocket::Core::Event& event, const std::string& param,
		ParameterTable& paramsTable)
{
	(*paramsTable.find(param)).second =
			event.GetParameter<Rocket::Core::String>(param.c_str(), "0.0").CString();
}

inline void setOptionChecked(Rocket::Core::Event& event,
		const std::string& param, const std::string& checked,
		const std::string& unchecked, ParameterTable& paramsTable)
{
	std::string paramValue = event.GetParameter<Rocket::Core::String>(
			param.c_str(), "").CString();
	paramValue == "true" ?
			(*paramsTable.find(param)).second = checked :
			(*paramsTable.find(param)).second = unchecked;
}

//event handler added to the main one
void rocketEventHandler(const Rocket::Core::String& value,
		Rocket::Core::Event& event)
{
	if (value == "camera::options")
	{
		//hide main menu
		GameGUIManager::GetSingletonPtr()->gGuiMainMenu->Hide();
		// Load and show the camera options document.
		Rocket::Core::ElementDocument *cameraOptionsMenu =
				GameGUIManager::GetSingletonPtr()->gGuiRocketContext->LoadDocument(
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
			case chaser_camera:
				cameraOptionsMenu->GetElementById("chaser_camera")->SetAttribute(
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
		PRINT_DEBUG("camera::body::load_logo");
	}
	else if (value == "camera::free_view_camera::options")
	{
		// This event is sent from the "onchange" of the "free_view_camera"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* cameraOptionsMenu =
				event.GetTargetElement()->GetOwnerDocument();
		if (cameraOptionsMenu == NULL)
			return;

		Rocket::Core::Element* free_view_camera_options =
				cameraOptionsMenu->GetElementById("free_view_camera_options");
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
				//set elements' values from options' values
				//pitch limit and pitch limit value: enabled@limit
				std::vector<std::string> paramValuesStr = parseCompoundString(
						(*cameraDriverParams.find("pitch_limit")).second, '@');
				Rocket::Controls::ElementFormControlInput *inputValueElem,
						*inputPitchElem;
				//pitch limit value
				inputValueElem =
						dynamic_cast<Rocket::Controls::ElementFormControlInput *>(cameraOptionsMenu->GetElementById(
								"pitch_limit_value"));
				inputValueElem->SetValue(paramValuesStr[1].c_str());
				//pitch limit
				inputPitchElem =
						dynamic_cast<Rocket::Controls::ElementFormControlInput *>(cameraOptionsMenu->GetElementById(
								"pitch_limit"));
				//checks
				if (inputPitchElem->GetAttribute("checked"))
				{
					if (paramValuesStr[0] == "true")
					{
						//enable value input element
						inputValueElem->SetDisabled(false);
					}
					else
					{
						//checked -> unchecked: change event thrown
						inputPitchElem->RemoveAttribute("checked");
					}
				}
				else
				{
					if (paramValuesStr[0] != "true")
					{
						//disable value input element
						inputValueElem->SetDisabled(true);
					}
					else
					{
						//unchecked -> checked: change event thrown
						inputPitchElem->SetAttribute<Rocket::Core::String>(
								"checked", "true");
					}
				}
				//max linear speed
				setElementValue("max_linear_speed", cameraDriverParams,
						cameraOptionsMenu);
				//max angular speed
				setElementValue("max_angular_speed", cameraDriverParams,
						cameraOptionsMenu);
				//linear accel
				setElementValue("linear_accel", cameraDriverParams,
						cameraOptionsMenu);
				//angular accel
				setElementValue("angular_accel", cameraDriverParams,
						cameraOptionsMenu);
				//linear friction
				setElementValue("linear_friction", cameraDriverParams,
						cameraOptionsMenu);
				//angular friction
				setElementValue("angular_friction", cameraDriverParams,
						cameraOptionsMenu);
				//fast factor
				setElementValue("fast_factor", cameraDriverParams,
						cameraOptionsMenu);
				//sens_x
				setElementValue("sens_x", cameraDriverParams,
						cameraOptionsMenu);
				//sens_y
				setElementValue("sens_y", cameraDriverParams,
						cameraOptionsMenu);
			}
		}
	}
	else if (value == "camera::chaser_camera::options")
	{
		// This event is sent from the "onchange" of the "chaser_camera"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* cameraOptionsMenu =
				event.GetTargetElement()->GetOwnerDocument();
		if (cameraOptionsMenu == NULL)
			return;

		Rocket::Core::Element* chaser_camera_options =
				cameraOptionsMenu->GetElementById("chaser_camera_options");
		if (chaser_camera_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				chaser_camera_options->SetProperty("display", "none");
			}
			else
			{
				chaser_camera_options->SetProperty("display", "block");
				//set elements' values from options' values
				//chased object
				Rocket::Controls::ElementFormControlSelect *objectsSelect =
						dynamic_cast<Rocket::Controls::ElementFormControlSelect *>(cameraOptionsMenu->GetElementById(
								"chased_object"));
				if (objectsSelect)
				{
					//remove all options
					objectsSelect->RemoveAll();
					//set object list
					std::list<SMARTPTR(Object)>::iterator objectIter;
					std::list<SMARTPTR(Object)> createdObjects =
					//to be safe with threads get a copy of created objects' list
					ObjectTemplateManager::GetSingletonPtr()->getCreatedObjects();
					int selectedIdx = objectsSelect->Add("", "");
					for (objectIter = createdObjects.begin();
							objectIter != createdObjects.end(); ++objectIter)
					{
						//don't add steady or with no parent objects
						if ((*objectIter)->isSteady()
								or (*objectIter)->getNodePath().get_parent().is_empty())
						{
							continue;
						}
						//add options
						ObjectId objectId = (*objectIter)->objectId();
						int i = objectsSelect->Add(objectId.c_str(),
								objectId.c_str());
						if (objectId == chasedObject)
						{
							selectedIdx = i;
						}
					}
					//set first option as selected
					objectsSelect->SetSelection(selectedIdx);
				}
				//fixed relative position
				setElementChecked("fixed_relative_position", "true", "false",
						"true", cameraChaserParams, cameraOptionsMenu);
				//backward
				setElementChecked("backward", "true", "false",
						"true", cameraChaserParams, cameraOptionsMenu);
				//abs max distance
				setElementValue("abs_max_distance", cameraChaserParams,
						cameraOptionsMenu);
				//abs min distance
				setElementValue("abs_min_distance", cameraChaserParams,
						cameraOptionsMenu);
				//abs max height
				setElementValue("abs_max_height", cameraChaserParams,
						cameraOptionsMenu);
				//abs min height
				setElementValue("abs_min_height", cameraChaserParams,
						cameraOptionsMenu);
				//abs lookat distance
				setElementValue("abs_lookat_distance", cameraChaserParams,
						cameraOptionsMenu);
				//abs lookat height
				setElementValue("abs_lookat_height", cameraChaserParams,
						cameraOptionsMenu);
				//friction
				setElementValue("friction", cameraChaserParams, cameraOptionsMenu);
			}
		}
	}
	else if (value == "camera::object_picker::options")
	{
		// This event is sent from the "onchange" of the "object_picker"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* cameraOptionsMenu =
				event.GetTargetElement()->GetOwnerDocument();
		if (cameraOptionsMenu == NULL)
			return;

		Rocket::Core::Element* object_picker_options =
				cameraOptionsMenu->GetElementById("object_picker_options");
		if (object_picker_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				object_picker_options->SetProperty("display", "none");
			}
			else
			{
				object_picker_options->SetProperty("display", "block");
				//set elements' values from options' values
				//constraint type
				pickerCsIspherical ?
						cameraOptionsMenu->GetElementById("spherical_constraint")->SetAttribute(
								"checked", true) :
						cameraOptionsMenu->GetElementById("generic_constraint")->SetAttribute(
								"checked", true);
			}
		}
	}
	else if (value == "pitch_limit::change")
	{
		Rocket::Core::ElementDocument* cameraOptionsMenu =
				event.GetTargetElement()->GetOwnerDocument();
		//check
		if (dynamic_cast<Rocket::Controls::ElementFormControlInput *>(cameraOptionsMenu->GetElementById(
				"pitch_limit"))->GetAttribute("checked"))
		{
			dynamic_cast<Rocket::Controls::ElementFormControlInput *>(cameraOptionsMenu->GetElementById(
					"pitch_limit_value"))->SetDisabled(false);
		}
		else
		{
			dynamic_cast<Rocket::Controls::ElementFormControlInput *>(cameraOptionsMenu->GetElementById(
					"pitch_limit_value"))->SetDisabled(true);
		}
	}
	///Submit
	else if (value == "camera::form::submit_options")
	{
		Rocket::Core::String paramValue;
		//check if ok or cancel
		paramValue = event.GetParameter<Rocket::Core::String>("submit",
				"cancel");
		if (paramValue == "ok")
		{
			//set new camera type
			paramValue = event.GetParameter<Rocket::Core::String>("camera",
					"none");
			if (paramValue == "free_view_camera")
			{
				cameraType = free_view_camera;
				//set options' values from elements' values
				//pitch limit and value: enabled@limit
				std::string enabled = (
						event.GetParameter<Rocket::Core::String>("pitch_limit",
								"") == Rocket::Core::String("true") ?
								"true" : "false");
				std::string limit;
				if (enabled == "true")
				{
					limit = event.GetParameter<Rocket::Core::String>(
							"pitch_limit_value", "0.0").CString();
				}
				else
				{
					limit = parseCompoundString(
							(*cameraDriverParams.find("pitch_limit")).second,
							'@')[1];
				}
				(*cameraDriverParams.find("pitch_limit")).second = enabled
						+ std::string("@") + limit;
				//max linear speed
				setOptionValue(event, "max_linear_speed", cameraDriverParams);
				//max angular speed
				setOptionValue(event, "max_angular_speed", cameraDriverParams);
				//linear accel
				setOptionValue(event, "linear_accel", cameraDriverParams);
				//angular accel
				setOptionValue(event, "angular_accel", cameraDriverParams);
				//linear friction
				setOptionValue(event, "linear_friction", cameraDriverParams);
				//angular friction
				setOptionValue(event, "angular_friction", cameraDriverParams);
				//fast factor
				setOptionValue(event, "fast_factor", cameraDriverParams);
				//sens_x
				setOptionValue(event, "sens_x", cameraDriverParams);
				//sens_y
				setOptionValue(event, "sens_y", cameraDriverParams);
			}
			else if (paramValue == "chaser_camera")
			{
				cameraType = chaser_camera;
				//set options' values from elements' values
				//chased object
				chasedObject = event.GetParameter<Rocket::Core::String>(
						"chased_object", "").CString();
				if (not chasedObject.empty())
				{
					(*cameraChaserParams.find("chased_object")).second =
							chasedObject;
				}
				//fixed relative position
				setOptionChecked(event, "fixed_relative_position", "true",
						"false", cameraChaserParams);
				//backward
				setOptionChecked(event, "backward", "true", "false",
						cameraChaserParams);
				//abs max distance
				setOptionValue(event, "abs_max_distance", cameraChaserParams);
				//abs min distance
				setOptionValue(event, "abs_min_distance", cameraChaserParams);
				//abs max height
				setOptionValue(event, "abs_max_height", cameraChaserParams);
				//abs min height
				setOptionValue(event, "abs_min_height", cameraChaserParams);
				//abs lookat distance
				setOptionValue(event, "abs_lookat_distance",
						cameraChaserParams);
				//abs lookat height
				setOptionValue(event, "abs_lookat_height", cameraChaserParams);
				//friction
				setOptionValue(event, "friction", cameraChaserParams);
			}
			else if (paramValue == "object_picker")
			{
				cameraType = object_picker;
				//set options' values from elements' values
				//constraint type
				paramValue = event.GetParameter<Rocket::Core::String>(
						"constraint_type", "");
				if (paramValue == "spherical")
				{
					pickerCsIspherical = true;
				}
				else if (paramValue == "generic")
				{
					pickerCsIspherical = false;
				}
			}
			else
			{
				//default
				cameraType = none;
			}
		}
		//close (i.e. unload) the camera options menu and set as closed..
		Rocket::Core::ElementDocument* cameraOptionsMenu =
				event.GetTargetElement()->GetOwnerDocument();
		cameraOptionsMenu->Close();
		//return to main menu.
		GameGUIManager::GetSingletonPtr()->gGuiMainMenu->Show();
	}
}

///<DEFAULT CAMERA CONTROL>
///ENABLING
//	WindowFramework* gameWindow =
//	GameManager::GetSingletonPtr()->windowFramework();
//	//reset trackball transform
//	LMatrix4 cameraMat = gameWindow->get_camera_group().get_transform()->get_mat();
//	cameraMat.invert_in_place();
//	SMARTPTR(Trackball)trackBall =
//	DCAST(Trackball, gameWindow->get_mouse().find("**/+Trackball").node());
//	trackBall->set_mat(cameraMat);
//	//(re)enable trackball
//	GameManager::GetSingletonPtr()->enable_mouse();
///DISABLING
//	//disable the trackball
//	GameManager::GetSingletonPtr()->disable_mouse();
///</DEFAULT CAMERA CONTROL>

//helper
inline void setCameraType(const CameraType& newType,
		const CameraType& actualType)
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

			//remove text
			textNode.remove_node();
		}
	}
	else if (actualType == chaser_camera)
	{
		SMARTPTR(Chaser)cameraControl = DCAST(Chaser, camera->getComponent(
						ComponentFamilyType("Control")));
		if (cameraControl->isEnabled())
		{
			//enabled: then disable it
			//disable
			RETURN_ON_COND(cameraControl->disable() != Chaser::Result::OK,)

			//remove text
			textNode.remove_node();
		}
	}
	else if(actualType == object_picker)
	{
		if (Picker::GetSingletonPtr())
		{
			SMARTPTR(Driver)cameraControl = DCAST(Driver, camera->getComponent(
							ComponentFamilyType("Control")));
			if (cameraControl->isEnabled())
			{
				//enabled: then disable it
				//disable
				RETURN_ON_COND(cameraControl->disable() != Driver::Result::OK,)
			}

			//picker on: remove
			delete Picker::GetSingletonPtr();
			//remove text
			textNode.remove_node();
		}
	}
	//set new type
	if (newType == free_view_camera)
	{
		//add a new Driver component to camera and ...
		RETURN_ON_COND(
				not ObjectTemplateManager::GetSingletonPtr()->addComponentToObject(
						ObjectId("camera"), ComponentType("Driver"),
						cameraDriverParams),)
		//... enable it
		RETURN_ON_COND(
				DCAST(Driver, camera->getComponent( ComponentFamilyType("Control")))->enable() != Driver::Result::OK,
				)

		//write text
		writeText(textNode, "Free View Camera", 0.05,
				LVecBase4(1.0, 1.0, 0.0, 1.0), LVector3f(-1.0, 0, -0.9));
	}
	else if (newType == chaser_camera)
	{
		//add a new Chaser component to camera and ...
		RETURN_ON_COND(
				not ObjectTemplateManager::GetSingletonPtr()->addComponentToObject(
						ObjectId("camera"), ComponentType("Chaser"),
						cameraChaserParams),)
		//... enable it
		RETURN_ON_COND(
				DCAST(Chaser, camera->getComponent( ComponentFamilyType("Control")))->enable() != Chaser::Result::OK,
				)

		//write text
		writeText(textNode,
				"Camera Chasing '"
						+ DCAST(Chaser, camera->getComponent(
										ComponentFamilyType("Control")))->getChasedObject()
						+ "'", 0.05, LVecBase4(1.0, 1.0, 0.0, 1.0),
				LVector3f(-1.0, 0, -0.9));

	}
	else if (newType == object_picker)
	{
		if (not Picker::GetSingletonPtr())
		{
			//add a new Driver component to camera without mouse movement control and ...
			ParameterTable cameraDriverParamsNoMouse = cameraDriverParams;
			cameraDriverParamsNoMouse.erase("mouse_enabled_h");
			cameraDriverParamsNoMouse.erase("mouse_enabled_p");
			cameraDriverParamsNoMouse.erase("mouse_move");
			cameraDriverParamsNoMouse.insert(
					std::make_pair("mouse_enabled_h", "false"));
			cameraDriverParamsNoMouse.insert(
					std::make_pair("mouse_enabled_p", "false"));
			cameraDriverParamsNoMouse.insert(
					std::make_pair("mouse_move", "disabled"));
			RETURN_ON_COND(
					not ObjectTemplateManager::GetSingletonPtr()->addComponentToObject(
							ObjectId("camera"), ComponentType("Driver"),
							cameraDriverParamsNoMouse),)
			//... enable it
			RETURN_ON_COND(
					DCAST(Driver, camera->getComponent( ComponentFamilyType("Control")))->enable() != Driver::Result::OK,
					)
			//picker off: add
			new Picker(camera->objectTmpl()->pandaFramework(),
					camera->objectTmpl()->windowFramework(), "shift-mouse1",
					"mouse1-up", pickerCsIspherical, 0.0);

			//write text
			writeText(textNode, "Object Picker Active", 0.05,
					LVecBase4(1.0, 1.0, 0.0, 1.0), LVector3f(-1.0, 0, -0.9));
		}
	}
}

//preset function called from main menu
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
	//set camera
	camera = object;
	//get Driver and Chaser component template parameters
	cameraDriverParams = ObjectTemplateManager::GetSingletonPtr()->
	getCreatedObject(ObjectId("cameraDriverTmp"))->getStoredCompTmplParams()["Driver"];
	cameraChaserParams = ObjectTemplateManager::GetSingletonPtr()->
	getCreatedObject(ObjectId("cameraChaserTmp"))->getStoredCompTmplParams()["Chaser"];
	//destroy no more needed cameraDriver and cameraChaser objects
	ObjectTemplateManager::GetSingletonPtr()->destroyObject(
	ObjectId("cameraDriverTmp"));
	ObjectTemplateManager::GetSingletonPtr()->destroyObject(
	ObjectId("cameraChaserTmp"));

	//register the add element function to (Rocket) main menu
	GameGUIManager::GetSingletonPtr()->gGuiAddElementsFunctions.push_back(&rocketAddElements);
	//register the event handler to main menu for each event value
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["camera::options"] = &rocketEventHandler;
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["camera::body::load_logo"] = &rocketEventHandler;
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["camera::form::submit_options"] = &rocketEventHandler;
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["camera::free_view_camera::options"] = &rocketEventHandler;
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["camera::chaser_camera::options"] = &rocketEventHandler;
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["camera::object_picker::options"] = &rocketEventHandler;
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["pitch_limit::change"] = &rocketEventHandler;
	//register the preset function to main menu
	GameGUIManager::GetSingletonPtr()->gGuiPresetFunctions.push_back(&rocketPreset);
	//register the commit function to main menu
	GameGUIManager::GetSingletonPtr()->gGuiCommitFunctions.push_back(&rocketCommit);
}

void Camera_initInit()
{
}

void Camera_initEnd()
{
}

