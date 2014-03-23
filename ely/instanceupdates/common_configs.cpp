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
 * \file /Ely/ely/transitions/common_configs.cpp
 *
 * \date 26/nov/2012 (11:29:26)
 * \author consultit
 */

#include "common_configs.h"

void instanceupdatesInit()
{
	PRINT_DEBUG("Executing instanceupdatesInit");
	callAllInits();
}

void instanceupdatesEnd()
{
	PRINT_DEBUG("Executing instanceupdatesEnd");
	callAllEnds();
}

///Insert declarations of all init/end functions
extern void Character_updtInit();
extern void Character_updtEnd();

///Call all init/end functions
void callAllInits()
{
	Character_updtInit();
}
void callAllEnds()
{
	Character_updtEnd();
}

