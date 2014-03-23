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
 * \file /Ely/ely/initializations/defs/Character_init.cpp
 *
 * \date 14/lug/2013 (08:48:01)
 * \author consultit
 */

#include "../common_configs.h"
#include "AudioComponents/Sound3d.h"
#include "BehaviorComponents/Activity.h"

///Player1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION player0_initialization;

#ifdef __cplusplus
}
#endif

void player0_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Player1
	fsm& player1FSM = (fsm&) (*DCAST(Activity, object->getComponent(
							ComponentFamilyType("Behavior"))));
	player1FSM.request("I");
	//play sound
	SMARTPTR(Sound3d) npc1Sound3d = DCAST(Sound3d, object->getComponent(
					ComponentFamilyType("Audio")));
	npc1Sound3d->getSound("walk-sound")->set_loop(true);
	npc1Sound3d->getSound("walk-sound")->play();
}

void Character_initInit()
{
}

void Character_initEnd()
{
}
