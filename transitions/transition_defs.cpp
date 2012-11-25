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

///Actor related
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

///Character related
//forward
void Enter_forward_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_forward_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().loop("eve/eve-walk", false);
	npc1CharCtrl->enableForward(true);
}
void Exit_forward_Character(fsm*, Activity& activity)
{
	PRINT("Exit_forward_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().stop("eve/eve-walk");
	npc1CharCtrl->enableForward(false);
}
//backward
void Enter_backward_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_backward_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().loop("eve/eve-walk", false);
	npc1CharCtrl->enableBackward(true);
}
void Exit_backward_Character(fsm*, Activity& activity)
{
	PRINT("Exit_backward_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().stop("eve/eve-walk");
	npc1CharCtrl->enableBackward(false);
}
//roll_left
void Enter_roll_left_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_roll_left_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1CharCtrl->enableRollLeft(true);
}
void Exit_roll_left_Character(fsm*, Activity& activity)
{
	PRINT("Exit_roll_left_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1CharCtrl->enableRollLeft(false);
}
//roll_right
void Enter_roll_right_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_roll_right_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1CharCtrl->enableRollRight(true);
}
void Exit_roll_right_Character(fsm*, Activity& activity)
{
	PRINT("Exit_roll_right_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1CharCtrl->enableRollRight(false);
}
//forward_roll_left
void Enter_forward_roll_left_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_forward_roll_left_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().loop("eve/eve-walk", false);
	npc1CharCtrl->enableRollLeft(true);
	npc1CharCtrl->enableForward(true);
}
void Exit_forward_roll_left_Character(fsm*, Activity& activity)
{
	PRINT("Exit_forward_roll_left_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().stop("eve/eve-walk");
	npc1CharCtrl->enableRollLeft(false);
	npc1CharCtrl->enableForward(false);
}
//forward_roll_right
void Enter_forward_roll_right_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_forward_roll_right_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().loop("eve/eve-walk", false);
	npc1CharCtrl->enableRollRight(true);
	npc1CharCtrl->enableForward(true);
}
void Exit_forward_roll_right_Character(fsm*, Activity& activity)
{
	PRINT("Exit_forward_roll_right_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent("Physics"));
	//
	npc1Model->animations().stop("eve/eve-walk");
	npc1CharCtrl->enableRollRight(false);
	npc1CharCtrl->enableForward(false);
}
//jump
void Enter_jump_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_jump_Character");
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	//
	npc1Model->animations().play("eve/eve-jump");
}
void Exit_jump_Character(fsm*, Activity& activity)
{
	PRINT("Exit_jump_Character");
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	//
	npc1Model->animations().stop("eve/eve-jump");
}
//idle
void Enter_idle_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT("Enter_idle_Character");
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	npc1Model->animations().stop_all();
}
void Exit_idle_Character(fsm*, Activity& activity)
{
	PRINT("Exit_idle_Character");
}
