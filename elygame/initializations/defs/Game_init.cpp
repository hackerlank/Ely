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
 * \file /Ely/elygame/initializations/defs/Game_init.cpp
 *
 * \date 2014-01-11 
 * \author consultit
 */

#include "../common_configs.h"
#include "Game/GamePhysicsManager.h"
#include "Support/Raycaster.h"
#include "Game_init.h"
#include "../../elygame.h"
#include "Game/GameGUIManager.h"

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

//globals
std::string rocketBaseDir(ELY_DATADIR);
//common text writing
void writeText(NodePath& textNode, const std::string& text, float scale, const LVecBase4& color,
		const LVector3f& location)
{
	textNode = NodePath(new TextNode("CommonTextNode"));
	textNode.reparent_to(
			ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(
					ObjectId("render2d"))->getNodePath());
	textNode.set_bin("fixed", 50);
	textNode.set_depth_write(false);
	textNode.set_depth_test(false);
	textNode.set_billboard_point_eye();
	DCAST(TextNode, textNode.node())->set_text(text);
	textNode.set_scale(scale);
	textNode.set_color(color);
	textNode.set_pos(location);
}

//locals
namespace
{
const int CALLBACKSNUM = 5;

void showMainMenu(const Event* e)
{
	GameGUIManager::GetSingletonPtr()->showMainMenu();
}

void showExitMenu(const Event* e)
{
	GameGUIManager::GetSingletonPtr()->showExitMenu();
}

//event handler added to the main one
void rocketEventHandler(const Rocket::Core::String& value,
		Rocket::Core::Event& event)
{
	if (value == "main::body::load_logo")
	{
		PRINT_DEBUG("main::body::load_logo");
	}
}

}  // namespace

void elyPreObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//create the global ray caster
	new Raycaster(pandaFramework, windowFramework, GamePhysicsManager::GetSingleton().bulletWorld(), CALLBACKSNUM);

	//register the add element function to gui (Rocket) main menu
	//register the event handler to gui main menu for each event value
	GameGUIManager::GetSingletonPtr()->gGuiEventHandlers["main::body::load_logo"] = &rocketEventHandler;
	//register the preset function to gui main menu
	//register the commit function to gui main menu
}

void elyPostObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	///Gui (libRocket)
	//add show main menu event handler
	EventHandler::get_global_event_handler()->add_hook("m", &showMainMenu);
	//handle "close request" and "esc" events
	windowFramework->get_graphics_window()->set_close_request_event("close_request_event");
	EventHandler::get_global_event_handler()->add_hook("close_request_event", &showExitMenu);
	EventHandler::get_global_event_handler()->add_hook("escape", &showExitMenu);
}

void Game_initInit()
{
}

void Game_initEnd()
{
	//delete the global ray caster
	delete Raycaster::GetSingletonPtr();
}
