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
 * \file /Ely/transitions/callback_defs.h
 *
 * \date 21/ago/2012 (09:18:56)
 * \author marco
 */

#ifndef TRANSITION_DEFS_H_
#define TRANSITION_DEFS_H_

#include "all_configs.h"

//ENTER Enter_<STATE>_<OBJECTTYPE>;
//EXIT Exit_<STATE>_<OBJECTTYPE>;
//FILTER Filter_<STATE>_<OBJECTTYPE>;
//FROMTO <STATEA>_FromTo_<STATEB>_<OBJECTTYPE>;

#ifdef __cplusplus
extern "C"
{
#endif

///Actor related
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

///Character related
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

#endif /* TRANSITION_DEFS_H_ */
