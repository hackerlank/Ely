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
 * \file /Ely/callbacks/controlCamera.cpp
 *
 * \date 26/nov/2012 (11:04:25)
 * \author consultit
 */

#include "../common_configs.h"
#include "ControlComponents/Driver.h"
#include "ControlComponents/Chaser.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Camera + Driver related
CALLBACK fast_Driver_Camera;
CALLBACK stop_fast_Driver_Camera;
CALLBACK forward_Driver_Camera;
CALLBACK stop_forward_Driver_Camera;
CALLBACK fast_forward_Driver_Camera;
CALLBACK backward_Driver_Camera;
CALLBACK stop_backward_Driver_Camera;
CALLBACK fast_backward_Driver_Camera;
CALLBACK strafe_left_Driver_Camera;
CALLBACK stop_strafe_left_Driver_Camera;
CALLBACK fast_strafe_left_Driver_Camera;
CALLBACK strafe_right_Driver_Camera;
CALLBACK stop_strafe_right_Driver_Camera;
CALLBACK fast_strafe_right_Driver_Camera;
CALLBACK roll_left_Driver_Camera;
CALLBACK stop_roll_left_Driver_Camera;
CALLBACK fast_roll_left_Driver_Camera;
CALLBACK roll_right_Driver_Camera;
CALLBACK stop_roll_right_Driver_Camera;
CALLBACK fast_roll_right_Driver_Camera;
CALLBACK up_Driver_Camera;
CALLBACK stop_up_Driver_Camera;
CALLBACK fast_up_Driver_Camera;
CALLBACK down_Driver_Camera;
CALLBACK stop_down_Driver_Camera;
CALLBACK fast_down_Driver_Camera;

///Camera + Chaser related
CALLBACK head_left_Chaser_Camera;
CALLBACK stop_head_left_Chaser_Camera;
CALLBACK head_right_Chaser_Camera;
CALLBACK stop_head_right_Chaser_Camera;
CALLBACK pitch_up_Chaser_Camera;
CALLBACK stop_pitch_up_Chaser_Camera;
CALLBACK pitch_down_Chaser_Camera;
CALLBACK stop_pitch_down_Chaser_Camera;
CALLBACK hold_lookat_Chaser_Camera;
CALLBACK stop_hold_lookat_Chaser_Camera;

#ifdef __cplusplus
}
#endif

///Camera + Driver related CALLBACKs
//fast:stop-fast
namespace
{
bool isFast = false;
}
void fast_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	if (not isFast)
	{
		float speedFactor = cameraDrv->getFastFactor();
		float maxAngularSpeed, angularAccel, senX, sensY;
		cameraDrv->setMaxLinearSpeed(cameraDrv->getMaxSpeeds(maxAngularSpeed) * speedFactor);
		cameraDrv->setMaxAngularSpeed(maxAngularSpeed * speedFactor);
		cameraDrv->setLinearAccel(cameraDrv->getAccels(angularAccel) * speedFactor);
		cameraDrv->setAngularAccel(angularAccel * speedFactor);
		cameraDrv->getSens(senX, sensY);
		cameraDrv->setSens(senX * speedFactor, sensY * speedFactor);
		isFast = not isFast;
	}
}
void stop_fast_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	if (isFast)
	{
		float speedFactor = cameraDrv->getFastFactor();
		float maxAngularSpeed, angularAccel, senX, sensY;
		cameraDrv->setMaxLinearSpeed(cameraDrv->getMaxSpeeds(maxAngularSpeed) / speedFactor);
		cameraDrv->setMaxAngularSpeed(maxAngularSpeed / speedFactor);
		cameraDrv->setLinearAccel(cameraDrv->getAccels(angularAccel) / speedFactor);
		cameraDrv->setAngularAccel(angularAccel / speedFactor);
		cameraDrv->getSens(senX, sensY);
		cameraDrv->setSens(senX / speedFactor, sensY / speedFactor);
		isFast = not isFast;
	}
}
//forward:stop-forward:fast-forward
void forward_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableForward(true);
}
void stop_forward_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableForward(false);
}
void fast_forward_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	forward_Driver_Camera(event, data);
}
//backward:stop-backward:fast-backward
void backward_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableBackward(true);
}
void stop_backward_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableBackward(false);
}
void fast_backward_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	backward_Driver_Camera(event, data);
}
//strafe_left:stop-strafe_left:fast-strafe_left
void strafe_left_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeLeft(true);
}
void stop_strafe_left_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeLeft(false);
}
void fast_strafe_left_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	strafe_left_Driver_Camera(event, data);
}
//strafe_right:stop-strafe_right:fast-strafe_right
void strafe_right_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeRight(true);
}
void stop_strafe_right_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeRight(false);
}
void fast_strafe_right_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	strafe_right_Driver_Camera(event, data);
}
//roll_left:stop-roll_left:fast-roll_left
void roll_left_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableRollLeft(true);
}
void stop_roll_left_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableRollLeft(false);
}
void fast_roll_left_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	roll_left_Driver_Camera(event, data);
}
//roll_right:stop-roll_right:fast-roll_right
void roll_right_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableRollRight(true);
}
void stop_roll_right_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableRollRight(false);
}
void fast_roll_right_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	roll_right_Driver_Camera(event, data);
}
//up:stop-up:fast-up
void up_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableUp(true);
}
void stop_up_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableUp(false);
}
void fast_up_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	up_Driver_Camera(event, data);
}
//down:stop-down:fast-down
void down_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableDown(true);
}
void stop_down_Driver_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableDown(false);
}
void fast_down_Driver_Camera(const Event* event, void* data)
{
	fast_Driver_Camera(event, data);
	down_Driver_Camera(event, data);
}

///Camera + Chaser related
void head_left_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enableHeadLeft(true);
}
void stop_head_left_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enableHeadLeft(false);
}
void head_right_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enableHeadRight(true);
}
void stop_head_right_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enableHeadRight(false);
}
void pitch_up_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enablePitchUp(true);
}
void stop_pitch_up_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enablePitchUp(false);
}
void pitch_down_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enablePitchDown(true);
}
void stop_pitch_down_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->enablePitchDown(false);
}
void hold_lookat_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->holdLookAt(true);
}
void stop_hold_lookat_Chaser_Camera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Chaser) cameraChs = reinterpret_cast<Chaser*>(data);

	cameraChs->holdLookAt(false);
}

///Init/end functions: see common_configs.cpp
void controlCameraInit()
{
}
void controlCameraEnd()
{
}

