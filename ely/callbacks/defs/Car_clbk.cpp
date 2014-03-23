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
 * \file /Ely/ely/callbacks/defs/Car_clbk.cpp
 *
 * \date 04/ott/2013 (16:45:21)
 * \author consultit
 */

#include "../common_configs.h"
#include "PhysicsControlComponents/Vehicle.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Car + Vehicle related
CALLBACK forward_Vehicle_Car;
CALLBACK stop_forward_Vehicle_Car;
CALLBACK backward_Vehicle_Car;
CALLBACK stop_backward_Vehicle_Car;
CALLBACK turn_left_Vehicle_Car;
CALLBACK stop_turn_left_Vehicle_Car;
CALLBACK turn_right_Vehicle_Car;
CALLBACK stop_turn_right_Vehicle_Car;
CALLBACK brake_Vehicle_Car;
CALLBACK stop_brake_Vehicle_Car;

#ifdef __cplusplus
}
#endif

///Car + Vehicle related CALLBACKs
void forward_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableForward(true);
}

void stop_forward_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableForward(false);
}
void backward_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableBackward(true);
}
void stop_backward_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableBackward(false);
}
void turn_left_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableTurnLeft(true);
}
void stop_turn_left_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableTurnLeft(false);
}
void turn_right_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableTurnRight(true);
}
void stop_turn_right_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableTurnRight(false);
}
void brake_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableBrake(true);
}
void stop_brake_Vehicle_Car(const Event* event, void* data)
{
	//get data
	SMARTPTR(Vehicle) vehicleCar = reinterpret_cast<Vehicle*>(data);

	vehicleCar->enableBrake(false);
}

///Init/end functions: see common_configs.cpp
void Car_clbkInit()
{
}
void Car_clbkEnd()
{
}




