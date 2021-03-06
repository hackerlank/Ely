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
 * \file /Ely/elygame/callbacks/defs/Actor_clbk.cpp
 *
 * \date 2012-11-26 
 * \author consultit
 */

#include "../common_configs.h"
#include "ObjectModel/Object.h"
#include "BehaviorComponents/Activity.h"
#include "ControlComponents/Driver.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Actor + Activity related
CALLBACK fast_Activity_Actor;
CALLBACK stop_fast_Activity_Actor;
CALLBACK forward_Activity_Actor;
CALLBACK stop_forward_Activity_Actor;
CALLBACK fast_forward_Activity_Actor;
CALLBACK backward_Activity_Actor;
CALLBACK stop_backward_Activity_Actor;
CALLBACK fast_backward_Activity_Actor;
CALLBACK head_left_Activity_Actor;
CALLBACK stop_head_left_Activity_Actor;
CALLBACK fast_head_left_Activity_Actor;
CALLBACK head_right_Activity_Actor;
CALLBACK stop_head_right_Activity_Actor;
CALLBACK fast_head_right_Activity_Actor;
CALLBACK up_Activity_Actor;
CALLBACK stop_up_Activity_Actor;
CALLBACK fast_up_Activity_Actor;
CALLBACK down_Activity_Actor;
CALLBACK stop_down_Activity_Actor;
CALLBACK fast_down_Activity_Actor;

#ifdef __cplusplus
}
#endif

///Actor + Activity related CALLBACKNAMEs & CALLBACKs
//fast:stop-fast
namespace
{
bool isFast = false;
}

void fast_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	if (not isFast)
	{
		float speedFactor = actorDrv->getFastFactor();
		float maxAngularSpeed;
		actorDrv->setMaxLinearSpeed(actorDrv->getMaxSpeeds(maxAngularSpeed) * speedFactor);
		actorDrv->setMaxAngularSpeed(maxAngularSpeed * speedFactor);
		isFast = not isFast;
	}
}
void stop_fast_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	if (isFast)
	{
		float speedFactor = actorDrv->getFastFactor();
		float maxAngularSpeed;
		actorDrv->setMaxLinearSpeed(actorDrv->getMaxSpeeds(maxAngularSpeed) / speedFactor);
		actorDrv->setMaxAngularSpeed(maxAngularSpeed / speedFactor);
		isFast = not isFast;
	}
}
//forward:stop-forward:fast-forward
void forward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableForward(true);
}
void stop_forward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableForward(false);
}
void fast_forward_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	forward_Activity_Actor(event, data);
}
//backward:stop-backward:fast-backward
void backward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableBackward(true);
}
void stop_backward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableBackward(false);
}
void fast_backward_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	backward_Activity_Actor(event, data);
}
//head_left:stop-head_left:fast-head_left
void head_left_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableHeadLeft(true);
}
void stop_head_left_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableHeadLeft(false);
}
void fast_head_left_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	head_left_Activity_Actor(event, data);
}
//head_right:stop-head_right:fast-head_right
void head_right_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableHeadRight(true);
}
void stop_head_right_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableHeadRight(false);
}
void fast_head_right_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	head_right_Activity_Actor(event, data);
}
//up:stop-up:fast-up
void up_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableUp(true);
}
void stop_up_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableUp(false);
}
void fast_up_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	up_Activity_Actor(event, data);
}
//down:stop-down:fast-down
void down_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableDown(true);
}
void stop_down_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv = DCAST(Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent(ComponentFamilyType("Control")));

	actorDrv->enableDown(false);
}
void fast_down_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	down_Activity_Actor(event, data);
}

///Init/end functions: see common_configs.cpp
void Actor_clbkInit()
{
}
void Actor_clbkEnd()
{
}
