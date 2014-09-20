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
 * \file /Ely/ely/transitions/defs/Character_trns.cpp
 *
 * \date 26/nov/2012 (12:02:39)
 * \author consultit
 */

#include "../common_configs.h"
#include "PhysicsControlComponents/CharacterController.h"
#include "SceneComponents/Model.h"

//ENTER Enter_<STATE>_<OBJECTTYPE>;
//EXIT Exit_<STATE>_<OBJECTTYPE>;
//FILTER Filter_<STATE>_<OBJECTTYPE>;
//FROMTO <STATEA>_FromTo_<STATEB>_<OBJECTTYPE>;

#ifdef __cplusplus
extern "C"
{
#endif

///Character (Activity) related
//I
ENTER Enter_I_Character;
EXIT Exit_I_Character;
//F
ENTER Enter_F_Character;
EXIT Exit_F_Character;
//B
ENTER Enter_B_Character;
EXIT Exit_B_Character;
//Sr
ENTER Enter_Sr_Character;
EXIT Exit_Sr_Character;
//Sl
ENTER Enter_Sl_Character;
EXIT Exit_Sl_Character;
//Rr
ENTER Enter_Rr_Character;
EXIT Exit_Rr_Character;
//Rl
ENTER Enter_Rl_Character;
EXIT Exit_Rl_Character;
//J
ENTER Enter_J_Character;
EXIT Exit_J_Character;
//F-Rr
ENTER Enter_F_Rr_Character;
EXIT Exit_F_Rr_Character;
//F-Rl
ENTER Enter_F_Rl_Character;
EXIT Exit_F_Rl_Character;
//F-Rr-Rl
ENTER Enter_F_Rr_Rl_Character;
EXIT Exit_F_Rr_Rl_Character;
//Rr-Rl
ENTER Enter_Rr_Rl_Character;
EXIT Exit_Rr_Rl_Character;
//F-J
ENTER Enter_F_J_Character;
EXIT Exit_F_J_Character;
//B-Rr
ENTER Enter_B_Rr_Character;
EXIT Exit_B_Rr_Character;
//B-Rl
ENTER Enter_B_Rl_Character;
EXIT Exit_B_Rl_Character;
//Sr-Rr
ENTER Enter_Sr_Rr_Character;
EXIT Exit_Sr_Rr_Character;
//Sr-Rl
ENTER Enter_Sr_Rl_Character;
EXIT Exit_Sr_Rl_Character;
//Sl-Rr
ENTER Enter_Sl_Rr_Character;
EXIT Exit_Sl_Rr_Character;
//Sl-Rl
ENTER Enter_Sl_Rl_Character;
EXIT Exit_Sl_Rl_Character;
//F-Q
ENTER Enter_F_Q_Character;
EXIT Exit_F_Q_Character;
//Sr-Q
ENTER Enter_Sr_Q_Character;
EXIT Exit_Sr_Q_Character;
//Sl-Q
ENTER Enter_Sl_Q_Character;
EXIT Exit_Sl_Q_Character;
//Rr-Q
ENTER Enter_Rr_Q_Character;
EXIT Exit_Rr_Q_Character;
//Rl-Q
ENTER Enter_Rl_Q_Character;
EXIT Exit_Rl_Q_Character;
//F-Rr-Q
ENTER Enter_F_Rr_Q_Character;
EXIT Exit_F_Rr_Q_Character;
//F-Rl-Q
ENTER Enter_F_Rl_Q_Character;
EXIT Exit_F_Rl_Q_Character;
//F-Rr-Rl-Q
ENTER Enter_F_Rr_Rl_Q_Character;
EXIT Exit_F_Rr_Rl_Q_Character;
//Rr-Rl-Q
ENTER Enter_Rr_Rl_Q_Character;
EXIT Exit_Rr_Rl_Q_Character;
//F-J-Q
ENTER Enter_F_J_Q_Character;
EXIT Exit_F_J_Q_Character;
//Sr-Rr-Q
ENTER Enter_Sr_Rr_Q_Character;
EXIT Exit_Sr_Rr_Q_Character;
//Sr-Rl-Q
ENTER Enter_Sr_Rl_Q_Character;
EXIT Exit_Sr_Rl_Q_Character;
//Sl-Rr-Q
ENTER Enter_Sl_Rr_Q_Character;
EXIT Exit_Sl_Rr_Q_Character;
//Sl-Rl-Q
ENTER Enter_Sl_Rl_Q_Character;
EXIT Exit_Sl_Rl_Q_Character;

#ifdef __cplusplus
}
#endif

///States' functions
//I
void Enter_I_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_I_Character");
}
void Exit_I_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_I_Character");
}
//F
void Enter_F_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("walk", false);
	npc1CharCtrl->enableForward(true);
}
void Exit_F_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("walk");
	npc1CharCtrl->enableForward(false);
}
//B
void Enter_B_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_B_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("walk", false);
	npc1CharCtrl->enableBackward(true);
}
void Exit_B_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_B_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("walk");
	npc1CharCtrl->enableBackward(false);
}
//Sr
void Enter_Sr_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeRight(true);
}
void Exit_Sr_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeRight(false);
}
//Sl
void Enter_Sl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeLeft(true);
}
void Exit_Sl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeLeft(false);
}
//Rr
void Enter_Rr_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_Rr_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableHeadRight(false);
}
//Rl
void Enter_Rl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_Rl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableHeadLeft(false);
}
//J
void Enter_J_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_J_Character");
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableJump(true);
}
void Exit_J_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_J_Character");
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableJump(false);
}
//F-Rr
void Enter_F_Rr_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("walk", false);
	npc1CharCtrl->enableForward(true);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_F_Rr_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("walk");
	npc1CharCtrl->enableForward(false);
	npc1CharCtrl->enableHeadRight(false);
}
//F-Rl
void Enter_F_Rl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("walk", false);
	npc1CharCtrl->enableForward(true);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_F_Rl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("walk");
	npc1CharCtrl->enableForward(false);
	npc1CharCtrl->enableHeadLeft(false);
}
//F-Rr-Rl
void Enter_F_Rr_Rl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Rr_Rl_Character");
	//
}
void Exit_F_Rr_Rl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Rr_Rl_Character");
	//
}
//Rr-Rl
void Enter_Rr_Rl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Rr_Rl_Character");
	//
}
void Exit_Rr_Rl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Rr_Rl_Character");
	//
}
//F-J
void Enter_F_J_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_J_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	//enable animation blending
	npc1Model->getPartBundle()->set_anim_blend_flag(true);
	npc1Model->getPartBundle()->set_control_effect(
			npc1Model->animations().find_anim("walk"), 0.5);
	npc1Model->getPartBundle()->set_control_effect(
			npc1Model->animations().find_anim("jump"), 0.5);
	npc1Model->animations().loop("walk", false);
	npc1Model->animations().loop("jump", false);
	npc1CharCtrl->enableForward(true);
	npc1CharCtrl->enableJump(true);
}
void Exit_F_J_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_J_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("jump");
	npc1Model->animations().stop("walk");
	//disable animation blending
	npc1Model->getPartBundle()->set_anim_blend_flag(false);
	npc1CharCtrl->enableForward(false);
	npc1CharCtrl->enableJump(false);
}
//B-Rr
void Enter_B_Rr_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_B_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("walk", false);
	npc1CharCtrl->enableBackward(true);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_B_Rr_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_B_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("walk");
	npc1CharCtrl->enableBackward(false);
	npc1CharCtrl->enableHeadRight(false);
}
//B-Rl
void Enter_B_Rl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_B_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("walk", false);
	npc1CharCtrl->enableBackward(true);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_B_Rl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_B_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("walk");
	npc1CharCtrl->enableBackward(false);
	npc1CharCtrl->enableHeadLeft(false);
}
//Sr-Rr
void Enter_Sr_Rr_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sr_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeRight(true);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_Sr_Rr_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sr_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeRight(false);
	npc1CharCtrl->enableHeadRight(false);
}
//Sr-Rl
void Enter_Sr_Rl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sr_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeRight(true);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_Sr_Rl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sr_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeRight(false);
	npc1CharCtrl->enableHeadLeft(false);
}
//Sl-Rr
void Enter_Sl_Rr_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sl_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeLeft(true);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_Sl_Rr_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sl_Rr_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeLeft(false);
	npc1CharCtrl->enableHeadRight(false);
}
//Sl-Rl
void Enter_Sl_Rl_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sl_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeLeft(true);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_Sl_Rl_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sl_Rl_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeLeft(false);
	npc1CharCtrl->enableHeadLeft(false);
}
static float linearSpeedFactor = 3.0;
static float angularSpeedFactor = 3.0;
//F-Q
void Enter_F_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("run", false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->enableForward(true);
}
void Exit_F_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("run");
	npc1CharCtrl->enableForward(false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
}
//Sr-Q
void Enter_Sr_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->enableStrafeRight(true);
}
void Exit_Sr_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeRight(false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
}
//Sl-Q
void Enter_Sl_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->enableStrafeLeft(true);
}
void Exit_Sl_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableStrafeLeft(false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
}
//Rr-Q
void Enter_Rr_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_Rr_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableHeadRight(false);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
}
//Rl-Q
void Enter_Rl_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_Rl_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->enableHeadLeft(false);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
}
//F-Rr-Q
void Enter_F_Rr_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("run", false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableForward(true);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_F_Rr_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("run");
	npc1CharCtrl->enableForward(false);
	npc1CharCtrl->enableHeadRight(false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
}
//F-Rl-Q
void Enter_F_Rl_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().loop("run", false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableForward(true);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_F_Rl_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("run");
	npc1CharCtrl->enableForward(false);
	npc1CharCtrl->enableHeadLeft(false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
}
//F-Rr-Rl-Q
void Enter_F_Rr_Rl_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_Rr_Rl_Q_Character");
	//
}
void Exit_F_Rr_Rl_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_Rr_Rl_Q_Character");
	//
}
//Rr-Rl-Q
void Enter_Rr_Rl_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Rr_Rl_Q_Character");
	//
}
void Exit_Rr_Rl_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Rr_Rl_Q_Character");
	//
}
//F-J-Q
void Enter_F_J_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_F_J_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	//enable animation blending
	npc1Model->getPartBundle()->set_anim_blend_flag(true);
	npc1Model->getPartBundle()->set_control_effect(
			npc1Model->animations().find_anim("run"), 0.5);
	npc1Model->getPartBundle()->set_control_effect(
			npc1Model->animations().find_anim("jump"), 0.5);
	npc1Model->animations().loop("run", false);
	npc1Model->animations().loop("jump", false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->enableForward(true);
	npc1CharCtrl->enableJump(true);
}
void Exit_F_J_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_F_J_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1Model->animations().stop("jump");
	npc1Model->animations().stop("run");
	//disable animation blending
	npc1Model->getPartBundle()->set_anim_blend_flag(false);
	npc1CharCtrl->enableForward(false);
	npc1CharCtrl->enableJump(false);
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
}
//Sr-Rr-Q
void Enter_Sr_Rr_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sr_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableStrafeRight(true);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_Sr_Rr_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sr_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
	npc1CharCtrl->enableStrafeRight(false);
	npc1CharCtrl->enableHeadRight(false);
}
//Sr-Rl-Q
void Enter_Sr_Rl_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sr_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableStrafeRight(true);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_Sr_Rl_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sr_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
	npc1CharCtrl->enableStrafeRight(false);
	npc1CharCtrl->enableHeadLeft(false);
}
//Sl-Rr-Q
void Enter_Sl_Rr_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sl_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableStrafeLeft(true);
	npc1CharCtrl->enableHeadRight(true);
}
void Exit_Sl_Rr_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sl_Rr_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
	npc1CharCtrl->enableStrafeLeft(false);
	npc1CharCtrl->enableHeadRight(false);
}
//Sl-Rl-Q
void Enter_Sl_Rl_Q_Character(fsm*, Activity& activity, const ValueList& valueList)
{
	PRINT_DEBUG("Enter_Sl_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() * linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() * angularSpeedFactor);
	npc1CharCtrl->enableStrafeLeft(true);
	npc1CharCtrl->enableHeadLeft(true);
}
void Exit_Sl_Rl_Q_Character(fsm*, Activity& activity)
{
	PRINT_DEBUG("Exit_Sl_Rl_Q_Character");
	//
	SMARTPTR(Object)npc1 = activity.getOwnerObject();
	SMARTPTR(Model)npc1Model = DCAST(Model, npc1->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(CharacterController)npc1CharCtrl = DCAST (CharacterController,
			npc1->getComponent(ComponentFamilyType("PhysicsControl")));
	//
	npc1CharCtrl->setLinearSpeed(npc1CharCtrl->getLinearSpeed() / linearSpeedFactor);
	npc1CharCtrl->setAngularSpeed(npc1CharCtrl->getAngularSpeed() / angularSpeedFactor);
	npc1CharCtrl->enableStrafeLeft(false);
	npc1CharCtrl->enableHeadLeft(false);
}

///Init/end functions: see common_configs.cpp
void Character_trnsInit()
{
}
void Character_trnsEnd()
{
}
