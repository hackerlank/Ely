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
 * \file /Ely/transitions/defs/Character.cpp
 *
 * \date 26/nov/2012 (12:02:39)
 * \author marco
 */

#include "../common_configs.h"
#include "Utilities/ComponentSuite.h"

//ENTER Enter_<STATE>_<OBJECTTYPE>;
//EXIT Exit_<STATE>_<OBJECTTYPE>;
//FILTER Filter_<STATE>_<OBJECTTYPE>;
//FROMTO <STATEA>_FromTo_<STATEB>_<OBJECTTYPE>;

#ifdef __cplusplus
extern "C"
{
#endif

///Character (Activity) related
//forward
ENTER Enter_forward_Character;
EXIT Exit_forward_Character;
//backward
ENTER Enter_backward_Character;
EXIT Exit_backward_Character;
//roll_left
ENTER Enter_roll_left_Character;
EXIT Exit_roll_left_Character;
//roll_right
ENTER Enter_roll_right_Character;
EXIT Exit_roll_right_Character;
//forward_roll_left
ENTER Enter_forward_roll_left_Character;
EXIT Exit_forward_roll_left_Character;
//forward_roll_right
ENTER Enter_forward_roll_right_Character;
EXIT Exit_forward_roll_right_Character;
//jump
ENTER Enter_jump_Character;
EXIT Exit_jump_Character;
//idle
ENTER Enter_idle_Character;
EXIT Exit_idle_Character;

#ifdef __cplusplus
}
#endif

///States' functions
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

///Init/end functions: see common_configs.cpp
void characterInit()
{
}
void characterEnd()
{
}
