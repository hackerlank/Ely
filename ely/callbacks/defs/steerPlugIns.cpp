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
 * \file /Ely/ely/callbacks/defs/steerPlugIns.cpp
 *
 * \date 11/gen/2014 (10:19:51)
 * \author consultit
 */

#include "../common_configs.h"
#include "AIComponents/SteerPlugIn.h"
#include "AIComponents/OpenSteerLocal/PlugIn_OneTurning.h"
#include "AIComponents/OpenSteerLocal/PlugIn_Pedestrian.h"
#include "AIComponents/OpenSteerLocal/PlugIn_Boids.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Support/Raycaster.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///OpenSteerPlugIn + SteerPlugIn related
CALLBACK add_vehicle;
CALLBACK remove_vehicle;
CALLBACK handleHits;
#ifdef ELY_DEBUG
CALLBACK steerPluginsToggleDebug;
#endif

#ifdef __cplusplus
}
#endif

#define TOBECLONEDOBJECT "steerVehicleToBeCloned"

///OpenSteerPlugIn + SteerPlugIn related CALLBACKs
void add_vehicle(const Event* event, void* data)
{
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = reinterpret_cast<SteerPlugIn*>(data);

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
	objParams.insert(std::pair<std::string, std::string>("store_params", "false"));
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
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("type", "one_turning"));
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("mov_type", "kinematic"));
	}
	else if (openSteerPlugInName == "Pedestrians")
	{
		//set SteerVehicle type, mov_type, thrown_events
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("type", "pedestrian"));
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("mov_type", "kinematic"));
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("thrown_events", "avoid_obstacle@@3:avoid_close_neighbor@@"));
	}
	else if (openSteerPlugInName == "Boids")
	{
		//set SteerVehicle type, mov_type, max_force, max_speed, speed
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("type", "boid"));
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("mov_type", "opensteer"));
		//
		compParams["SteerVehicle"].erase("max_force");
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("max_force", "27"));
		compParams["SteerVehicle"].erase("max_speed");
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("max_speed", "20"));
		compParams["SteerVehicle"].erase("speed");
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("speed", "3"));
		//set InstanceOf instance_of, scale
		compParams["InstanceOf"].erase("instance_of");
		compParams["InstanceOf"].insert(std::pair<std::string, std::string>("instance_of", "Smiley1"));
		compParams["InstanceOf"].erase("max_speed");
		compParams["InstanceOf"].insert(std::pair<std::string, std::string>("scale", "0.1,0.1,0.1"));
	}
	//set SteerVehicle add_to_plugin
	compParams["SteerVehicle"].erase("add_to_plugin");
	compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("add_to_plugin", steerPlugInObjectId));

	//create actually the clone
	ObjectTemplateManager::GetSingletonPtr()->
	createObject(toBeClonedObject->objectTmpl()->objectType(), ObjectId(),
			objParams, compParams, false);
}

void remove_vehicle(const Event* event, void* data)
{
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = reinterpret_cast<SteerPlugIn*>(data);

	//get underlying OpenSteer PlugIn name
	std::string openSteerPlugInName = steerPlugIn->getAbstractPlugIn().name();

	//get object under mouse pointer
	Raycaster* rayCaster = Raycaster::GetSingletonPtr();
	RETURN_ON_COND(not rayCaster,)
	SMARTPTR(Object) hitObject = rayCaster->rayCast(BitMask32::all_on());
	if(hitObject)
	{
		//check if it is has a SteerVehicle component
		SMARTPTR(Component) aiComp = hitObject->getComponent(ComponentFamilyType("AI"));
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

void handleHits(const Event* event, void* data)
{
	//get first parameter
	SMARTPTR(SteerVehicle)steerVehicle =
			DCAST(SteerVehicle, event->get_parameter(0).get_ptr());
	//
	std::cout << "Got: " << event->get_name() << " - Object: " <<
	std::string(steerVehicle->getOwnerObject()->objectId()) << std::endl;
}

#ifdef ELY_DEBUG
void steerPluginsToggleDebug(const Event* event, void* data)
{
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = reinterpret_cast<SteerPlugIn*>(data);

	if (steerPlugIn->getDrawer3dDebugNodePath().is_hidden() and
			steerPlugIn->getDrawer2dDebugNodePath().is_hidden())
	{
		steerPlugIn->debug(true);
	}
	else
	{
		steerPlugIn->debug(false);
	}
}
#endif

///Init/end functions: see common_configs.cpp
void steerPluginsInit()
{
}
void steerPluginsEnd()
{
}

