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
 * \file /Ely/callbacks/all_configs.h
 *
 * \date 20/ago/2012 (10:07:25)
 * \author marco
 */

#ifndef ALL_CONFIGS_H_
#define ALL_CONFIGS_H_

#include <iostream>
#include <string>
#include <ObjectModel/Component.h>
#include <ObjectModel/Object.h>
#include "Utilities/Tools.h"

__attribute__((constructor)) void callbacksInit();
__attribute__((destructor)) void callbacksEnd();

//generic typedefs
typedef EventHandler::EventCallbackFunction CALLBACK;
typedef std::string CALLBACKNAME;

//extern CALLBACKNAME EVENT_COMPONENTTYPE_OBJECTID;

#ifdef __cplusplus
extern "C"
{
#endif

//default callback
CALLBACK default_callback__;

#ifdef __cplusplus
}
#endif

#endif /* ALL_CONFIGS_H_ */
