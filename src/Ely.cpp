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
#ifdef ELY_THREAD
	//create (if necessary) other task chains
	AsyncTaskChain *taskChain =
			AsyncTaskManager::get_global_ptr()->make_task_chain(
					"ManagersChain");
	//Changes the number of threads for taskChain.
	taskChain->set_num_threads(1);
	//Sets the frame_sync flag.
	taskChain->set_frame_sync(true);
	//
	GameAudioManager* gameAudioMgr = new GameAudioManager();
	GameInputManager* gameInputMgr = new GameInputManager();
	GamePhysicsManager* gamePhysicsMgr = new GamePhysicsManager();
#else
	GameAudioManager* gameAudioMgr = new GameAudioManager();
	GameInputManager* gameInputMgr = new GameInputManager();
	GamePhysicsManager* gamePhysicsMgr = new GamePhysicsManager();
#endif
#ifdef DEBUG
	//threading
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
	//loop over task chains
	int numTaskChains =
			AsyncTaskManager::get_global_ptr()->get_num_task_chains();
	std::cout << "Number of AsyncTaskChains :" << numTaskChains << std::endl;
	for (int chain = 0; chain < numTaskChains; ++chain)
	{
		AsyncTaskChain* taskChain =
				AsyncTaskManager::get_global_ptr()->get_task_chain(chain);
		std::cout << "AsyncTaskChain '" << taskChain->get_name() << "':"
				<< std::endl;
		std::cout << "\tNumber of AsyncTasks (Chain)"
				<< taskChain->get_num_tasks() << std::endl;
		AsyncTaskCollection taskColl = taskChain->get_tasks();
		int numTasks = taskColl.get_num_tasks();
		std::cout << "\tNumber of AsyncTasks (Collection)" << numTasks
				<< std::endl;
		for (int task = 0; task < numTasks; ++task)
		{
			AsyncTask* asyncTask = taskColl.get_task(task);
			std::cout << "\t\tAsyncTask: '" << asyncTask->get_name() << "'"
					<< std::endl;
			std::cout << "\t\t\tpriority: " << asyncTask->get_priority() << std::endl;
			std::cout << "\t\t\tsort: " << asyncTask->get_sort() << std::endl;
		}
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

