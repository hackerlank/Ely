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
 * \file /Ely/elygame/initializations/common_configs.cpp
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
extern void Actor_initInit();
extern void Actor_initEnd();
extern void Camera_initInit();
extern void Camera_initEnd();
extern void Game_initInit();
extern void Game_initEnd();
extern void StaticObjects_initInit();
extern void StaticObjects_initEnd();
extern void Character_initInit();
extern void Character_initEnd();
extern void SoftObject_initInit();
extern void SoftObject_initEnd();
extern void OpenSteerPlugIn_initInit();
extern void OpenSteerPlugIn_initEnd();
extern void RecastNavMesh_initInit();
extern void RecastNavMesh_initEnd();

///Call all init/end functions
void callAllInits()
{
	Actor_initInit();
	Camera_initInit();
	Game_initInit();
	StaticObjects_initInit();
	Character_initInit();
	SoftObject_initInit();
	OpenSteerPlugIn_initInit();
	RecastNavMesh_initInit();
}
void callAllEnds()
{
	Actor_initEnd();
	Camera_initEnd();
	Game_initEnd();
	StaticObjects_initEnd();
	Character_initEnd();
	SoftObject_initEnd();
	OpenSteerPlugIn_initEnd();
	RecastNavMesh_initEnd();
}

