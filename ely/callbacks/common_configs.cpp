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
 * \file /Ely/callbacks/common_configs.cpp
 *
 * \date 26/nov/2012 (10:18:48)
 * \author consultit
 */

#include "common_configs.h"
#include "ObjectModel/Object.h"

using namespace ely;

void callbacksInit()
{
	PRINT_DEBUG("Executing callbacksInit");
	callAllInits();
}

void callbacksEnd()
{
	PRINT_DEBUG("Executing callbacksEnd");
	callAllEnds();
}

void default_callback__(const Event* event, void* data)
{
#ifdef ELY_DEBUG
	//get data
	SMARTPTR(Component) component = (Component*) data;
	std::string evtStr = event->get_name();
	std::string compTypeStr = std::string(component->componentType());
	std::string objIdStr = std::string(component->getOwnerObject()->objectId());
	//show message
	std::cout << "Default Callback called: [EVENT: '" << evtStr
			<< "'][COMPONENT: '" << compTypeStr << "'][OBJECT: '" << objIdStr
			<< "']" << std::endl;
#endif
}

///Insert declarations of all init/end functions
extern void activityCharacterInit();
extern void activityCharacterEnd();
extern void activityActorInit();
extern void activityActorEnd();
extern void controlCameraInit();
extern void controlCameraEnd();

///Call all init/end functions
void callAllInits()
{
	activityCharacterInit();
	activityActorInit();
	controlCameraInit();
}
void callAllEnds()
{
	activityCharacterEnd();
	activityActorEnd();
	controlCameraEnd();
}
