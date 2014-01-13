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
#include "AIComponents/SteerPlugIn.h"

///Environment objects related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Terrain1_initialization;

#ifdef __cplusplus
}
#endif

///Terrain1
#ifdef ELY_DEBUG
namespace
{
void toggleWireframeMode(const Event* event, void* data)
{
	SMARTPTR(Object)terrain1= reinterpret_cast<Object*>(data);

	if (terrain1->getNodePath().has_render_mode())
	{
		terrain1->getNodePath().clear_render_mode();
	}
	else
	{
		terrain1->getNodePath().set_render_mode_wireframe(1);
	}
}
}
#endif

void Terrain1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Terrain1
#ifdef ELY_DEBUG
	//set toggle Wire frame Mode callback
	pandaFramework->define_key("t", "toggleWireframeMode", &toggleWireframeMode,
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
