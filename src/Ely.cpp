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
 * \file /Ely/src/Ely.cpp
 *
 * \date Nov 7, 2011
 * \author marco
 */

#include "Ely.h"

int main(int argc, char **argv)
{
	// Load your configuration
	load_prc_file("config.prc");
	// Add code defined configuration
	load_prc_file_data("", "sync-video #t");
	//load_prc_file_data("", "want-directtools #t");
	//load_prc_file_data("", "want-tk #t");

	// Setup the game framework
	// ComponentTemplate manager
	ComponentTemplateManager* componentTmplMgr = new ComponentTemplateManager();
	// ObjectTemplate manager
	ObjectTemplateManager* objectTmplMgr = new ObjectTemplateManager();
	// First create a Game manager: mandatory
	GameManager* gameMgr = new GameManager(argc, argv);
	// Other managers (depending on GameManager)
	GameAudioManager* gameAudioMgr = new GameAudioManager();
	GameInputManager* gameInputMgr = new GameInputManager();
	GamePhysicsManager* gamePhysicsMgr = new GamePhysicsManager();
#ifdef DEBUG
	if (Thread::is_threading_supported())
	{
		std::cout << "Threading support has been compiled in and enabled"
				<< std::endl;
	}
	if (Thread::is_simple_threads())
	{
		std::cout << "Panda is currently compiled for \"simple threads\", "
				"which is to say, cooperative context switching only"
				<< std::endl;
	}
	if (Thread::is_true_threads())
	{
		std::cout << "A real threading library is available that "
				"supports actual OS-implemented threads" << std::endl;
	}
#endif
	// Set the game up
	gameMgr->initialize();
	// Do the main loop
	gameMgr->main_loop();

	// Close the game framework
	delete gamePhysicsMgr;
	delete gameInputMgr;
	delete gameAudioMgr;
	delete gameMgr;
	delete objectTmplMgr;
	delete componentTmplMgr;
	return 0;
}

