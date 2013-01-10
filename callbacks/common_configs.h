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
 * \file /Ely/callbacks/common_configs.h
 *
 * \date 26/nov/2012 (10:18:23)
 * \author marco
 */

#ifndef COMMON_CONFIGS_H_
#define COMMON_CONFIGS_H_

#include <iostream>
#include <utility>
#include <map>
#include <string>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

__attribute__((constructor)) void callbacksInit();
void callAllInits();

__attribute__((destructor)) void callbacksEnd();
void callAllEnds();

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

///Common declarations
typedef std::pair<std::string, std::string> StateEventType;
typedef std::string NextState;
typedef std::pair<StateEventType, NextState> TransitionTableItem;
typedef std::map<StateEventType, NextState> TransitionTable;

#define TABLEINSERT(tablePtr,state,eventType,nextState) \
	(*tablePtr).insert(TransitionTableItem\
			(StateEventType(state, eventType), NextState(nextState)))

#endif /* COMMON_CONFIGS_H_ */
