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
 * \file /Ely/callbacks/driverCamera.cpp
 *
 * \date 26/nov/2012 (11:04:25)
 * \author consultit
 */

#include "../common_configs.h"
#include "Utilities/ComponentSuite.h"

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Camera + Driver related
CALLBACK fastDriverCamera;
CALLBACK stopFastDriverCamera;
CALLBACK forwardDriverCamera;
CALLBACK stopForwardDriverCamera;
CALLBACK fastForwardDriverCamera;
CALLBACK backwardDriverCamera;
CALLBACK stopBackwardDriverCamera;
CALLBACK fastBackwardDriverCamera;
CALLBACK strafeLeftDriverCamera;
CALLBACK stopStrafeLeftDriverCamera;
CALLBACK fastStrafeLeftDriverCamera;
CALLBACK strafeRightDriverCamera;
CALLBACK stopStrafeRightDriverCamera;
CALLBACK fastStrafeRightDriverCamera;
CALLBACK upDriverCamera;
CALLBACK stopUpDriverCamera;
CALLBACK fastUpDriverCamera;
CALLBACK downDriverCamera;
CALLBACK stopDownDriverCamera;
CALLBACK fastDownDriverCamera;

#ifdef __cplusplus
}
#endif

///Camera + Driver related CALLBACKNAMEs & CALLBACKs
//fast:stop-fast
CALLBACKNAME fast_Driver_Camera = "fastDriverCamera";
CALLBACKNAME stop_fast_Driver_Camera = "stopFastDriverCamera";
static bool isFast = false;
void fastDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	if (not isFast)
	{
		float speedFactor = cameraDrv->getFastFactor();
		cameraDrv->setLinearSpeed(cameraDrv->getLinearSpeed() * speedFactor);
		cameraDrv->setAngularSpeed(cameraDrv->getAngularSpeed() * speedFactor);
		isFast = not isFast;
	}
}
void stopFastDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	if (isFast)
	{
		float speedFactor = cameraDrv->getFastFactor();
		cameraDrv->setLinearSpeed(cameraDrv->getLinearSpeed() / speedFactor);
		cameraDrv->setAngularSpeed(cameraDrv->getAngularSpeed() / speedFactor);
		isFast = not isFast;
	}
}
//forward:stop-forward:fast-forward
CALLBACKNAME forward_Driver_Camera = "forwardDriverCamera";
CALLBACKNAME stop_forward_Driver_Camera = "stopForwardDriverCamera";
CALLBACKNAME fast_forward_Driver_Camera = "fastForwardDriverCamera";
void forwardDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableForward(true);
}
void stopForwardDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableForward(false);
}
void fastForwardDriverCamera(const Event* event, void* data)
{
	fastDriverCamera(event, data);
	forwardDriverCamera(event, data);
}
//backward:stop-backward:fast-backward
CALLBACKNAME backward_Driver_Camera = "backwardDriverCamera";
CALLBACKNAME stop_backward_Driver_Camera = "stopBackwardDriverCamera";
CALLBACKNAME fast_backward_Driver_Camera = "fastBackwardDriverCamera";
void backwardDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableBackward(true);
}
void stopBackwardDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableBackward(false);
}
void fastBackwardDriverCamera(const Event* event, void* data)
{
	fastDriverCamera(event, data);
	backwardDriverCamera(event, data);
}
//strafe_left:stop-strafe_left:fast-strafe_left
CALLBACKNAME strafe_left_Driver_Camera = "strafeLeftDriverCamera";
CALLBACKNAME stop_strafe_left_Driver_Camera = "stopStrafeLeftDriverCamera";
CALLBACKNAME fast_strafe_left_Driver_Camera = "fastStrafeLeftDriverCamera";
void strafeLeftDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeLeft(true);
}
void stopStrafeLeftDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeLeft(false);
}
void fastStrafeLeftDriverCamera(const Event* event, void* data)
{
	fastDriverCamera(event, data);
	strafeLeftDriverCamera(event, data);
}
//strafe_right:stop-strafe_right:fast-strafe_right
CALLBACKNAME strafe_right_Driver_Camera = "strafeRightDriverCamera";
CALLBACKNAME stop_strafe_right_Driver_Camera = "stopStrafeRightDriverCamera";
CALLBACKNAME fast_strafe_right_Driver_Camera = "fastStrafeRightDriverCamera";
void strafeRightDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeRight(true);
}
void stopStrafeRightDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableStrafeRight(false);
}
void fastStrafeRightDriverCamera(const Event* event, void* data)
{
	fastDriverCamera(event, data);
	strafeRightDriverCamera(event, data);
}
//up:stop-up:fast-up
CALLBACKNAME up_Driver_Camera = "upDriverCamera";
CALLBACKNAME stop_up_Driver_Camera = "stopUpDriverCamera";
CALLBACKNAME fast_up_Driver_Camera = "fastUpDriverCamera";
void upDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableUp(true);
}
void stopUpDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableUp(false);
}
void fastUpDriverCamera(const Event* event, void* data)
{
	fastDriverCamera(event, data);
	upDriverCamera(event, data);
}
//down:stop-down:fast-down
CALLBACKNAME down_Driver_Camera = "downDriverCamera";
CALLBACKNAME stop_down_Driver_Camera = "stopDownDriverCamera";
CALLBACKNAME fast_down_Driver_Camera = "fastDownDriverCamera";
void downDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableDown(true);
}
void stopDownDriverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)cameraDrv = reinterpret_cast<Driver*>(data);

	cameraDrv->enableDown(false);
}
void fastDownDriverCamera(const Event* event, void* data)
{
	fastDriverCamera(event, data);
	downDriverCamera(event, data);
}

///Init/end functions: see common_configs.cpp
void driverCameraInit()
{
}
void driverCameraEnd()
{
}

