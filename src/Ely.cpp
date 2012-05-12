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
	// Open the framework
	GameManager* gameApp = new GameManager(argc, argv);
	// Set your application up
	gameApp->setup();
	// Do the main loop
	gameApp->main_loop();
	// Close the framework
	delete gameApp;
	return 0;
}

