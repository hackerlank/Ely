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
 * \file /Ely/ely/callbacks/defs/Game_clbk.cpp
 *
 * \date 27/ago/2014 (12:17:38)
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
	std::cout << "Got: " << event->get_name() << " - Object: " <<
			std::string(vehicle->getOwnerObject()->objectId()) << std::endl;
}

///Character + CharacterController related CALLBACKs
void characterGroundAir(const Event* event, void* data)
{
	//get first parameter
	SMARTPTR(CharacterController) characterController =
	DCAST(CharacterController, event->get_parameter(0).get_ptr());
	//
	std::cout << "Got: " << event->get_name() << " - Object: " <<
			std::string(characterController->getOwnerObject()->objectId()) << std::endl;
}

///OpenSteerVehicle + SteerVehicle related CALLBACKs
void handleHits(const Event* event, void* data)
{
	//get first parameter
	SMARTPTR(SteerVehicle)steerVehicle =
	DCAST(SteerVehicle, event->get_parameter(0).get_ptr());
	//
	std::cout << "Got: " << event->get_name() << " - Object: " <<
			std::string(steerVehicle->getOwnerObject()->objectId()) << std::endl;
}

///Init/end functions: see common_configs.cpp
void Game_clbkInit()
{
}
void Game_clbkEnd()
{
}

