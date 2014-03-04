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
 * \file /Ely/ely/initializations/defs/ely.cpp
 *
 * \date 11/gen/2014 (11:22:20)
 * \author consultit
 */

#include "../common_configs.h"
#include "Game/GamePhysicsManager.h"
#include "Support/Raycaster.h"
#include <rocketRegion.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

///ely related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION elyPreObjects_initialization;
INITIALIZATION elyPostObjects_initialization;

#ifdef __cplusplus
}
#endif

namespace
{
const int CALLBACKSNUM = 5;
std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");
void LoadFonts(const char* directory);
}  // namespace
Rocket::Core::Context *gRocketContext;

void elyPreObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//create the global ray caster
	new Raycaster(pandaFramework, windowFramework, GamePhysicsManager::GetSingleton().bulletWorld(), CALLBACKSNUM);

	//initialize libRocket: one region for the main window
	LoadFonts((baseDir + "data/misc/").c_str());

	SMARTPTR(RocketRegion)region = RocketRegion::make("elyRocket", windowFramework->get_graphics_window());
	region->set_active(true);

	SMARTPTR(RocketInputHandler)inputHandler = new RocketInputHandler();
	windowFramework->get_mouse().attach_new_node(inputHandler);
	region->set_input_handler(inputHandler);

	gRocketContext = region->get_context();

	Rocket::Controls::Initialise();

#ifdef ELY_DEBUG
	Rocket::Debugger::Initialise(gRocketContext);
	Rocket::Debugger::SetVisible(true);
#endif
}

void elyPostObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
}

namespace
{
// Loads libRocket fonts from the given path.
void LoadFonts(const char* directory)
{
	Rocket::Core::String font_names[4];
	font_names[0] = "Delicious-Roman.otf";
	font_names[1] = "Delicious-Italic.otf";
	font_names[2] = "Delicious-Bold.otf";
	font_names[3] = "Delicious-BoldItalic.otf";

	for (unsigned int i = 0;
			i < sizeof(font_names) / sizeof(Rocket::Core::String); i++)
	{
		Rocket::Core::FontDatabase::LoadFontFace(
				Rocket::Core::String(directory) + font_names[i]);
	}
}
}

void elyGameInit()
{
}

void elyGameEnd()
{
	//delete the global ray caster
	delete Raycaster::GetSingletonPtr();
}
