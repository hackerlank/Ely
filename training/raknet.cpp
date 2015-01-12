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
#include <string>
#include <getopt.h>
#include <config.h>
// RakNet Headers
#include <raknet/MessageIdentifiers.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetTypes.h>

using namespace std;

static string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/elygame/");
#define COPYRIGHT_LICENSE \
"Copyright (C) 2015 Consultit.\n" \
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> \n" \
"This is free software: you are free to change and redistribute it. \n" \
"There is NO WARRANTY, to the extent permitted by law."

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
		CLIENT = 1, SERVER = 2, PEER = 3
	};
	int peerType = (int) CLIENT;
	string serverIP = "127.0.0.1";
	unsigned short int serverPort = 20000;
	unsigned int maxConnectionsAllowed = 2;
	unsigned short int maxPlayersPerServer = 2;
	// get options and parameters
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
		// --version, --help (from GNU Coding Standards)
				{ "version", no_argument, 0, 1000 },
				{ "help", no_argument, 0, 1001 },
				// These options set a flag.
				{ "client", no_argument, &peerType, (int) CLIENT },
				{ "server", no_argument, &peerType, (int) SERVER },
				{ "peer", no_argument, &peerType, (int) PEER },
				// These options don't set a flag.
				// We distinguish them by their indices.
				{ "address", required_argument, 0, 'a' },
				{ "port", required_argument, 0, 'p' },
				{ "max-connections-allowed", required_argument, 0, 1002 },
				{ "max-players-per-server", required_argument, 0, 1003 },
				{ 0, 0, 0, 0 } };
		// getopt_long stores the option index here.
		int option_index = 0;
		c = getopt_long(argc, argv, "a:p:", long_options, &option_index);

		// Detect the end of the options.
		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			// If option.flag != 0 then *option.flag = option.val
			// and getopt_long returns 0.
			if (long_options[option_index].flag != 0)
			{
				break;
			}
			// If option.flag == 0 then getopt_long returns option.val.
			break;
		case 1000:
			//--version
			cout << endl;
			cout << PACKAGE_NAME << " " << PACKAGE_VERSION << endl;
			cout << COPYRIGHT_LICENSE << endl;
			exit(0);
		case 1001:
		{
			//--help
			cout << endl;
			cout << argv[0] << " tests some feature to be used in "
					<< PACKAGE_NAME << "." << endl;
			cout << "Options: \n" << endl;
			option* opt = &long_options[0];
			while (opt->name)
			{
				string val, vall;
				if (opt->has_arg == required_argument)
				{
					val = " VALUE";
					vall = "=VALUE";
				}
				else if (opt->has_arg == optional_argument)
				{
					val = " [VALUE]";
					vall = "[=VALUE]";
				}
				else
				{
					val = vall = "";
				}
				// print short if is alpha
				if (isalpha(opt->val))
				{
					cout << "\t-" << (char) opt->val << val << endl;
				}
				// print long
				cout << "\t--" << opt->name << vall << endl;
				//
				cout << endl;
				++opt;
			}
		}
			exit(0);
		case 'a':
			//--address
			serverIP = string(optarg);
			break;
		case 'p':
			//--port
			serverPort = (unsigned short int) stoi(string(optarg));
			break;
		case 1002:
			//--max-connections-allowed
			maxConnectionsAllowed = (unsigned int) stoi(string(optarg));
			break;
		case 1003:
			//--max-players-per-server
			maxPlayersPerServer = (unsigned short int) stoi(string(optarg));
			break;
		case '?':
			// getopt_long already printed an error message.
			break;
		default:
			abort();
		}
	}
	// Print any remaining command line arguments (not options).
	if (optind < argc)
	{
		cout << "non-option arguments: ";
		while (optind < argc)
		{
			cout << argv[optind++];
		}
		cout << endl;
	}
	// Instancing
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	//connect
	switch (peerType)
	{
	// Connection as Client
	case CLIENT:
	{
		RakNet::SocketDescriptor clientSD = RakNet::SocketDescriptor();
		peer->Startup(1, &clientSD, 1);
		peer->Connect(serverIP.c_str(), serverPort, 0, 0);
	}
		break;
		// Connection as Server
	case SERVER:
	{
		RakNet::SocketDescriptor serverSD(serverPort, 0);
		peer->Startup(maxConnectionsAllowed, &serverSD, 1);
		peer->SetMaximumIncomingConnections(maxPlayersPerServer);
	}
		break;
		// Peer to peer connections
	case PEER:
	{
		RakNet::SocketDescriptor serverSD(serverPort, 0);
		peer->Startup(maxConnectionsAllowed, &serverSD, 1);
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

