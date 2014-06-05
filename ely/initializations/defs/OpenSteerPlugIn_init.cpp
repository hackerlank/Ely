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
 * \file /Ely/ely/initializations/defs/OpenSteerPlugIn_init.cpp
 *
 * \date 22/mar/2014 (09:17:55)
 * \author consultit
 */

#include "../common_configs.h"
#include "Game_init.h"
#include "AIComponents/SteerPlugIn.h"
#include "AIComponents/OpenSteerLocal/PlugIn_OneTurning.h"
#include "AIComponents/OpenSteerLocal/PlugIn_Pedestrian.h"
#include "AIComponents/OpenSteerLocal/PlugIn_Boids.h"
#include "AIComponents/OpenSteerLocal/PlugIn_MultiplePursuit.h"
#include "AIComponents/OpenSteerLocal/PlugIn_Soccer.h"
#include "AIComponents/OpenSteerLocal/PlugIn_CaptureTheFlag.h"
#include "AIComponents/OpenSteerLocal/PlugIn_LowSpeedTurn.h"
#include "AIComponents/OpenSteerLocal/PlugIn_MapDrive.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GamePhysicsManager.h"
#include "Support/Raycaster.h"

///SteerPlugIn objects related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION steerPlugInOneTurning1_initialization;
INITIALIZATION steerPlugInPedestrian1_initialization;
INITIALIZATION steerPlugInBoid1_initialization;
INITIALIZATION steerPlugInMultiplePursuit1_initialization;
INITIALIZATION steerPlugInSoccer1_initialization;
INITIALIZATION steerPlugInCtf1_initialization;
INITIALIZATION steerPlugInLST1_initialization;
INITIALIZATION steerPlugInMapDrive1_initialization;
//common SteerVehicle initialization
INITIALIZATION steerVehicleToBeCloned_init;

#ifdef __cplusplus
}
#endif

//locals
namespace
{
//common text writing
NodePath textNode;

//flag for libRocket initialization
bool rocketInitialized = false;
enum SteerPlugInType
{
	one_turning = 0,
	pedestrian,
	boid,
	multiple_pursuit,
	soccer,
	capture_the_flag,
	low_speed_turn,
	map_drive,
	none
} activeSteerPlugInType = none;
///XXX
const char* steerPlugInNames[] =
{
	"one_turning",
	"pedestrian",
	"boid",
	"multiple_pursuit",
	"soccer",
	"capture_the_flag",
	"low_speed_turn",
	"map_drive",
	"none"
};
//common globals
std::map<SteerPlugInType, SteerPlugIn*> steerPlugIns;
std::string addKey = "y", removeKey = "shift-y";
Rocket::Core::ElementDocument *steerPlugInOptionsMenu;
//multiple_pursuit globals
ObjectId mpWandererObjectId, mpNewWanderedObjectId;
bool mpWandererExternalUpdate = false;
//capture_the_flag globals
ObjectId ctfSeekerObjectId, ctfNewSeekerObjectId;
float ctfHomeBaseRadius, ctfMinStartRadius, ctfMaxStartRadius,
		ctfBrakingRate, ctfAvoidancePredictTimeMin, ctfAvoidancePredictTimeMax;
bool ctfSeekerExternalUpdate = false;
//low_speed_turn globals
float lstSteeringSpeed;
//soccer globals
enum SoccerActor
{
	player_teamA, player_teamB, ball
} soccerActorSelected = ball;
//map_drive globals
enum DemoSelect
{
	demo_select_0, demo_select_1, demo_select_2
} demoSelect = demo_select_2;
bool usePathFences = true;

//add elements (tags) function for main menu
void rocketAddElements(Rocket::Core::ElementDocument * mainMenu)
{
	//<button onclick="steerPlugIn::options">SteerPlugIn options</button><br/>
	Rocket::Core::Element* content = mainMenu->GetElementById("content");
	Rocket::Core::Element* exit = mainMenu->GetElementById(
			"main::button::exit");
	if (content)
	{
		//create input element
		Rocket::Core::Dictionary params;
		params.Set("onclick", "steerPlugIn::options");
		Rocket::Core::Element* input = Rocket::Core::Factory::InstanceElement(
		NULL, "button", "button", params);
		Rocket::Core::Factory::InstanceElementText(input,
				"SteerPlugIn options");
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
//to create a selection list of objects
inline void createObjectSelectionList(
		Rocket::Controls::ElementFormControlSelect *objectsSelect,
		ObjectId actualSelectedObject)
{
	//remove all options
	objectsSelect->RemoveAll();
	//set object list
	std::list<SMARTPTR(Object)>::iterator objectIter;
	std::list<SMARTPTR(Object)> createdObjects =
	//to be safe with threads get a copy of created objects' list
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObjects();
	//add an empty object
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
		int i = objectsSelect->Add(objectId.c_str(), objectId.c_str());
		if (objectId == actualSelectedObject)
		{
			selectedIdx = i;
		}
	}
	//set first option as selected
	objectsSelect->SetSelection(selectedIdx);
}

inline void setElementValue(const std::string& param, float value)
{
	Rocket::Core::Element *inputElem =
			steerPlugInOptionsMenu->GetElementById(param.c_str());
	if (inputElem)
	{
		inputElem->SetAttribute<float>("value", value);
	}
}

//event handler added to the main one
void rocketEventHandler(const Rocket::Core::String& value,
		Rocket::Core::Event& event)
{
	if (value == "steerPlugIn::options")
	{
		//hide main menu
		gRocketMainMenu->Hide();
		// Load and show the camera options document.
		steerPlugInOptionsMenu = gRocketContext->LoadDocument(
				(rocketBaseDir + "misc/ely-steerPlugIn-options.rml").c_str());
		if (steerPlugInOptionsMenu != NULL)
		{
			steerPlugInOptionsMenu->GetElementById("title")->SetInnerRML(
					steerPlugInOptionsMenu->GetTitle());
			///update controls
			//add key/remove key
			steerPlugInOptionsMenu->GetElementById("add_key")->SetAttribute(
					"value", addKey.c_str());
			steerPlugInOptionsMenu->GetElementById("remove_key")->SetInnerRML(
					removeKey.c_str());
			//hide element for not existing plug in and check the active one
			Rocket::Core::Element* elem;
			for (int p = one_turning; p != (none + 1); ++p)
			{
				elem = steerPlugInOptionsMenu->GetElementById(
						steerPlugInNames[p]);
				if ((p == none) or (steerPlugIns.find((SteerPlugInType) p)
						!= steerPlugIns.end()))
				{
					//there is the plugin
					if (activeSteerPlugInType == p)
					{
						elem->SetAttribute("checked", true);
					}
				}
				else
				{
					elem->GetParentNode()->SetProperty("display", "none");
				}
			}
			//
			steerPlugInOptionsMenu->Show();
			steerPlugInOptionsMenu->RemoveReference();
		}
	}
	else if (value == "steerPlugIn::body::load_logo")
	{
	}
	else if (value == "add_key::change")
	{
		// This event is sent from the "onchange" of the "add_key"
		//input button. It shows or hides the related options.
		Rocket::Core::ElementDocument* body =
				event.GetTargetElement()->GetOwnerDocument();
		if (body == NULL)
			return;

		//The "value" parameter of an "onchange" event is set to
		//the value the control would send if it was submitted;
		//so, the empty string if it is clear or to the "value"
		//attribute of the control if it is set.
		Rocket::Core::String paramValue = event.GetParameter<
				Rocket::Core::String>("value", "");
		if (not paramValue.Empty())
		{
			body->GetElementById("remove_key")->SetInnerRML(
					"shift-" + paramValue);
		}
	}
	else if (value == "steerPlugIn::multiple_pursuit::options")
	{
		// This event is sent from the "onchange" of the "multiple_pursuit"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* multiple_pursuit_options =
				options_body->GetElementById("multiple_pursuit_options");
		if (multiple_pursuit_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				multiple_pursuit_options->SetProperty("display", "none");
			}
			else
			{
				multiple_pursuit_options->SetProperty("display", "block");
				//set elements' values from options' values
				//wanderer object
				Rocket::Controls::ElementFormControlSelect *objectsSelect =
						dynamic_cast<Rocket::Controls::ElementFormControlSelect *>(steerPlugInOptionsMenu->GetElementById(
								"wanderer_object"));
				if (objectsSelect)
				{
					//use the helper to create the selection list
					createObjectSelectionList(objectsSelect, mpWandererObjectId);
				}
				//external update
				mpWandererExternalUpdate ?
						steerPlugInOptionsMenu->GetElementById(
								"external_update_wanderer_yes")->SetAttribute(
								"checked", true) :
						steerPlugInOptionsMenu->GetElementById(
								"external_update_wanderer_no")->SetAttribute(
								"checked", true);
			}
		}
	}
	else if (value == "steerPlugIn::soccer::options")
	{
		// This event is sent from the "onchange" of the "soccer"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* soccer_options = options_body->GetElementById(
				"soccer_options");
		if (soccer_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				soccer_options->SetProperty("display", "none");
			}
			else
			{
				soccer_options->SetProperty("display", "block");
				//set elements' values from options' values
				//players/ball
				switch (soccerActorSelected)
				{
				case player_teamA:
					steerPlugInOptionsMenu->GetElementById("player_teamA")->SetAttribute(
							"checked", true);
					break;
				case player_teamB:
					steerPlugInOptionsMenu->GetElementById("player_teamB")->SetAttribute(
							"checked", true);
					break;
				case ball:
					steerPlugInOptionsMenu->GetElementById("ball")->SetAttribute(
							"checked", true);
					break;
				default:
					break;
				}
			}
		}
	}
	else if (value == "steerPlugIn::capture_the_flag::options")
	{
		// This event is sent from the "onchange" of the "capture_the_flag"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* capture_the_flag_options =
				options_body->GetElementById("capture_the_flag_options");
		if (capture_the_flag_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				capture_the_flag_options->SetProperty("display", "none");
			}
			else
			{
				capture_the_flag_options->SetProperty("display", "block");
				//set elements' values from options' values
				//seeker object
				Rocket::Controls::ElementFormControlSelect *objectsSelect =
						dynamic_cast<Rocket::Controls::ElementFormControlSelect *>(steerPlugInOptionsMenu->GetElementById(
								"seeker_object"));
				if (objectsSelect)
				{
					//use the helper to create the selection list
					createObjectSelectionList(objectsSelect, ctfSeekerObjectId);
				}
				//external update
				ctfSeekerExternalUpdate ?
						steerPlugInOptionsMenu->GetElementById(
								"external_update_seeker_yes")->SetAttribute(
								"checked", true) :
						steerPlugInOptionsMenu->GetElementById(
								"external_update_seeker_no")->SetAttribute(
								"checked", true);
				//other options
				//home base radius
				setElementValue("home_base_radius", ctfHomeBaseRadius);
				//min start radius
				setElementValue("min_start_radius", ctfMinStartRadius);
				//max start radius
				setElementValue("max_start_radius", ctfMaxStartRadius);
				//braking rate
				setElementValue("braking_rate", ctfBrakingRate);
				//avoidance predict time_min
				setElementValue("avoidance_predict_time_min", ctfAvoidancePredictTimeMin);
				//avoidance predict time_max
				setElementValue("avoidance_predict_time_max", ctfAvoidancePredictTimeMax);
			}
		}
	}
	else if (value == "min_start_radius::change")
	{
		float max =
				steerPlugInOptionsMenu->GetElementById("min_start_radius")->GetAttribute<
						float>("max", 0.0);
		float step =
				steerPlugInOptionsMenu->GetElementById("min_start_radius")->GetAttribute<
						float>("step", 0.0);
		ctfMinStartRadius = max * event.GetParameter<float>("value", 0.0);
		//check
		if (ctfMinStartRadius > ctfMaxStartRadius)
		{
			ctfMaxStartRadius = ctfMinStartRadius + step;
			setElementValue("max_start_radius", ctfMaxStartRadius);
		}
	}
	else if (value == "max_start_radius::change")
	{
		float max =
				steerPlugInOptionsMenu->GetElementById("max_start_radius")->GetAttribute<
						float>("max", 0.0);
		float step =
				steerPlugInOptionsMenu->GetElementById("max_start_radius")->GetAttribute<
						float>("step", 0.0);
		ctfMaxStartRadius = max * event.GetParameter<float>("value", 0.0);
		//check
		if (ctfMaxStartRadius < ctfMinStartRadius)
		{
			ctfMinStartRadius = ctfMaxStartRadius - step;
			setElementValue("min_start_radius", ctfMinStartRadius);
		}
	}
	else if (value == "avoidance_predict_time_min::change")
	{
		float max = steerPlugInOptionsMenu->GetElementById(
				"avoidance_predict_time_min")->GetAttribute<float>("max", 0.0);
		float step = steerPlugInOptionsMenu->GetElementById(
				"avoidance_predict_time_min")->GetAttribute<float>("step", 0.0);
		ctfAvoidancePredictTimeMin = max
				* event.GetParameter<float>("value", 0.0);
		//check
		if (ctfAvoidancePredictTimeMin > ctfAvoidancePredictTimeMax)
		{
			ctfAvoidancePredictTimeMax = ctfAvoidancePredictTimeMin + step;
			setElementValue("avoidance_predict_time_max",
					ctfAvoidancePredictTimeMax);
		}
	}
	else if (value == "avoidance_predict_time_max::change")
	{
		float max = steerPlugInOptionsMenu->GetElementById(
				"avoidance_predict_time_max")->GetAttribute<float>("max", 0.0);
		float step = steerPlugInOptionsMenu->GetElementById(
				"avoidance_predict_time_max")->GetAttribute<float>("step", 0.0);
		ctfAvoidancePredictTimeMax = max
				* event.GetParameter<float>("value", 0.0);
		//check
		if (ctfAvoidancePredictTimeMax < ctfAvoidancePredictTimeMin)
		{
			ctfAvoidancePredictTimeMin = ctfAvoidancePredictTimeMax - step;
			setElementValue("avoidance_predict_time_min",
					ctfAvoidancePredictTimeMin);
		}
	}
	else if (value == "steerPlugIn::low_speed_turn::options")
	{
		// This event is sent from the "onchange" of the "low_speed_turn"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* low_speed_turn_options =
				options_body->GetElementById("low_speed_turn_options");
		if (low_speed_turn_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				low_speed_turn_options->SetProperty("display", "none");
			}
			else
			{
				low_speed_turn_options->SetProperty("display", "block");
				//set elements' values from options' values
				//steering speed
				setElementValue("steering_speed", lstSteeringSpeed);
			}
		}
	}
	else if (value == "steerPlugIn::map_drive::options")
	{
		// This event is sent from the "onchange" of the "map_drive"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* map_drive_options =
				options_body->GetElementById("map_drive_options");
		if (map_drive_options)
		{
			//The "value" parameter of an "onchange" event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the "value"
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				map_drive_options->SetProperty("display", "none");
			}
			else
			{
				map_drive_options->SetProperty("display", "block");
				//set elements' values from options' values
				//demo select
				switch (demoSelect)
				{
				case demo_select_0:
					steerPlugInOptionsMenu->GetElementById("demo_select_0")->SetAttribute(
							"checked", true);
					break;
				case demo_select_1:
					steerPlugInOptionsMenu->GetElementById("demo_select_1")->SetAttribute(
							"checked", true);
					break;
				case demo_select_2:
					steerPlugInOptionsMenu->GetElementById("demo_select_2")->SetAttribute(
							"checked", true);
					break;
				default:
					break;
				}
				//use path fences
				usePathFences ?
						steerPlugInOptionsMenu->GetElementById(
								"use_path_fences_yes")->SetAttribute(
								"checked", true) :
						steerPlugInOptionsMenu->GetElementById(
								"use_path_fences_no")->SetAttribute(
								"checked", true);
			}
		}
	}
	///Submit
	else if (value == "steerPlugIn::form::submit_options")
	{
		Rocket::Core::String paramValue;
		//check if ok or cancel
		paramValue = event.GetParameter<Rocket::Core::String>("submit",
				"cancel");
		if (paramValue == "ok")
		{
			//set new add key
			paramValue = event.GetParameter<Rocket::Core::String>("add_key",
					"");
			if (not paramValue.Empty())
			{
				addKey = paramValue.CString();
				removeKey = "shift-" + addKey;
			}
			//set new steer plugin type
			paramValue = event.GetParameter<Rocket::Core::String>("steerPlugIn",
					"");
			if (paramValue == steerPlugInNames[one_turning])
			{
				activeSteerPlugInType = one_turning;
			}
			else if (paramValue == steerPlugInNames[pedestrian])
			{
				activeSteerPlugInType = pedestrian;
			}
			else if (paramValue == steerPlugInNames[boid])
			{
				activeSteerPlugInType = boid;
			}
			else if (paramValue == steerPlugInNames[multiple_pursuit])
			{
				activeSteerPlugInType = multiple_pursuit;
				//set options' values from elements' values
				//new wanderer object
				mpNewWanderedObjectId = event.GetParameter<Rocket::Core::String>(
						"wanderer_object", "").CString();
				//external update
				paramValue = event.GetParameter<Rocket::Core::String>(
						"external_update_wanderer", "");
				paramValue == "yes" ?
						mpWandererExternalUpdate = true :
						//paramValue == "no"
						mpWandererExternalUpdate = false;
			}
			else if (paramValue == steerPlugInNames[soccer])
			{
				activeSteerPlugInType = soccer;
				//set options' values from elements' values
				//player/ball
				paramValue = event.GetParameter<Rocket::Core::String>(
						"player_ball", "");
				if (paramValue == "player_teamA")
				{
					soccerActorSelected = player_teamA;
				}
				else if (paramValue == "player_teamB")
				{
					soccerActorSelected = player_teamB;
				}
				else
				{
					//default: ball
					soccerActorSelected = ball;
				}
			}
			else if (paramValue == steerPlugInNames[capture_the_flag])
			{
				activeSteerPlugInType = capture_the_flag;
				//set options' values from elements' values
				//new seeker object
				ctfNewSeekerObjectId = event.GetParameter<Rocket::Core::String>(
						"seeker_object", "").CString();
				//external update
				paramValue = event.GetParameter<Rocket::Core::String>(
						"external_update_seeker", "");
				paramValue == "yes" ?
						ctfSeekerExternalUpdate = true :
						//paramValue == "no"
						ctfSeekerExternalUpdate = false;
				//other options
				//home base radius
				ctfHomeBaseRadius = event.GetParameter<float>(
						"home_base_radius", 0.0);
				//min start radius
				ctfMinStartRadius = event.GetParameter<float>(
						"min_start_radius", 0.0);
				//max start radius
				ctfMaxStartRadius = event.GetParameter<float>(
						"max_start_radius", 0.0);
				//braking rate
				ctfBrakingRate = event.GetParameter<float>("braking_rate", 0.0);
				//avoidance predict time_min
				ctfAvoidancePredictTimeMin = event.GetParameter<float>(
						"avoidance_predict_time_min", 0.0);
				//avoidance predict time_max
				ctfAvoidancePredictTimeMax = event.GetParameter<float>(
						"avoidance_predict_time_max", 0.0);
			}
			else if (paramValue == steerPlugInNames[low_speed_turn])
			{
				activeSteerPlugInType = low_speed_turn;
				//other options
				//home base radius
				lstSteeringSpeed = event.GetParameter<float>(
						"steering_speed", 0.0);
			}
			else if (paramValue == steerPlugInNames[map_drive])
			{
				activeSteerPlugInType = map_drive;
				//set options' values from elements' values
				//demo select
				paramValue = event.GetParameter<Rocket::Core::String>(
						"demo_select", "");
				if (paramValue == "demo_select_0")
				{
					demoSelect = demo_select_0;
				}
				else if (paramValue == "demo_select_1")
				{
					demoSelect = demo_select_1;
				}
				else
				{
					//default: demo_select_2
					demoSelect = demo_select_2;
				}
				//use path fences
				paramValue = event.GetParameter<Rocket::Core::String>(
						"use_path_fences", "");
				paramValue == "yes" ?
						usePathFences = true :
						//paramValue == "no"
						usePathFences = false;
			}
			else
			{
				//default
				activeSteerPlugInType = none;
			}
		}
		//close (i.e. unload) the camera options menu and set as closed..
		steerPlugInOptionsMenu->Close();
		//return to main menu.
		gRocketMainMenu->Show();
	}
}

#define TOBECLONEDOBJECT "steerVehicleToBeCloned"
void add_vehicle(const Event* event)
{
	RETURN_ON_COND(activeSteerPlugInType == none,)
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[activeSteerPlugInType];

	//get object to be cloned
	SMARTPTR(Object)toBeClonedObject =
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(ObjectId(TOBECLONEDOBJECT));
	RETURN_ON_COND(not toBeClonedObject,)

	//get SteerPlugIn ObjectId
	std::string steerPlugInObjectId = steerPlugIn->getOwnerObject()->objectId();
	//get underlying OpenSteer PlugIn name
	std::string openSteerPlugInName = steerPlugIn->getAbstractPlugIn().name();

	//get the object-to-be-cloned parameter table
	ParameterTable objParams = toBeClonedObject->getStoredObjTmplParams();
	//get the object-to-be-cloned components' parameter tables
	ParameterTableMap compParams = toBeClonedObject->getStoredCompTmplParams();

	//get position under mouse pointer
	Raycaster* rayCaster = Raycaster::GetSingletonPtr();
	RETURN_ON_COND(not rayCaster,)
	LPoint3f hitPos;
	{
		HOLD_REMUTEX(rayCaster->getMutex())

		if (rayCaster->rayCast(BitMask32::all_on()))
		{
			hitPos = rayCaster->getHitPos();
		}
		else
		{
			hitPos = LPoint3f::zero();
		}
	}

	///create the clone
	//tweak clone object's common parameters
	//set clone object store_params
	objParams.erase("store_params");
	objParams.insert(
			std::pair<std::string, std::string>("store_params", "false"));
	//set clone object pos
	objParams.erase("pos");
	std::ostringstream pos;
	pos << hitPos.get_x() << "," << hitPos.get_y() << "," << hitPos.get_z();
	objParams.insert(std::pair<std::string, std::string>("pos", pos.str()));

	//tweak clone components' parameter tables
	compParams["SteerVehicle"].erase("type");
	compParams["SteerVehicle"].erase("mov_type");
	compParams["SteerVehicle"].erase("thrown_events");
	if (openSteerPlugInName == "One Turning Away")
	{
		//set SteerVehicle type, mov_type
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type", "one_turning"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
	}
	else if (openSteerPlugInName == "Pedestrians")
	{
		//set SteerVehicle type, mov_type, thrown_events
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type", "pedestrian"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("thrown_events",
						"avoid_obstacle@@3:avoid_close_neighbor@@"));
	}
	else if (openSteerPlugInName == "Boids")
	{
		//set SteerVehicle type, mov_type, max_force, max_speed, speed
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type", "boid"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "opensteer"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("thrown_events",
						"avoid_obstacle@@3:avoid_close_neighbor@@"));
		//
		compParams["SteerVehicle"].erase("max_force");
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("max_force", "27"));
		compParams["SteerVehicle"].erase("max_speed");
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("max_speed", "20"));
		compParams["SteerVehicle"].erase("speed");
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("speed", "3"));
		//set InstanceOf instance_of, scale
		compParams["InstanceOf"].erase("instance_of");
		compParams["InstanceOf"].insert(
				std::pair<std::string, std::string>("instance_of", "Smiley1"));
		compParams["InstanceOf"].erase("scale");
		compParams["InstanceOf"].insert(
				std::pair<std::string, std::string>("scale", "0.5,0.5,0.5"));
	}
	else if (openSteerPlugInName == "Multiple Pursuit")
	{
		//set SteerVehicle type, mov_type
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type",
						"mp_pursuer"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
	}
	else if (openSteerPlugInName == "Michael's Simple Soccer")
	{
		compParams["SteerVehicle"].erase("max_force");
		compParams["SteerVehicle"].erase("max_speed");
		compParams["SteerVehicle"].erase("speed");
		compParams["InstanceOf"].erase("instance_of");
		compParams["InstanceOf"].erase("scale");
		//set SteerVehicle type, mov_type, max_force, max_speed
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type",
						(soccerActorSelected == ball ? "ball" : "player")));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("max_force",
						(soccerActorSelected == ball ? "9.0" : "3000.7")));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("max_speed",
						(soccerActorSelected == ball ? "9.0" : "10")));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("speed",
						(soccerActorSelected == ball ? "1.0" : "0.0")));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("thrown_events",
						"avoid_neighbor@@"));
		//set InstanceOf instance_of, scale
		compParams["InstanceOf"].insert(
				std::pair<std::string, std::string>("instance_of",
						(soccerActorSelected == ball ?
								"Smiley1" :
								(soccerActorSelected == player_teamA ?
										"Panda1" : "Gorilla1"))));
		compParams["InstanceOf"].insert(
				std::pair<std::string, std::string>("scale",
						(soccerActorSelected == ball ?
								"1.0" :
								(soccerActorSelected == player_teamA ?
										"0.5" : "0.5"))));
	}
	else if (openSteerPlugInName == "Capture the Flag")
	{
		//set SteerVehicle type, mov_type, thrown_events, speed
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type",
						"ctf_enemy"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("thrown_events",
						"avoid_obstacle@@3:avoid_close_neighbor@@"));
		compParams["SteerVehicle"].erase("speed");
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("speed", "1.0"));

	}
	else if (openSteerPlugInName == "Low Speed Turn")
	{
		//set SteerVehicle type, mov_type
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type",
						"low_speed_turn"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
	}
	else if (openSteerPlugInName == "Driving through map based obstacles")
	{
		compParams["SteerVehicle"].erase("max_speed");
		compParams["SteerVehicle"].erase("max_force");
		compParams["InstanceOf"].erase("instance_of");
		compParams["InstanceOf"].erase("scale");
		//set SteerVehicle type, mov_type, max_speed, max_force
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type",
						"map_driver"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("max_speed", "20"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("max_force", "8"));
		//set InstanceOf instance_of, scale
		compParams["InstanceOf"].insert(
				std::pair<std::string, std::string>("instance_of", "vehicle1"));
		compParams["InstanceOf"].insert(
				std::pair<std::string, std::string>("scale", "1.0"));
	}
	//set SteerVehicle add_to_plugin
	compParams["SteerVehicle"].erase("add_to_plugin");
	compParams["SteerVehicle"].insert(
			std::pair<std::string, std::string>("add_to_plugin",
					steerPlugInObjectId));

	//create actually the clone and...
	SMARTPTR(Object)clone = ObjectTemplateManager::GetSingletonPtr()->createObject(
			toBeClonedObject->objectTmpl()->objectType(), ObjectId(), objParams,
			compParams, false);
	//...initialize it
	clone->worldSetup();
}

void remove_vehicle(const Event* event)
{
	RETURN_ON_COND(activeSteerPlugInType == none,)
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[activeSteerPlugInType];

	//get underlying OpenSteer PlugIn name
	std::string openSteerPlugInName = steerPlugIn->getAbstractPlugIn().name();

	//get object under mouse pointer
	Raycaster* rayCaster = Raycaster::GetSingletonPtr();
	RETURN_ON_COND(not rayCaster,)
	SMARTPTR(Object)hitObject = rayCaster->rayCast(BitMask32::all_on());
	if (hitObject)
	{
		//check if it is has a SteerVehicle component
		SMARTPTR(Component)aiComp = hitObject->getComponent(ComponentFamilyType("AI"));
		if(aiComp and (aiComp->componentType() == ComponentType("SteerVehicle")))
		{
			//check if it is the type requested
			OpenSteer::AbstractVehicle* vehicle =
			&DCAST(SteerVehicle, aiComp)->getAbstractVehicle();
			if(openSteerPlugInName == "One Turning Away")
			{
				if(not (dynamic_cast<OneTurning<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalOneTurning<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Pedestrians")
			{
				if(not(dynamic_cast<Pedestrian<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalPedestrian<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Boids")
			{
				if(not (dynamic_cast<Boid<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalBoid<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Multiple Pursuit")
			{
				if(not (dynamic_cast<MpPursuer<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalMpPursuer<SteerVehicle>*>(vehicle) or
								dynamic_cast<MpWanderer<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalMpWanderer<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Michael's Simple Soccer")
			{
				if(not (dynamic_cast<Ball<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalBall<SteerVehicle>*>(vehicle) or
								dynamic_cast<Player<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalPlayer<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Capture the Flag")
			{
					if(not (dynamic_cast<CtfEnemy<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalCtfEnemy<SteerVehicle>*>(vehicle) or
								dynamic_cast<CtfSeeker<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalCtfSeeker<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Low Speed Turn")
			{
				if(not(dynamic_cast<LowSpeedTurn<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalLowSpeedTurn<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Driving through map based obstacles")
			{
				if(not(dynamic_cast<MapDriver<SteerVehicle>*>(vehicle) or
								dynamic_cast<ExternalMapDriver<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else
			{
				return;
			}
			//try to remove this SteerVehicle from this SteerPlugIn
			if(steerPlugIn->removeSteerVehicle(DCAST(SteerVehicle, aiComp))
					== SteerPlugIn::Result::OK)
			{
				//the SteerVehicle belonged to this SteerPlugIn:
				//remove actually the clone
				ObjectTemplateManager::GetSingletonPtr()->destroyObject(hitObject->objectId());
			}
		}
	}
}

//preset function called from main menu
void rocketPreset()
{
	RETURN_ON_COND(activeSteerPlugInType == none,)
	//remove add/remove vehicle event handlers
	EventHandler::get_global_event_handler()->remove_hook(addKey, &add_vehicle);
	EventHandler::get_global_event_handler()->remove_hook(removeKey,
			&remove_vehicle);
}

//commit function called from main menu
void rocketCommit()
{
	RETURN_ON_COND(activeSteerPlugInType == none,)
	//add add/remove vehicle event handlers
	EventHandler::get_global_event_handler()->add_hook(addKey, &add_vehicle);
	EventHandler::get_global_event_handler()->add_hook(removeKey,
			&remove_vehicle);
}

//helper commit for mp and ctf
inline void rocketHelperCommit(SteerPlugInType plugInType, ObjectId& objectId,
		const ObjectId& newObjectId, const std::string& vehicleType,
		bool externalUpdate)
{
	RETURN_ON_COND(activeSteerPlugInType != plugInType,)
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[activeSteerPlugInType];
	//get SteerPlugIn ObjectId
	std::string steerPlugInObjectId = steerPlugIn->getOwnerObject()->objectId();

	RETURN_ON_COND(objectId == newObjectId,)

	//remove SteerVehicle component from wandererObject
	ObjectTemplateManager::GetSingletonPtr()->removeComponentFromObject(
			objectId, ComponentType("SteerVehicle"));

	//get object to be cloned
	SMARTPTR(Object)toBeClonedObject =
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(ObjectId(TOBECLONEDOBJECT));
	RETURN_ON_COND(not toBeClonedObject,)
	//get the object-to-be-cloned components' parameter tables
	ParameterTableMap compParams = toBeClonedObject->getStoredCompTmplParams();
	//tweak clone components' parameter tables
	compParams["SteerVehicle"].erase("type");
	compParams["SteerVehicle"].insert(
			std::pair<std::string, std::string>("type", vehicleType));
	if (externalUpdate)
	{
		//replace the AI component with a externally updated wanderer
		//set SteerVehicle external_update
		compParams["SteerVehicle"].erase("external_update");
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("external_update", "true"));
	}
	else
	{
		//replace the AI component with a normal wanderer
		//set SteerVehicle mov_type
		compParams["SteerVehicle"].erase("mov_type");
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
	}
	//set SteerVehicle add_to_plugin
	compParams["SteerVehicle"].erase("add_to_plugin");
	compParams["SteerVehicle"].insert(
			std::pair<std::string, std::string>("add_to_plugin",
					steerPlugInObjectId));
	//add the SteerVehicle component
	ObjectTemplateManager::GetSingletonPtr()->addComponentToObject(newObjectId,
			ComponentType("SteerVehicle"), compParams["SteerVehicle"]);
	//change object
	objectId = newObjectId;
}

//commit function for Multiple Pursuit
void rocketMultiplePursuitCommit()
{
	//call helper
	rocketHelperCommit(multiple_pursuit, mpWandererObjectId,
			mpNewWanderedObjectId, "mp_wanderer", mpWandererExternalUpdate);
}

//commit function for Capture The Flag
void rocketCaptureTheFlagCommit()
{
	//call helper
	rocketHelperCommit(capture_the_flag, ctfSeekerObjectId,
			ctfNewSeekerObjectId, "ctf_seeker", ctfSeekerExternalUpdate);
	//update plugin options (with gAvoidancePredictTime included)
	RETURN_ON_COND(activeSteerPlugInType != capture_the_flag,)
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[activeSteerPlugInType];
	//
	CtfPlugIn<SteerVehicle>* ctfPlugIn =
			dynamic_cast<CtfPlugIn<SteerVehicle>*>(&(steerPlugIn->getAbstractPlugIn()));
	//set options
	ctfPlugIn->m_CtfPlugInData.gHomeBaseRadius = ctfHomeBaseRadius;
	ctfPlugIn->m_CtfPlugInData.gMinStartRadius = ctfMinStartRadius;
	ctfPlugIn->m_CtfPlugInData.gMaxStartRadius = ctfMaxStartRadius;
	ctfPlugIn->m_CtfPlugInData.gBrakingRate = ctfBrakingRate;
	ctfPlugIn->m_CtfPlugInData.gAvoidancePredictTimeMin =
			ctfAvoidancePredictTimeMin;
	ctfPlugIn->m_CtfPlugInData.gAvoidancePredictTimeMax =
			ctfAvoidancePredictTimeMax;
	ctfPlugIn->m_CtfPlugInData.gAvoidancePredictTime =
			ctfAvoidancePredictTimeMin;
}

//commit function for Low Speed Turn
void rocketLowSpeedTurnCommit()
{
	//update plugin options
	RETURN_ON_COND(activeSteerPlugInType != low_speed_turn,)
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[activeSteerPlugInType];
	//
	LowSpeedTurnPlugIn<SteerVehicle>* lstPlugIn =
			dynamic_cast<LowSpeedTurnPlugIn<SteerVehicle>*>(&(steerPlugIn->getAbstractPlugIn()));
	//set options
	lstPlugIn->steeringSpeed = lstSteeringSpeed;
}

//commit function for Map Drive
void rocketMapDriveCommit()
{
	//update plugin options
	RETURN_ON_COND(activeSteerPlugInType != map_drive,)
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[activeSteerPlugInType];
	//
	MapDrivePlugIn<SteerVehicle>* mapDrivePlugIn =
			dynamic_cast<MapDrivePlugIn<SteerVehicle>*>(&(steerPlugIn->getAbstractPlugIn()));
	//set options
	mapDrivePlugIn->setOptions(demoSelect, usePathFences);
}

//called by all steer plugins, executed only once
void rocketInitOnce()
{
	RETURN_ON_COND(rocketInitialized,)

	//libRocket initialization
	//register the add element function to (Rocket) main menu
	gRocketAddElementsFunctions.push_back(&rocketAddElements);
	//register the event handler to main menu for each event value
	gRocketEventHandlers["steerPlugIn::options"] = &rocketEventHandler;
	gRocketEventHandlers["steerPlugIn::body::load_logo"] = &rocketEventHandler;
	gRocketEventHandlers["add_key::change"] = &rocketEventHandler;
	gRocketEventHandlers["steerPlugIn::form::submit_options"] =
			&rocketEventHandler;
	//register the preset function to main menu
	gRocketPresetFunctions.push_back(&rocketPreset);
	//register the commit function to main menu
	gRocketCommitFunctions.push_back(&rocketCommit);
	//flag rocket initialized
	rocketInitialized = true;
}

} // namespace

///steerPlugInOneTurning1
void steerPlugInOneTurning1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));

	///init libRocket
	steerPlugIns[one_turning] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
}

///steerPlugInPedestrian1
void steerPlugInPedestrian1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));

	///init libRocket
	steerPlugIns[pedestrian] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
}

///steerPlugInBoid1
#define WORLDCENTEROBJECT "beachhouse2_1"
void steerPlugInBoid1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//tweak some parameter
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));

	//set world center/radius around WORLDCENTEROBJECT
	NodePath worldCenterObjectNP = ObjectTemplateManager::GetSingletonPtr()->
	getCreatedObject(ObjectId(WORLDCENTEROBJECT))->getNodePath();
	LVecBase3f modelDims;
	LVector3f modelDeltaCenter;
	float modelRadius;
	GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
	worldCenterObjectNP, modelDims, modelDeltaCenter, modelRadius);
	BoidsPlugIn<SteerVehicle>* boidsPlugIn = dynamic_cast<BoidsPlugIn<SteerVehicle>*>
	(&DCAST(SteerPlugIn, aiComp)->getAbstractPlugIn());
	boidsPlugIn->worldCenter = LVecBase3fToOpenSteerVec3(
	worldCenterObjectNP.get_pos() + LVector3f(0.0, 0.0, 1.5 * modelDims.get_z()));
	boidsPlugIn->worldRadius = 1.5 * modelDims.get_z();

	///init libRocket
	steerPlugIns[boid] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
}

///steerPlugInMultiplePursuit1
void steerPlugInMultiplePursuit1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));

	///init libRocket
	steerPlugIns[multiple_pursuit] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
	//custom setup
	gRocketEventHandlers["steerPlugIn::multiple_pursuit::options"] =
	&rocketEventHandler;
	gRocketCommitFunctions.push_back(&rocketMultiplePursuitCommit);
}

///steerPlugInSoccer1
void steerPlugInSoccer1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));

	//set soccer field
	MicTestPlugIn<SteerVehicle>* micTestplugIn =
	dynamic_cast<MicTestPlugIn<SteerVehicle>*>(&DCAST(SteerPlugIn, aiComp)->
	getAbstractPlugIn());
	OpenSteer::SegmentedPathway* pathWay =
	dynamic_cast<OpenSteer::SegmentedPathway*>(micTestplugIn->getPathway());
	if (pathWay->pointCount() >= 2)
	{
		micTestplugIn->setSoccerField(pathWay->point(0), pathWay->point(1));
	}

	///init libRocket
	steerPlugIns[soccer] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
	//custom setup
	gRocketEventHandlers["steerPlugIn::soccer::options"] = &rocketEventHandler;
}

///steerPlugInCtf1
void steerPlugInCtf1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));

	//set home base center
	CtfPlugIn<SteerVehicle>* ctfPlugIn =
	dynamic_cast<CtfPlugIn<SteerVehicle>*>(&DCAST(SteerPlugIn, aiComp)->
	getAbstractPlugIn());
	OpenSteer::SegmentedPathway* pathWay =
	dynamic_cast<OpenSteer::SegmentedPathway*>(ctfPlugIn->getPathway());
	if (pathWay->pointCount() >= 1)
	{
		ctfPlugIn->m_CtfPlugInData.gHomeBaseCenter = pathWay->point(0);
	}
	//get default options
	ctfHomeBaseRadius = ctfPlugIn->m_CtfPlugInData.gHomeBaseRadius;//1.5
	ctfMinStartRadius = ctfPlugIn->m_CtfPlugInData.gMinStartRadius;//30.0
	ctfMaxStartRadius = ctfPlugIn->m_CtfPlugInData.gMaxStartRadius;//40.0
	ctfBrakingRate = ctfPlugIn->m_CtfPlugInData.gBrakingRate;//0.75
	ctfAvoidancePredictTimeMin =
			ctfPlugIn->m_CtfPlugInData.gAvoidancePredictTimeMin;//0.9
	ctfAvoidancePredictTimeMax =
			ctfPlugIn->m_CtfPlugInData.gAvoidancePredictTimeMax;//2.0

	///init libRocket
	steerPlugIns[capture_the_flag] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
	//custom setup
	gRocketEventHandlers["steerPlugIn::capture_the_flag::options"] = &rocketEventHandler;
	gRocketEventHandlers["min_start_radius::change"] = &rocketEventHandler;
	gRocketEventHandlers["max_start_radius::change"] = &rocketEventHandler;
	gRocketEventHandlers["avoidance_predict_time_min::change"] = &rocketEventHandler;
	gRocketEventHandlers["avoidance_predict_time_max::change"] = &rocketEventHandler;
	gRocketCommitFunctions.push_back(&rocketCaptureTheFlagCommit);
}

///steerPlugInLST1_initialization
void steerPlugInLST1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));
	//set home base center
	LowSpeedTurnPlugIn<SteerVehicle>* lstPlugIn =
	dynamic_cast<LowSpeedTurnPlugIn<SteerVehicle>*>(&DCAST(SteerPlugIn, aiComp)->
	getAbstractPlugIn());
	//get default options
	lstSteeringSpeed = lstPlugIn->steeringSpeed;//1.0

	///init libRocket
	steerPlugIns[low_speed_turn] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
	//custom setup
	gRocketEventHandlers["steerPlugIn::low_speed_turn::options"] = &rocketEventHandler;
	gRocketCommitFunctions.push_back(&rocketLowSpeedTurnCommit);
}

///steerPlugInMapDrive1_initialization
void steerPlugInMapDrive1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));
	//
	MapDrivePlugIn<SteerVehicle>* mapDrivePlugIn =
	dynamic_cast<MapDrivePlugIn<SteerVehicle>*>(&DCAST(SteerPlugIn, aiComp)->
	getAbstractPlugIn());
#ifdef ELY_DEBUG
	//set windowWidth
	mapDrivePlugIn->windowWidth =
	windowFramework->get_graphics_window()->get_properties().get_x_size();
#endif
	//create the map
	OpenSteer::PolylineSegmentedPathwaySegmentRadii* pathWay =
	dynamic_cast<OpenSteer::PolylineSegmentedPathwaySegmentRadii*>(mapDrivePlugIn->getPathway());
	//get map dimensions and center
	float minMaxX[2] = { FLT_MAX, -FLT_MAX}; //min,max
	float minMaxY[2] = { FLT_MAX, -FLT_MAX}; //min,max
	LPoint3f mapCenter = LPoint3f::zero();
	for (OpenSteer::SegmentedPathway::size_type i = 0;
			i < pathWay->pointCount(); ++i)
	{
		LPoint3f point = OpenSteerVec3ToLVecBase3f(pathWay->point(i));
		if (point.get_x() < minMaxX[0])
		{
			minMaxX[0] = point.get_x();
		}
		if (point.get_x() > minMaxX[1])
		{
			minMaxX[1] = point.get_x();
		}
		//
		if (point.get_y() < minMaxY[0])
		{
			minMaxY[0] = point.get_y();
		}
		if (point.get_y() > minMaxY[1])
		{
			minMaxY[1] = point.get_y();
		}
		//
		mapCenter += point;
	}
	//take the middle point
	if(pathWay->pointCount() > 0)
	{
		mapCenter /= pathWay->pointCount();
	}
	float maxRadius = 0.0;
	for (OpenSteer::SegmentedPathway::size_type i = 0;
	i<pathWay->segmentCount(); ++i)
	{
		if (pathWay->segmentRadius(i) > maxRadius)
		{
			maxRadius = pathWay->segmentRadius(i);
		}
	}
	//set worldSize to max spread between dX and dY and resolution = 200
	float dX = minMaxX[1] - minMaxX[0];
	float dY =  minMaxY[1] - minMaxY[0];
	mapDrivePlugIn->makeMap(LVecBase3fToOpenSteerVec3(mapCenter),
			max(dX, dY) + maxRadius * 5.0, 200);
	///init libRocket
	steerPlugIns[map_drive] = DCAST(SteerPlugIn, aiComp);
	rocketInitOnce();
	//custom setup
	gRocketEventHandlers["steerPlugIn::map_drive::options"] = &rocketEventHandler;
	gRocketCommitFunctions.push_back(&rocketMapDriveCommit);
}

///steerVehicleToBeCloned_init
void steerVehicleToBeCloned_init(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Component) aiComp = object->getComponent(ComponentFamilyType("AI"));

	//check if it is a soccer player
	Player<SteerVehicle>* player =
	dynamic_cast<Player<SteerVehicle>*>(&DCAST(SteerVehicle, aiComp)->
	getAbstractVehicle());
	if (player)
	{
		MicTestPlugIn<SteerVehicle>* micTestplugIn =
		dynamic_cast<MicTestPlugIn<SteerVehicle>*>(&DCAST(SteerVehicle, aiComp)->
		getSteerPlugIn()->getAbstractPlugIn());
		micTestplugIn->addPlayerToTeam(player,
		soccerActorSelected == player_teamA ? true: false);
	}
}

///init/end
void OpenSteerPlugIn_initInit()
{
}

void OpenSteerPlugIn_initEnd()
{
}

