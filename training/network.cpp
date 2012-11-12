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
	std::set<PT(Connection)> activeConnections;
	///@{
	///A task data for new connections Polling.
	SMARTPTR(TaskInterface<Server>::TaskData) mNewConnData;
	SMARTPTR(AsyncTask) mNewConnTask;
	///A task data for reset connections Polling.
	SMARTPTR(TaskInterface<Server>::TaskData) mResetConnData;
	SMARTPTR(AsyncTask) mResetConnTask;
	///A task data for read connections Polling.
	SMARTPTR(TaskInterface<Server>::TaskData) mReadConnData;
	SMARTPTR(AsyncTask) mReadConnTask;
	///@}

public:
	Server(int port, int backlog):cPort(port),cBacklog(backlog)
	{
		cManager = new QueuedConnectionManager();
		cListener = new QueuedConnectionListener(cManager, 0);
		cReader = new QueuedConnectionReader(cManager, 0);
		cWriter = new ConnectionWriter(cManager,0);
		cReader->set_raw_mode(true);
		cWriter->set_raw_mode(true);
		//
		PT(Connection)tcpSocket = cManager->open_TCP_server_rendezvous(cPort, cBacklog);
		cListener->add_connection(tcpSocket);
		//new connections polling task
		mNewConnData = new TaskInterface<Server>::TaskData(this,
				&Server::newConnPolling);
		mNewConnTask = new GenericAsyncTask("Server::newConnPolling",
				&TaskInterface<Server>::taskFunction,
				reinterpret_cast<void*>(mNewConnData.p()));
		mNewConnTask->set_sort(-38);
		AsyncTaskManager::get_global_ptr()->add(mNewConnTask);
		//reset connections polling task
		mResetConnData = new TaskInterface<Server>::TaskData(this,
				&Server::resetConnPolling);
		mResetConnTask = new GenericAsyncTask("Server::resetConnPolling",
				&TaskInterface<Server>::taskFunction,
				reinterpret_cast<void*>(mResetConnData.p()));
		mResetConnTask->set_sort(-39);
		AsyncTaskManager::get_global_ptr()->add(mNewConnTask);
		//readConn polling task
		mReadConnData = new TaskInterface<Server>::TaskData(this,
				&Server::readConnPolling);
		mReadConnTask = new GenericAsyncTask("Server::readerPolling",
				&TaskInterface<Server>::taskFunction,
				reinterpret_cast<void*>(mReadConnData.p()));
		mReadConnTask->set_sort(-40);
		AsyncTaskManager::get_global_ptr()->add(mReadConnTask);

	}
	~Server()
	{
		delete cWriter;
		delete cReader;
		delete cListener;
		delete cManager;
		AsyncTaskManager::get_global_ptr()->remove(mReadConnTask);
		AsyncTaskManager::get_global_ptr()->remove(mResetConnTask);
		AsyncTaskManager::get_global_ptr()->remove(mNewConnTask);
	}

	AsyncTask::DoneStatus newConnPolling(GenericAsyncTask* task)
	{
		if (cListener->new_connection_available())
		{
			PT(Connection) rendezvous;
			NetAddress netAddress;
			PT(Connection) newConnection;
			if (cListener->get_new_connection(rendezvous,netAddress,newConnection))
			{
				activeConnections.insert(newConnection);
				cReader->add_connection(newConnection.p());
				NetAddress addr = newConnection->get_address();
				PRINT("Connection from address: " << addr.get_ip_string());
			}
		}
		return AsyncTask::DS_cont;
	}
	AsyncTask::DoneStatus resetConnPolling(GenericAsyncTask* task)
	{
	    if (cManager.reset_connection_available()) {
	      PT(Connection) connection;
	      if (cManager.get_reset_connection(connection)) {
	    	  PRINT("Lost connection from "<< connection->get_address());
	        clients.erase(connection);
	        cManager.close_connection(connection);
	      }
	    }
		return AsyncTask::DS_cont;
	}
	AsyncTask::DoneStatus readConnPolling(GenericAsyncTask* task)
	{
		if (cReader->data_available())
		{
			NetDatagram datagram;
			if (cReader->get_data(datagram))
			{
				std::string inData = datagram.get_message();
				NetDatagram outData;
				outData.add_string("echo: " + inData);
				cWriter->send(outData,datagram.get_connection());
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

