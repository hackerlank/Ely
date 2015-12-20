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
 * \file /Ely/elygame/transitions/common_configs.h
 *
 * \date 2012-11-26 
 * \author consultit
 */

#ifndef COMMON_CONFIGS_H_
#define COMMON_CONFIGS_H_

#include "BehaviorComponents/Activity.h"

using namespace ely;

__attribute__((constructor)) void transitionsInit();
void callAllInits();

__attribute__((destructor)) void transitionsEnd();
void callAllEnds();

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

///Common declarations
typedef void ENTER(fsm*, Activity&, const ValueList&);
typedef void EXIT(fsm*, Activity&);
typedef ValueList FILTER(fsm*, Activity&, const std::string&,
		const ValueList&);
typedef void FROMTO(fsm*, Activity&, const ValueList&);

#endif /* COMMON_CONFIGS_H_ */
