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
#include "ObjectModel/ObjectTemplateManager.h"
#include "Support/Raycaster.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///OpenSteerPlugIn + SteerPlugIn related
CALLBACK add_steer_vehicle_SteerPlugIn_OpenSteerPlugIn;
CALLBACK remove_steer_vehicle_SteerPlugIn_OpenSteerPlugIn;
#ifdef ELY_DEBUG
CALLBACK toggleDebug;
#endif

#ifdef __cplusplus
}
#endif

#define TOBECLONEDOBJECT "steerVehicleToBeCloned"
#define TOBECLONEDRAYMASK 0x7FFFFFFF

///OpenSteerPlugIn + SteerPlugIn related CALLBACKs
void add_steer_vehicle_SteerPlugIn_OpenSteerPlugIn(const Event* event, void* data)
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

		rayCaster->rayCast(BitMask32::all_on());
		hitPos = rayCaster->getHitPos();
	}

	///create the clone
	//tweak clone object's parameters
	//set clone object store_params
	objParams.erase("store_params");
	objParams.insert(std::pair<std::string, std::string>("store_params", "false"));
	//set clone object pos
	objParams.erase("pos");
	std::ostringstream pos;
	pos << hitPos.get_x() << "," << hitPos.get_y() << "," << hitPos.get_z();
	objParams.insert(std::pair<std::string, std::string>("pos", pos.str()));

	//tweak clone components' parameter tables
	//set SteerVehicle type
	compParams["SteerVehicle"].erase("type");
	if (openSteerPlugInName == "One Turning Away")
	{
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("type", "one_turning"));
	}
	else if (openSteerPlugInName == "Pedestrians")
	{
		compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("type", "pedestrian"));
	}
	//set SteerVehicle add_to_plugin
	compParams["SteerVehicle"].erase("add_to_plugin");
	compParams["SteerVehicle"].insert(std::pair<std::string, std::string>("add_to_plugin", steerPlugInObjectId));
	//create actually the clone
	ObjectTemplateManager::GetSingletonPtr()->
	createObject(toBeClonedObject->objectTmpl()->objectType(), ObjectId(),
			objParams, compParams, false);
}

void remove_steer_vehicle_SteerPlugIn_OpenSteerPlugIn(const Event* event, void* data)
{
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = reinterpret_cast<SteerPlugIn*>(data);

	//get object under mouse pointer
	Raycaster* rayCaster = Raycaster::GetSingletonPtr();
	RETURN_ON_COND(not rayCaster,)
	SMARTPTR(Object) hitObject = rayCaster->rayCast(BitMask32::all_on());
	if(hitObject)
	{
		//check if it is has a SteerVehicle
		SMARTPTR(Component) aiComp = hitObject->getComponent(ComponentFamilyType("AI"));
		if(aiComp and aiComp->is_of_type(SteerVehicle::get_class_type()))
		{
			if (std::find(steerPlugIn->getAbstractPlugIn().allVehicles().begin(),
							steerPlugIn->getAbstractPlugIn().allVehicles().end(),
							&DCAST(SteerVehicle, aiComp)->getAbstractVehicle()) !=
					steerPlugIn->getAbstractPlugIn().allVehicles().end() )
			{
				//remove actually the clone
				ObjectTemplateManager::GetSingletonPtr()->destroyObject(hitObject->objectId());
			}
		}
	}
}

#ifdef ELY_DEBUG
void toggleDebug(const Event* event, void* data)
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

