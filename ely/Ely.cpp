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
 * \file /Ely/ely/Ely.cpp
 *
 * \date Aug 30, 2013
 * \author consultit
 */

#include "Ely.h"
#include "Ely_ini.h"

using namespace ely;

int main(int argc, char **argv)
{
#ifdef ELY_DEBUG
	ConfigVariableManager::get_global_ptr()->list_variables();
#endif
	// Load your configuration
	Filename configPrc(std::string());
	load_prc_file(
			Filename(std::string(ELY_CONFIGPRC)));

	// Libtool: initialize libltdl.
	if (lt_dlinit() != 0)
	{
		throw GameException(
				"Ely::main: Libtool error on libltdl initialization");
	}
	// Setup the game framework
	// ComponentTemplate manager
	ComponentTemplateManager* componentTmplMgr = new ComponentTemplateManager();
	// ObjectTemplate manager
	ObjectTemplateManager* objectTmplMgr = new ObjectTemplateManager();
	// First create a Game manager: mandatory
	GameManager* gameMgr = new GameManager(argc, argv);
	// Add game data info
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::DATADIR, ELY_DATADIR);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::CONFIGFILE, ELY_CONFIGFILE);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::CALLBACKS, ELY_CALLBACKS_LA);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::TRANSITIONS, ELY_TRANSITIONS_LA);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::INITIALIZATIONS, ELY_INITIALIZATIONS_LA);
	// Other managers (depending on GameManager)
#ifdef ELY_THREAD
	AsyncTaskChain *taskChain;
	GAMESUBMANAGER(GameAIManager, gameAIMgr, 0, 0, taskChain, 2, true)
	GAMESUBMANAGER(GameControlManager, gameControlMgr, 0, 0, taskChain, 2, true)
	GAMESUBMANAGER(GameSceneManager, gameSceneMgr, 0, 0, taskChain, 2, true)
	GAMESUBMANAGER(GamePhysicsManager, gamePhysicsMgr, 0, 0, taskChain, 2, true)
	GAMESUBMANAGER(GameAudioManager, gameAudioMgr, 0, 0, taskChain, 2, true)
#else
	GameAIManager* gameAIMgr = new GameAIManager();
	GameControlManager* gameControlMgr = new GameControlManager();
	GameSceneManager* gameSceneMgr = new GameSceneManager();
	GamePhysicsManager* gamePhysicsMgr = new GamePhysicsManager();
	GameAudioManager* gameAudioMgr = new GameAudioManager();
#endif

#if defined (ELY_THREAD) && defined (ELY_DEBUG)
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
				"supports current OS-implemented threads" << std::endl;
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
			std::cout << "\t\t\tpriority: " << asyncTask->get_priority()
					<< std::endl;
			std::cout << "\t\t\tsort: " << asyncTask->get_sort() << std::endl;
		}
	}
#endif

	// Set the game up
	gameMgr->gameSetup();

	// Do the main loop
	gameMgr->main_loop();

	// Clean the game up
	gameMgr->gameCleanup();

	// Close the game framework
	delete gameAudioMgr;
	delete gamePhysicsMgr;
	delete gameSceneMgr;
	delete gameControlMgr;
	delete gameAIMgr;
	delete gameMgr;
	delete objectTmplMgr;
	delete componentTmplMgr;
	// Libtool: shut down libltdl and close all modules.
	if (lt_dlexit() != 0)
	{
		std::cerr << "Ely::main: Libtool error on libltdl shutting down" << std::endl;
	}
	return 0;
}

