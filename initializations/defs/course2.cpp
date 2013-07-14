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
 * \file /Ely/initializations/defs/course2.cpp
 *
 * \date 14/lug/2013 (08:49:04)
 * \author consultit
 */

#include "../common_configs.h"
#include "AIComponents/NavMesh.h"

///course2 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION course2_initialization;

#ifdef __cplusplus
}
#endif

#ifdef ELY_DEBUG
///debug flag
static bool debugOn = false;

static void toggleDebugNavMesh(const Event* event, void* data)
{
	SMARTPTR(Object)course2 = reinterpret_cast<Object*>(data);
	SMARTPTR(Component) compAI = course2->getComponent(
			ComponentFamilyType("AI"));
	if (compAI->is_of_type(NavMesh::get_class_type()))
	{
		SMARTPTR(NavMesh)course2NavMesh = DCAST(NavMesh, course2->getComponent(
						ComponentFamilyType("AI")));
		//toggle debug
		course2NavMesh->debug(not debugOn);
		//toggle flag
		debugOn = not debugOn;
	}
}
#endif

void course2_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//course2
#ifdef ELY_DEBUG
	//enable/disable navigation mesh debugging by event
	pandaFramework->define_key("m", "toggleDebugNavMesh", &toggleDebugNavMesh,
			static_cast<void*>(object));
#endif
}

void course2Init()
{
}

void course2End()
{
}




