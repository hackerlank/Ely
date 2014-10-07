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
 * \file /Ely/elygame/transitions/common_configs.cpp
 *
 * \date 26/nov/2012 (11:29:26)
 * \author consultit
 */

#include "common_configs.h"

void transitionsInit()
{
	PRINT_DEBUG("Executing transitionsInit");
	callAllInits();
}

void transitionsEnd()
{
	PRINT_DEBUG("Executing transitionsEnd");
	callAllEnds();
}

///Insert declarations of all init/end functions
extern void Actor_trnsInit();
extern void Actor_trnsEnd();
extern void Character_trnsInit();
extern void Character_trnsEnd();

///Call all init/end functions
void callAllInits()
{
	Actor_trnsInit();
	Character_trnsInit();
}
void callAllEnds()
{
	Actor_trnsEnd();
	Character_trnsEnd();
}

