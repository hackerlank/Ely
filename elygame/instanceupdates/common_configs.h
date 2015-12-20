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
 * \file /Ely/elygame/instanceupdates/common_configs.h
 *
 * \date 2013-09-01 
 * \author consultit
 */

#ifndef INSTANCEUPDATES_CONFIGS_H_
#define INSTANCEUPDATES_CONFIGS_H_

#include "BehaviorComponents/Activity.h"

using namespace ely;

__attribute__((constructor)) void instanceupdatesInit();
void callAllInits();

__attribute__((destructor)) void instanceupdatesEnd();
void callAllEnds();

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

///Common declarations
typedef void INSTANCEUPDATE(float, Activity&);

#endif /* INSTANCEUPDATES_CONFIGS_H_ */
