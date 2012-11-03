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
 * \file /Ely/initializations/callback_defs.h
 *
 * \date 05/set/2012 (20:07:54)
 * \author marco
 */

#ifndef TRANSITION_DEFS_H_
#define TRANSITION_DEFS_H_

#include "all_configs.h"

#ifdef __cplusplus
extern "C"
{
#endif

///camera related
INITIALIZATION camera_initialization;

///Actor1 related
INITIALIZATION Actor1_initialization;

///Plane1 related
INITIALIZATION Plane1_initialization;

///Terrain1 related
INITIALIZATION Terrain1_initialization;

///NPC1 related
INITIALIZATION NPC1_initialization;

#ifdef __cplusplus
}
#endif

#endif /* TRANSITION_DEFS_H_ */
