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

//ENTER Enter_<STATE>_<OBJECTID>;
//EXIT Exit_<STATE>_<OBJECTID>;
//FILTER Filter_<STATE>_<OBJECTID>;
//FROMTO <STATEA>_FromTo_<STATEB>_<OBJECTID>;

#ifdef __cplusplus
extern "C"
{
#endif

///Actor1 related
//forward
ENTER Enter_forward_Actor1;
EXIT Exit_forward_Actor1;
FILTER Filter_forward_Actor1;
//backward
ENTER Enter_backward_Actor1;
EXIT Exit_backward_Actor1;
FILTER Filter_backward_Actor1;
//strafe_left
ENTER Enter_strafe_left_Actor1;
EXIT Exit_strafe_left_Actor1;
FILTER Filter_strafe_left_Actor1;
//strafe_right
ENTER Enter_strafe_right_Actor1;
EXIT Exit_strafe_right_Actor1;
FILTER Filter_strafe_right_Actor1;
//up
ENTER Enter_up_Actor1;
EXIT Exit_up_Actor1;
FILTER Filter_up_Actor1;
//down
ENTER Enter_down_Actor1;
EXIT Exit_down_Actor1;
FILTER Filter_down_Actor1;
//forward_FromTo_strafe_left
FROMTO forward_FromTo_strafe_left_Actor1;
//forward_FromTo_strafe_right
FROMTO forward_FromTo_strafe_right_Actor1;

#ifdef __cplusplus
}
#endif

#endif /* TRANSITION_DEFS_H_ */
