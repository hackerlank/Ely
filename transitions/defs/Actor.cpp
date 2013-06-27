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
 * \file /Ely/transitions/defs/Actor.cpp
 *
 * \date 26/nov/2012 (11:37:57)
 * \author consultit
 */

#include "../common_configs.h"

//ENTER Enter_<STATE>_<OBJECTTYPE>;
//EXIT Exit_<STATE>_<OBJECTTYPE>;
//FILTER Filter_<STATE>_<OBJECTTYPE>;
//FROMTO <STATEA>_FromTo_<STATEB>_<OBJECTTYPE>;

///Actor (Activity) related
#ifdef __cplusplus
extern "C"
{
#endif

//forward
ENTER Enter_forward_Actor;
EXIT Exit_forward_Actor;
FILTER Filter_forward_Actor;
//backward
ENTER Enter_backward_Actor;
EXIT Exit_backward_Actor;
FILTER Filter_backward_Actor;
//strafe_left
ENTER Enter_strafe_left_Actor;
EXIT Exit_strafe_left_Actor;
FILTER Filter_strafe_left_Actor;
//strafe_right
ENTER Enter_strafe_right_Actor;
EXIT Exit_strafe_right_Actor;
FILTER Filter_strafe_right_Actor;
//up
ENTER Enter_up_Actor;
EXIT Exit_up_Actor;
FILTER Filter_up_Actor;
//down
ENTER Enter_down_Actor;
EXIT Exit_down_Actor;
FILTER Filter_down_Actor;
//forward_FromTo_strafe_left
FROMTO forward_FromTo_strafe_left_Actor;
//forward_FromTo_strafe_right
FROMTO forward_FromTo_strafe_right_Actor;

#ifdef __cplusplus
}
#endif

///States' functions
//forward
void Enter_forward_Actor(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_forward_Actor");
}
void Exit_forward_Actor(fsm*, Activity& activity)
{
	PRINT("Exit_forward_Actor");
}
ValueList Filter_forward_Actor(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_forward_Actor");
	valList.push_front(state);
	return valList;
}
//backward
void Enter_backward_Actor(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_backward_Actor");
}
void Exit_backward_Actor(fsm*, Activity& activity)
{
	PRINT("Exit_backward_Actor");
}
ValueList Filter_backward_Actor(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_backward_Actor");
	valList.push_front(state);
	return valList;
}
//strafe_left
void Enter_strafe_left_Actor(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("Enter_strafe_left_Actor");
}
void Exit_strafe_left_Actor(fsm*, Activity& activity)
{
	PRINT("Exit_strafe_left_Actor");
}
ValueList Filter_strafe_left_Actor(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_strafe_left_Actor");
	valList.push_front(state);
	return valList;
}
//strafe_right
void Enter_strafe_right_Actor(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("Enter_strafe_right_Actor");
}
void Exit_strafe_right_Actor(fsm*, Activity& activity)
{
	PRINT("Exit_strafe_right_Actor");
}
ValueList Filter_strafe_right_Actor(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_strafe_right_Actor");
	valList.push_front(state);
	return valList;
}
//up
void Enter_up_Actor(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_up_Actor");
}
void Exit_up_Actor(fsm*, Activity& activity)
{
	PRINT("Exit_up_Actor");
}
ValueList Filter_up_Actor(fsm*, Activity& activity, const std::string& state,
		const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_up_Actor");
	valList.push_front(state);
	return valList;
}
//down
void Enter_down_Actor(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_down_Actor");
}
void Exit_down_Actor(fsm*, Activity& activity)
{
	PRINT("Exit_down_Actor");
}
ValueList Filter_down_Actor(fsm*, Activity& activity, const std::string& state,
		const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_down_Actor");
	valList.push_front(state);
	return valList;
}
//forward_FromTo_strafe_left
void forward_FromTo_strafe_left_Actor(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("forward_FromTo_strafe_left_Actor");
}
//forward_FromTo_strafe_right
void forward_FromTo_strafe_right_Actor(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("forward_FromTo_strafe_right_Actor");
}

///Init/end functions: see common_configs.cpp
void actorInit()
{
}
void actorEnd()
{
}
