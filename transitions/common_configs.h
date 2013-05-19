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
 * \file /Ely/transitions/common_configs.h
 *
 * \date 26/nov/2012 (11:29:07)
 * \author consultit
 */

#ifndef COMMON_CONFIGS_H_
#define COMMON_CONFIGS_H_

#include <iostream>
#include <string>
#include "BehaviorComponents/Activity.h"
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

__attribute__((constructor)) void transitionsInit();
void callAllInits();

__attribute__((destructor)) void transitionsEnd();
void callAllEnds();

//generic typedefs
typedef void ENTER(fsm*, Activity&, const ValueList&);
typedef void EXIT(fsm*, Activity&);
typedef ValueList FILTER(fsm*, Activity&, const std::string&,
		const ValueList&);
typedef void FROMTO(fsm*, Activity&, const ValueList&);

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

///Common declarations

#endif /* COMMON_CONFIGS_H_ */
