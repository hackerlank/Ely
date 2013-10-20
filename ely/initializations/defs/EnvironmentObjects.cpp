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
 * \file /Ely/ely/initializations/defs/EnvironmentObjects.cpp
 *
 * \date 20/ott/2013 (10:00:47)
 * \author consultit
 */

#include "../common_configs.h"
#include "AIComponents/NavMesh.h"

///Environment objects related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Terrain1_initialization;
INITIALIZATION course2_initialization;

#ifdef __cplusplus
}
#endif

///Terrain1
void Terrain1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Terrain1
//	object->getNodePath().set_render_mode_wireframe(1);
}

///course2
#ifdef ELY_DEBUG
//debug flag
namespace
{
bool debugOn = false;

void toggleDebugNavMesh(const Event* event, void* data)
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

///init/end
void environmentObjectsInit()
{
}

void environmentObjectsEnd()
{
}
