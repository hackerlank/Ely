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
 * \file /Ely/transitions/callback_defs.cpp
 *
 * \date 20/ago/2012 (12:50:42)
 * \author marco
 */

#include "transition_defs.h"
#include "Utilities/ComponentSuite.h"

///Actor1 related
//forward
void Enter_forward_Actor1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_forward_Actor1");
}
void Exit_forward_Actor1(fsm*, Activity& activity)
{
	PRINT("Exit_forward_Actor1");
}
ValueList Filter_forward_Actor1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_forward_Actor1");
	valList.push_front(state);
	return valList;
}
//backward
void Enter_backward_Actor1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_backward_Actor1");
}
void Exit_backward_Actor1(fsm*, Activity& activity)
{
	PRINT("Exit_backward_Actor1");
}
ValueList Filter_backward_Actor1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_backward_Actor1");
	valList.push_front(state);
	return valList;
}
//strafe_left
void Enter_strafe_left_Actor1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("Enter_strafe_left_Actor1");
}
void Exit_strafe_left_Actor1(fsm*, Activity& activity)
{
	PRINT("Exit_strafe_left_Actor1");
}
ValueList Filter_strafe_left_Actor1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_strafe_left_Actor1");
	valList.push_front(state);
	return valList;
}
//strafe_right
void Enter_strafe_right_Actor1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("Enter_strafe_right_Actor1");
}
void Exit_strafe_right_Actor1(fsm*, Activity& activity)
{
	PRINT("Exit_strafe_right_Actor1");
}
ValueList Filter_strafe_right_Actor1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_strafe_right_Actor1");
	valList.push_front(state);
	return valList;
}
//up
void Enter_up_Actor1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_up_Actor1");
}
void Exit_up_Actor1(fsm*, Activity& activity)
{
	PRINT("Exit_up_Actor1");
}
ValueList Filter_up_Actor1(fsm*, Activity& activity, const std::string& state,
		const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_up_Actor1");
	valList.push_front(state);
	return valList;
}
//down
void Enter_down_Actor1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_down_Actor1");
}
void Exit_down_Actor1(fsm*, Activity& activity)
{
	PRINT("Exit_down_Actor1");
}
ValueList Filter_down_Actor1(fsm*, Activity& activity, const std::string& state,
		const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_down_Actor1");
	valList.push_front(state);
	return valList;
}
//forward_FromTo_strafe_left
void forward_FromTo_strafe_left_Actor1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("forward_FromTo_strafe_left_Actor1");
}
//forward_FromTo_strafe_right
void forward_FromTo_strafe_right_Actor1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("forward_FromTo_strafe_right_Actor1");
}

///NPC1 related
//forward
void Enter_forward_NPC1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_forward_NPC1");
	SMARTPTR(Object) npc1 = activity.getOwnerObject();
	SMARTPTR(Model) npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	npc1Model->animations().loop("eve/eve-walk", false);
}
void Exit_forward_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_forward_NPC1");
	SMARTPTR(Object) npc1 = activity.getOwnerObject();
	SMARTPTR(Model) npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	npc1Model->animations().stop("eve/eve-walk");
}
ValueList Filter_forward_NPC1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_forward_NPC1");
	valList.push_front(state);
	return valList;
}
//backward
void Enter_backward_NPC1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_backward_NPC1");
}
void Exit_backward_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_backward_NPC1");
}
ValueList Filter_backward_NPC1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_backward_NPC1");
	valList.push_front(state);
	return valList;
}
//strafe_left
void Enter_strafe_left_NPC1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("Enter_strafe_left_NPC1");
}
void Exit_strafe_left_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_strafe_left_NPC1");
}
ValueList Filter_strafe_left_NPC1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_strafe_left_NPC1");
	valList.push_front(state);
	return valList;
}
//strafe_right
void Enter_strafe_right_NPC1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("Enter_strafe_right_NPC1");
}
void Exit_strafe_right_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_strafe_right_NPC1");
}
ValueList Filter_strafe_right_NPC1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_strafe_right_NPC1");
	valList.push_front(state);
	return valList;
}
//roll_left
void Enter_roll_left_NPC1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_roll_left_NPC1");
}
void Exit_roll_left_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_roll_left_NPC1");
}
ValueList Filter_roll_left_NPC1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_roll_left_NPC1");
	valList.push_front(state);
	return valList;
}
//roll_right
void Enter_roll_right_NPC1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_roll_right_NPC1");
}
void Exit_roll_right_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_roll_right_NPC1");
}
ValueList Filter_roll_right_NPC1(fsm*, Activity& activity,
		const std::string& state, const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_roll_right_NPC1");
	valList.push_front(state);
	return valList;
}
//jump
void Enter_jump_NPC1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_jump_NPC1");
}
void Exit_jump_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_jump_NPC1");
}
ValueList Filter_jump_NPC1(fsm*, Activity& activity, const std::string& state,
		const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_jump_NPC1");
	valList.push_front(state);
	return valList;
}
//idle
void Enter_idle_NPC1(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_idle_NPC1");
	SMARTPTR(Object) npc1 = activity.getOwnerObject();
	SMARTPTR(Model) npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	npc1Model->animations().stop_all();
}
void Exit_idle_NPC1(fsm*, Activity& activity)
{
	PRINT("Exit_idle_NPC1");
}
ValueList Filter_idle_NPC1(fsm*, Activity& activity, const std::string& state,
		const ValueList& valueList)
{
	ValueList valList = valueList;
	PRINT("Filter_idle_NPC1");
	valList.push_front(state);
	return valList;
}
//forward_FromTo_roll_left
void forward_FromTo_roll_left_NPC1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("forward_FromTo_roll_left_NPC1");
}
//forward_FromTo_roll_right
void forward_FromTo_roll_right_NPC1(fsm*, Activity& activity,
		const ValueList& valueList)
{
	PRINT("forward_FromTo_roll_right_NPC1");
}
