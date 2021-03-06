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
 * \file /Ely/elygame/callbacks/defs/Game_clbk.cpp
 *
 * \date 2014-08-27 
 * \author consultit
 */

#include "../common_configs.h"
#include "PhysicsControlComponents/Vehicle.h"
#include "PhysicsControlComponents/CharacterController.h"
#include "AIComponents/SteerVehicle.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Car + Vehicle related
CALLBACK carMoveSteady;
///Character + CharacterController related
CALLBACK characterGroundAir;
///OpenSteerVehicle + SteerVehicle related
CALLBACK handleHits;
///Car + GhostObject related
CALLBACK carGhostOverlap;
///Notify Collisions related
CALLBACK notifyCollisions;

#ifdef __cplusplus
}
#endif

///Car + Vehicle related CALLBACKs
void carMoveSteady(const Event* event, void* data)
{
	//get first parameter
	SMARTPTR(Vehicle) vehicle =
	DCAST(Vehicle, event->get_parameter(0).get_ptr());
	//
	PRINT_DEBUG("Got: " << event->get_name() << " - Object: " <<
			std::string(vehicle->getOwnerObject()->objectId()));
}

///Character + CharacterController related CALLBACKs
void characterGroundAir(const Event* event, void* data)
{
	//get first parameter
	SMARTPTR(CharacterController) characterController =
	DCAST(CharacterController, event->get_parameter(0).get_ptr());
	//
	PRINT_DEBUG("Got: " << event->get_name() << " - Object: " <<
			std::string(characterController->getOwnerObject()->objectId()));
}

///OpenSteerVehicle + SteerVehicle related CALLBACKs
void handleHits(const Event* event, void* data)
{
	//get first parameter
	SMARTPTR(SteerVehicle)steerVehicle =
	DCAST(SteerVehicle, event->get_parameter(0).get_ptr());
	//
	PRINT_DEBUG("Got: " << event->get_name() << " - Object: " <<
			std::string(steerVehicle->getOwnerObject()->objectId()));
}

///Car + GhostObject related CALLBACKs
void carGhostOverlap(const Event* event, void* data)
{
	//get first parameter: overlapping physics component
	SMARTPTR(Component)overlappingCar =
	DCAST(Component, event->get_parameter(0).get_ptr());
	//get second parameter: overlapped ghost component
	SMARTPTR(Component)overlappedGhost =
	DCAST(Component, event->get_parameter(1).get_ptr());
	//
	PRINT_DEBUG("Got: " << event->get_name() << " - Overlapping object: " <<
		std::string(overlappingCar->getOwnerObject()->objectId())
		<< " - Overlapped object: " <<
		std::string(overlappedGhost->getOwnerObject()->objectId()));
}

///Notify Collisions related
void notifyCollisions(const Event* event, void* data)
{
	//get first parameter: overlapping physics component
	SMARTPTR(Component)collidingCar =
	DCAST(Component, event->get_parameter(0).get_ptr());
	//get second parameter: overlapped ghost component
	SMARTPTR(Component)collidingCharacter =
	DCAST(Component, event->get_parameter(1).get_ptr());
	//
	PRINT_DEBUG("Got: " << event->get_name() << " - Colliding objects: " <<
		std::string(collidingCar->getOwnerObject()->objectId())
		<< " - " <<
		std::string(collidingCharacter->getOwnerObject()->objectId()));
}

///Init/end functions: see common_configs.cpp
void Game_clbkInit()
{
}
void Game_clbkEnd()
{
}

