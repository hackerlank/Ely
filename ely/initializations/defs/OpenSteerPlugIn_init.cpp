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
	one_turning, pedestrians, boids, multiple_pursuit, none
} steerPlugInType = none;
std::map<SteerPlugInType, SteerPlugIn*> steerPlugIns;
std::string addKey = "y", removeKey = "shift-y";
Rocket::Core::ElementDocument *steerPlugInOptionsMenu;
ObjectId wandererObjectId, newWanderedObjectId;
bool wandererExternalUpdate = false;

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
			//steerPlugIn type
			switch (steerPlugInType)
			{
			case one_turning:
				steerPlugInOptionsMenu->GetElementById("one_turning_away")->SetAttribute(
						"checked", true);
				break;
			case pedestrians:
				steerPlugInOptionsMenu->GetElementById("pedestrians")->SetAttribute(
						"checked", true);
				break;
			case boids:
				steerPlugInOptionsMenu->GetElementById("boids")->SetAttribute(
						"checked", true);
				break;
			case multiple_pursuit:
				steerPlugInOptionsMenu->GetElementById("multiple_pursuit")->SetAttribute(
						"checked", true);
				break;
			case none:
				steerPlugInOptionsMenu->GetElementById("none")->SetAttribute(
						"checked", true);
				break;
			default:
				break;
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
		// This event is sent from the "onchange" of the "chaser_camera"
		//radio button. It shows or hides the related options.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* multiple_pursuit_options = options_body->GetElementById(
				"multiple_pursuit_options");
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
						dynamic_cast<Rocket::Controls::ElementFormControlSelect *>(steerPlugInOptionsMenu->GetElementById("wanderer_object"));
				if (objectsSelect)
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
						int i = objectsSelect->Add(objectId.c_str(),
								objectId.c_str());
						if (objectId == wandererObjectId)
						{
							selectedIdx = i;
						}
					}
					//set first option as selected
					objectsSelect->SetSelection(selectedIdx);
				}
				//external update
				wandererExternalUpdate ?
					steerPlugInOptionsMenu->GetElementById("external_update_yes")->SetAttribute(
							"checked", true):
					steerPlugInOptionsMenu->GetElementById("external_update_no")->SetAttribute(
							"checked", true);
			}
		}
	}
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
			if (paramValue == "one_turning_away")
			{
				steerPlugInType = one_turning;
			}
			else if (paramValue == "pedestrians")
			{
				steerPlugInType = pedestrians;
			}
			else if (paramValue == "boids")
			{
				steerPlugInType = boids;
			}
			else if (paramValue == "multiple_pursuit")
			{
				steerPlugInType = multiple_pursuit;
				//set options' values from elements' values
				//new wanderer object
				newWanderedObjectId = event.GetParameter<Rocket::Core::String>(
							"wanderer_object", "").CString();
				//external update
				paramValue = event.GetParameter<Rocket::Core::String>("external_update",
									"");
				paramValue == "yes" ?
					wandererExternalUpdate = true:
					//paramValue == "no"
					wandererExternalUpdate = false;
			}
			else
			{
				//default
				steerPlugInType = none;
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
	RETURN_ON_COND(steerPlugInType == none,)
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[steerPlugInType];

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
		compParams["InstanceOf"].erase("max_speed");
		compParams["InstanceOf"].insert(
				std::pair<std::string, std::string>("scale", "0.1,0.1,0.1"));
	}
	else if (openSteerPlugInName == "Multiple Pursuit")
	{
		//set SteerVehicle type, mov_type
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("type", "multiple_pursuit_pursuer"));
		compParams["SteerVehicle"].insert(
				std::pair<std::string, std::string>("mov_type", "kinematic"));
	}
	//set SteerVehicle add_to_plugin
	compParams["SteerVehicle"].erase("add_to_plugin");
	compParams["SteerVehicle"].insert(
			std::pair<std::string, std::string>("add_to_plugin",
					steerPlugInObjectId));

	//create actually the clone
	ObjectTemplateManager::GetSingletonPtr()->createObject(
			toBeClonedObject->objectTmpl()->objectType(), ObjectId(), objParams,
			compParams, false);
}

void remove_vehicle(const Event* event)
{
	RETURN_ON_COND(steerPlugInType == none,)
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[steerPlugInType];

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
				if((not dynamic_cast<OneTurning<SteerVehicle>*>(vehicle)) and
						(not dynamic_cast<ExternalOneTurning<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Pedestrians")
			{
				if((not dynamic_cast<Pedestrian<SteerVehicle>*>(vehicle)) and
						(not dynamic_cast<ExternalPedestrian<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Boids")
			{
				if((not dynamic_cast<Boid<SteerVehicle>*>(vehicle)) and
						(not dynamic_cast<ExternalBoid<SteerVehicle>*>(vehicle)))
				{
					return;
				}
			}
			else if (openSteerPlugInName == "Multiple Pursuit")
			{
				if((not dynamic_cast<MpPursuer<SteerVehicle>*>(vehicle)) and
						(not dynamic_cast<ExternalMpPursuer<SteerVehicle>*>(vehicle)))
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
	RETURN_ON_COND(steerPlugInType == none,)
	//remove add/remove vehicle event handlers
	EventHandler::get_global_event_handler()->remove_hook(addKey, &add_vehicle);
	EventHandler::get_global_event_handler()->remove_hook(removeKey,
			&remove_vehicle);
}

//commit function called main menu
void rocketCommit()
{
	RETURN_ON_COND(steerPlugInType == none,)
	//add add/remove vehicle event handlers
	EventHandler::get_global_event_handler()->add_hook(addKey, &add_vehicle);
	EventHandler::get_global_event_handler()->add_hook(removeKey,
			&remove_vehicle);
}

//commit function for Multiple Pursuit
void rocketMultiplePursuitCommit()
{
	RETURN_ON_COND(steerPlugInType == none,)
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = steerPlugIns[steerPlugInType];
	//get SteerPlugIn ObjectId
	std::string steerPlugInObjectId = steerPlugIn->getOwnerObject()->objectId();

	RETURN_ON_COND(wandererObjectId == newWanderedObjectId,)

	//remove SteerVehicle component from wandererObject
	ObjectTemplateManager::GetSingletonPtr()->removeComponentFromObject(
			wandererObjectId, ComponentType("SteerVehicle"));

	//get object to be cloned
	SMARTPTR(Object)toBeClonedObject =
	ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(ObjectId(TOBECLONEDOBJECT));
	RETURN_ON_COND(not toBeClonedObject,)
	//get the object-to-be-cloned components' parameter tables
	ParameterTableMap compParams = toBeClonedObject->getStoredCompTmplParams();
	//tweak clone components' parameter tables
	compParams["SteerVehicle"].erase("type");
	compParams["SteerVehicle"].insert(
			std::pair<std::string, std::string>("type",
					"multiple_pursuit_wanderer"));
	if (wandererExternalUpdate)
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
	ObjectTemplateManager::GetSingletonPtr()->addComponentToObject(
			newWanderedObjectId, ComponentType("SteerVehicle"),
			compParams["SteerVehicle"]);
	//change wanderer object
	wandererObjectId = newWanderedObjectId;
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
	steerPlugIns[pedestrians] = DCAST(SteerPlugIn, aiComp);
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
	steerPlugIns[boids] = DCAST(SteerPlugIn, aiComp);
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

///init/end
void OpenSteerPlugIn_initInit()
{
}

void OpenSteerPlugIn_initEnd()
{
}

