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
 * \file /Ely/ely/initializations/common_configs.cpp
 *
 * \date 26/nov/2012 (12:16:12)
 * \author consultit
 */

#include "common_configs.h"

void initializationsInit()
{
	PRINT_DEBUG("Executing initializationsInit");
	callAllInits();
}

void initializationsEnd()
{
	PRINT_DEBUG("Executing initializationsEnd");
	callAllEnds();
}

///Insert declarations of all init/end functions
extern void cameraInit();
extern void cameraEnd();
extern void Actor1Init();
extern void Actor1End();
extern void Plane1Init();
extern void Plane1End();
extern void Terrain1Init();
extern void Terrain1End();
extern void player0Init();
extern void player0End();
extern void Steerer1Init();
extern void Steerer1End();
extern void course2Init();
extern void course2End();

///Call all init/end functions
void callAllInits()
{
	cameraInit();
	Actor1Init();
	Plane1Init();
	Terrain1Init();
	player0Init();
	Steerer1Init();
	course2Init();
}
void callAllEnds()
{
	cameraEnd();
	Actor1End();
	Plane1End();
	Terrain1End();
	player0End();
	Steerer1End();
	course2End();
}

