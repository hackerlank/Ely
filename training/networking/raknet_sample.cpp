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
#include <cstdlib>
// RakNet Headers
#include <raknet/MessageIdentifiers.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/RakNetTypes.h>
#include <raknet/BitStream.h>

using namespace std;

static string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/elygame/");
#define COPYRIGHT_LICENSE \
"Copyright (C) 2015 Consultit.\n" \
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> \n" \
"This is free software: you are free to change and redistribute it. \n" \
"There is NO WARRANTY, to the extent permitted by law."

enum PeerType
{
	CLIENT = 1, SERVER = 2, PEER = 3
};
RakNet::RakPeerInterface* peer;
AsyncTask::DoneStatus readMessagesFunction(GenericAsyncTask* task,
		void * peerType);
enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

int main(int argc, char *argv[])
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
	int peerType = (int) CLIENT;
	string serverIP = "127.0.0.1";
	unsigned short int serverPort = 60000;
	unsigned int maxConnectionsAllowed = 10;
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
//			serverPort = (unsigned short int) stoi(string(optarg)); //-std=c++11
			serverPort = (unsigned short int) strtol(string(optarg).c_str(), NULL, 0);
			break;
		case 1002:
			//--max-connections-allowed
//			maxConnectionsAllowed = (unsigned int) stoi(string(optarg)); //-std=c++11
			maxConnectionsAllowed = (unsigned int) strtol(string(optarg).c_str(), NULL, 0);
			break;
		case 1003:
			//--max-players-per-server
//			maxPlayersPerServer = (unsigned short int) stoi(string(optarg)); //-std=c++11
			maxPlayersPerServer = (unsigned short int) strtol(string(optarg).c_str(), NULL, 0);
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
	peer = RakNet::RakPeerInterface::GetInstance();
	// Connect
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

	// Set up a "read messages task" (synchronous, i.e. into the main task chain)
	PT(GenericAsyncTask)readMessagesTask = new GenericAsyncTask("readMessagesTask",
			&readMessagesFunction,
			reinterpret_cast<void*>(&peerType));
	readMessagesTask->set_sort(0);
	AsyncTaskManager::get_global_ptr()->add(readMessagesTask);

	//do the main loop, equal to run() in python
	framework.main_loop();

	// Cleaning Up
	RakNet::RakPeerInterface::DestroyInstance(peer);

	//close the window framework
	framework.close_framework();
	return (0);
}

AsyncTask::DoneStatus readMessagesFunction(GenericAsyncTask* task,
		void * pPeerType)
{
	PeerType peerType = *(reinterpret_cast<PeerType*>(pPeerType));

	RakNet::Packet* packet;
	// Cycle until packet are present; there should be a limitation
	// when this task is synchronous with the main panda task chain
	for (packet = peer->Receive(); packet;
			peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			cout << ID_REMOTE_DISCONNECTION_NOTIFICATION
					<< ": Another client has disconnected." << endl;
			break;
		case ID_REMOTE_CONNECTION_LOST:
			cout << ID_REMOTE_CONNECTION_LOST
					<< ": Another client has lost the connection." << endl;
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			cout << ID_REMOTE_NEW_INCOMING_CONNECTION
					<< ": Another client has connected." << endl;
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			cout << ID_CONNECTION_REQUEST_ACCEPTED
					<< ": Our connection request has been accepted." << endl;

			// Use a BitStream to write a custom user message
			// Bitstreams are easier to use than sending casted structures,
			// and handle endian swapping automatically
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID) ID_GAME_MESSAGE_1);
			bsOut.Write("Hello world");
			peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
					packet->systemAddress, false);
		}

			break;
		case ID_NEW_INCOMING_CONNECTION:
			cout << ID_NEW_INCOMING_CONNECTION << ": A connection is incoming."
					<< endl;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			cout << ID_NO_FREE_INCOMING_CONNECTIONS << ": The server is full."
					<< endl;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			cout << ID_DISCONNECTION_NOTIFICATION;
			if (peerType == SERVER)
			{
				cout << ": A client has disconnected." << endl;
			}
			else if (peerType == CLIENT)
			{
				cout << ": We have been disconnected." << endl;
			}
			break;
		case ID_CONNECTION_LOST:
			cout << ID_CONNECTION_LOST;
			if (peerType == SERVER)
			{
				cout << ": A client lost the connection." << endl;
			}
			else if (peerType == CLIENT)
			{
				cout << ": Connection lost." << endl;
			}
			break;
		case ID_GAME_MESSAGE_1:
		{
			RakNet::RakString rs;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rs);
			cout << rs.C_String() << endl;
		}
			break;
		default:
			cout << "Message with identifier '" << packet->data[0]
					<< "' has arrived." << endl;
			break;
		}
	}
	//
	return AsyncTask::DS_cont;
}
