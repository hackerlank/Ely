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
 * \file /Ely/callbacks/activityActor.cpp
 *
 * \date 26/nov/2012 (10:37:44)
 * \author marco
 */

#include "../common_configs.h"
#include "Utilities/ComponentSuite.h"
#include <lvector3.h>

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Actor + Activity related
CALLBACK fastActivityActor;
CALLBACK stopFastActivityActor;
CALLBACK forwardActivityActor;
CALLBACK stopForwardActivityActor;
CALLBACK fastForwardActivityActor;
CALLBACK backwardActivityActor;
CALLBACK stopBackwardActivityActor;
CALLBACK fastBackwardActivityActor;
CALLBACK rollLeftActivityActor;
CALLBACK stopRollLeftActivityActor;
CALLBACK fastRollLeftActivityActor;
CALLBACK rollRightActivityActor;
CALLBACK stopRollRightActivityActor;
CALLBACK fastRollRightActivityActor;
CALLBACK upActivityActor;
CALLBACK stopUpActivityActor;
CALLBACK fastUpActivityActor;
CALLBACK downActivityActor;
CALLBACK stopDownActivityActor;
CALLBACK fastDownActivityActor;

#ifdef __cplusplus
}
#endif

///Actor + Activity related CALLBACKNAMEs & CALLBACKs
//fast:stop-fast
CALLBACKNAME fast_Activity_Actor = "fastActivityActor";
CALLBACKNAME stop_fast_Activity_Actor = "stopFastActivityActor";
static bool isFast = false;
void fastActivityActor(const Event* event, void* data)
{
	//get data
//	SMARTPTR(Activity)activity = reinterpret_cast<Activity*>(data);
//	SMARTPTR(Object) actorObj = activity->getOwnerObject();
//	SMARTPTR(Driver) actorDrv = DCAST (Driver, actorObj->getComponent("Control"));
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	if (not isFast)
	{
		float speedFactor = actorDrv->getFastFactor();
		actorDrv->setLinearSpeed(actorDrv->getLinearSpeed() * speedFactor);
		actorDrv->setAngularSpeed(actorDrv->getAngularSpeed() * speedFactor);
		isFast = not isFast;
	}
}
void stopFastActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	if (isFast)
	{
		float speedFactor = actorDrv->getFastFactor();
		actorDrv->setLinearSpeed(actorDrv->getLinearSpeed() / speedFactor);
		actorDrv->setAngularSpeed(actorDrv->getAngularSpeed() / speedFactor);
		isFast = not isFast;
	}
}
//forward:stop-forward:fast-forward
CALLBACKNAME forward_Activity_Actor = "forwardActivityActor";
CALLBACKNAME stop_forward_Activity_Actor = "stopForwardActivityActor";
CALLBACKNAME fast_forward_Activity_Actor = "fastForwardActivityActor";
void forwardActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableForward(true);
}
void stopForwardActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableForward(false);
}
void fastForwardActivityActor(const Event* event, void* data)
{
	fastActivityActor(event, data);
	forwardActivityActor(event, data);
}
//backward:stop-backward:fast-backward
CALLBACKNAME backward_Activity_Actor = "backwardActivityActor";
CALLBACKNAME stop_backward_Activity_Actor = "stopBackwardActivityActor";
CALLBACKNAME fast_backward_Activity_Actor = "fastBackwardActivityActor";
void backwardActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableBackward(true);
}
void stopBackwardActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableBackward(false);
}
void fastBackwardActivityActor(const Event* event, void* data)
{
	fastActivityActor(event, data);
	backwardActivityActor(event, data);
}
//roll_left:stop-roll_left:fast-roll_left
CALLBACKNAME roll_left_Activity_Actor = "rollLeftActivityActor";
CALLBACKNAME stop_roll_left_Activity_Actor = "stopRollLeftActivityActor";
CALLBACKNAME fast_roll_left_Activity_Actor = "fastRollLeftActivityActor";
void rollLeftActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollLeft(true);
}
void stopRollLeftActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollLeft(false);
}
void fastRollLeftActivityActor(const Event* event, void* data)
{
	fastActivityActor(event, data);
	rollLeftActivityActor(event, data);
}
//roll_right:stop-roll_right:fast-roll_right
CALLBACKNAME roll_right_Activity_Actor = "rollRightActivityActor";
CALLBACKNAME stop_roll_right_Activity_Actor = "stopRollRightActivityActor";
CALLBACKNAME fast_roll_right_Activity_Actor = "fastRollRightActivityActor";
void rollRightActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollRight(true);
}
void stopRollRightActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollRight(false);
}
void fastRollRightActivityActor(const Event* event, void* data)
{
	fastActivityActor(event, data);
	rollRightActivityActor(event, data);
}
//up:stop-up:fast-up
CALLBACKNAME up_Activity_Actor = "upActivityActor";
CALLBACKNAME stop_up_Activity_Actor = "stopUpActivityActor";
CALLBACKNAME fast_up_Activity_Actor = "fastUpActivityActor";
void upActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableUp(true);
}
void stopUpActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableUp(false);
}
void fastUpActivityActor(const Event* event, void* data)
{
	fastActivityActor(event, data);
	upActivityActor(event, data);
}
//down:stop-down:fast-down
CALLBACKNAME down_Activity_Actor = "downActivityActor";
CALLBACKNAME stop_down_Activity_Actor = "stopDownActivityActor";
CALLBACKNAME fast_down_Activity_Actor = "fastDownActivityActor";
void downActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableDown(true);
}
void stopDownActivityActor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableDown(false);
}
void fastDownActivityActor(const Event* event, void* data)
{
	fastActivityActor(event, data);
	downActivityActor(event, data);
}

///Init/end functions: see common_configs.cpp
void activityActorInit()
{
}
void activityActorEnd()
{
}
