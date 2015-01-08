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
 * \file /Ely/training/manual_example.cpp
 *
 * \date 10/mar/2013 (15:39:11)
 * \author consultit
 */

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <geomLines.h>
#include <lpoint3.h>
#include <iostream>
#include <raknet/MessageIdentifiers.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetTypes.h>

using namespace std;

static string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/elygame/");

int raknet_main(int argc, char *argv[])
{
	// Load your configuration
	load_prc_file_data("", "model-path " + baseDir + "data/models");
	load_prc_file_data("", "model-path " + baseDir + "data/shaders");
	load_prc_file_data("", "model-path " + baseDir + "data/sounds");
	load_prc_file_data("", "model-path " + baseDir + "data/textures");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "lock-to-one-cpu 0");
	load_prc_file_data("", "support-threads 1");
	load_prc_file_data("", "audio-buffering-seconds 5");
	load_prc_file_data("", "audio-preload-threshold 2000000");
	load_prc_file_data("", "sync-video #t");
	//open a new window framework
	PandaFramework framework;
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	WindowFramework *window = framework.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}
	//setup camera trackball (local coordinate)
	NodePath tballnp = window->get_mouse().find("**/+Trackball");
	PT(Trackball)trackball = DCAST(Trackball, tballnp.node());
	trackball->set_pos(0, 500, 0);
	trackball->set_hpr(0, 15, 0);

	///here is room for your own code
	enum
	{
		CLIENT, SERVER
	} peerType = CLIENT;
	string serverIP = "127.0.0.1";
	unsigned short int serverPort = 20000;
	unsigned int maxConnectionsAllowed = 2;
	unsigned short int maxPlayersPerServer = 2;
	// get parameters
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "csa:p:m:n:")) != -1)
	{
		switch (c)
		{
		case 'c':
			peerType = CLIENT;
			break;
		case 's':
			peerType = SERVER;
			break;
		case 'a':
			serverIP = string(optarg);
			break;
		case 'p':
			serverPort = (unsigned short int) atoi(optarg);
			break;
		case 'm':
			maxConnectionsAllowed = (unsigned int) atoi(optarg);
			break;
		case 'n':
			maxPlayersPerServer = (unsigned short int) atoi(optarg);
			break;
		case '?':
			if ((optopt == 'a') or (optopt == 'p') or (optopt == 'm')
					or (optopt == 'n'))
				std::cerr << "Option " << optopt << " requires an argument.\n"
						<< std::endl;
			else if (isprint(optopt))
				std::cerr << "Unknown option " << optopt << std::endl;
			else
				std::cerr << "Unknown option character " << optopt << std::endl;
			return 1;
		default:
			abort();
		}
	}
	//
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	//connect
	switch (peerType)
	{
	case CLIENT:
	{
		RakNet::SocketDescriptor clientSocket = RakNet::SocketDescriptor();
		peer->Startup(1, &clientSocket, 1);
		peer->Connect(serverIP.c_str(), serverPort, 0, 0);
	}
		break;
	case SERVER:
	{
		RakNet::SocketDescriptor serverSocket(serverPort, 0);
		peer->Startup(maxConnectionsAllowed, &serverSocket, 1);
		peer->SetMaximumIncomingConnections(maxPlayersPerServer);
	}
		break;
	default:
		abort();
	}

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

