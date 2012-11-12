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
 * \file /Ely/training/network.cpp
 *
 * \date 11/nov/2012 10:48:06
 * \author marco
 */

#include <iostream>
#include <string>
#include <list>
#include <load_prc_file.h>
#include <pandaFramework.h>
#include <genericAsyncTask.h>
#include <asyncTaskManager.h>
#include <queuedConnectionManager.h>
#include <queuedConnectionListener.h>
#include <queuedConnectionReader.h>
#include <connectionWriter.h>
#include <connection.h>
#include <netAddress.h>
#include <netDatagram.h>
#include "Utilities/Tools.h"

class Server
{
	int cPort, cBacklog;
	QueuedConnectionManager* cManager;
	QueuedConnectionListener* cListener;
	QueuedConnectionReader* cReader;
	ConnectionWriter* cWriter;
	std::list<PT(Connection)> activeConnections;
	///@{
	///A task data for listener Polling.
	SMARTPTR(TaskInterface<Server>::TaskData) mListenerData;
	SMARTPTR(AsyncTask) mListenerTask;
	///@}
	///@{
	///A task data for reader Polling.
	SMARTPTR(TaskInterface<Server>::TaskData) mReaderData;
	SMARTPTR(AsyncTask) mReaderTask;
	///@}

public:
	Server(int port, int backlog):cPort(port),cBacklog(backlog)
	{
		cManager = new QueuedConnectionManager();
		cListener = new QueuedConnectionListener(cManager, 0);
		cReader = new QueuedConnectionReader(cManager, 0);
		cWriter = new ConnectionWriter(cManager,0);
		//
		PT(Connection)tcpSocket = cManager->open_TCP_server_rendezvous(cPort, cBacklog);
		cListener->add_connection(tcpSocket);
		//listener polling task
		mListenerData = new TaskInterface<Server>::TaskData(this,
				&Server::listenerPolling);
		mListenerTask = new GenericAsyncTask("Server::listenerPolling",
				&TaskInterface<Server>::taskFunction,
				reinterpret_cast<void*>(mListenerData.p()));
		mListenerTask->set_sort(-39);
		AsyncTaskManager::get_global_ptr()->add(mListenerTask);
		//reader polling task
		mReaderData = new TaskInterface<Server>::TaskData(this,
				&Server::readerPolling);
		mReaderTask = new GenericAsyncTask("Server::readerPolling",
				&TaskInterface<Server>::taskFunction,
				reinterpret_cast<void*>(mReaderData.p()));
		mReaderTask->set_sort(-40);
		AsyncTaskManager::get_global_ptr()->add(mReaderTask);

	}
	~Server()
	{
		delete cWriter;
		delete cReader;
		delete cListener;
		delete cManager;
		AsyncTaskManager::get_global_ptr()->remove(mReaderTask);
		AsyncTaskManager::get_global_ptr()->remove(mListenerTask);
	}
	void openTCPServerRendezvous()
	{
	}

	AsyncTask::DoneStatus listenerPolling(GenericAsyncTask* task)
	{
		if (cListener->new_connection_available())
		{
			PT(Connection) rendezvous;
			NetAddress netAddress;
			PT(Connection) newConnection;
			if (cListener->get_new_connection(rendezvous,netAddress,newConnection))
			{
				activeConnections.push_back(newConnection);	// Remember connection
				cReader->add_connection(newConnection.p());// Begin reading connection}
			}
		}
		return AsyncTask::DS_cont;
	}
	AsyncTask::DoneStatus readerPolling(GenericAsyncTask* task)
	{
		if (cReader->data_available())
		{
			NetDatagram datagram; // catch the incoming data in this instance
			// Check the return value; if we were threaded,
			// someone else could have
			// snagged this data before we did
			if (cReader->get_data(datagram))
			{
				//myProcessDataFunction(datagram);
			}
		}
		return AsyncTask::DS_cont;
	}
};

int main(int argc, char **argv)
{
	///setup
	// Load your configuration
	load_prc_file("config.prc");
	PandaFramework panda = PandaFramework();
	panda.open_framework(argc, argv);
	panda.set_window_title("animation training");
	WindowFramework* window = panda.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}

	// create server
	Server* server = new Server(9099, 1000);
	//
	// Do the main loop
	panda.main_loop();
	// delete server
	delete server;
	// close the framework
	panda.close_framework();
	//
	return 0;
}

