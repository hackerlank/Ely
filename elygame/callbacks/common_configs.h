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
 * \file /Ely/elygame/callbacks/common_configs.h
 *
 * \date 2012-11-26 
 * \author consultit
 */

#ifndef COMMON_CONFIGS_H_
#define COMMON_CONFIGS_H_

#include <eventHandler.h>

__attribute__((constructor)) void callbacksInit();
void callAllInits();

__attribute__((destructor)) void callbacksEnd();
void callAllEnds();

//generic typedefs
typedef EventHandler::EventCallbackFunction CALLBACK;

#ifdef __cplusplus
extern "C"
{
#endif

//default callback
CALLBACK default_callback__;

#ifdef __cplusplus
}
#endif

#endif /* COMMON_CONFIGS_H_ */
