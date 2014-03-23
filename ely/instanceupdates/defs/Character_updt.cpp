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
 * \file /Ely/ely/instanceupdates/defs/Character_updt.cpp
 *
 * \date 01/set/2013 (20:18:30)
 * \author consultit
 */

#include "../common_configs.h"
#include "PhysicsControlComponents/CharacterController.h"

///player related
#ifdef __cplusplus
extern "C"
{
#endif

INSTANCEUPDATE playerUpdate;

#ifdef __cplusplus
}
#endif

namespace
{
float timeElapsed = 0;
bool jumping = false;
const float TIMETHRESHOLD = 3.0; //seconds
}
///player custom update
void playerUpdate(float dt, Activity& activity)
{
//	PRINT_DEBUG(static_cast<fsm&>(activity).getCurrentOrNextState());

//	fsm& playerFSM = (fsm&) activity;
//	std::string currentState;
//	//every 3 seconds jump
//	if (timeElapsed > TIMETHRESHOLD)
//	{
//		//reset timer
//		timeElapsed = 0;
//		//enable jump
//		//get fsm
//		currentState = playerFSM.getCurrentOrNextState();
//		playerFSM.request("J");
//		jumping = true;
//	}
//	else if (jumping)
//	{
//		//disable jump
//		if (currentState.empty())
//		{
//			playerFSM.request("I");
//		}
//		else
//		{
//			playerFSM.request(currentState);
//		}
////		characterComp->enableJump(false);
//		jumping = false;
//	}
//	else
//	{
//		timeElapsed += dt;
//	}
}

///Init/end functions: see common_configs.cpp
void Character_updtInit()
{
}
void Character_updtEnd()
{
}




