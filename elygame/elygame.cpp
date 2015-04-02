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
 * \file /Ely/elygame/Ely.cpp
 *
 * \date Aug 30, 2013
 * \author consultit
 */

#include "elygame.h"
#include "elygame_ini.h"
#include <pandaFramework.h>

using namespace ely;

int main(int argc, char **argv)
{
#ifdef ELY_DEBUG
	ConfigVariableManager::get_global_ptr()->list_variables();
#endif
	// Load your configuration
	Filename configPrc(std::string());
	load_prc_file(Filename(std::string(ELY_CONFIGPRC)));

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
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::DATADIR,
	ELY_DATADIR);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::CONFIGFILE,
	ELY_CONFIGFILE);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::CALLBACKS,
	ELY_CALLBACKS_LA);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::TRANSITIONS,
	ELY_TRANSITIONS_LA);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::INITIALIZATIONS,
	ELY_INITIALIZATIONS_LA);
	GameManager::GetSingletonPtr()->setDataInfo(GameManager::INSTANCEUPDATES,
	ELY_INSTANCEUPDATES_LA);
	// Other managers (depending on GameManager)
#ifdef ELY_THREAD
	unsigned long int completedMask;
	completedAllMask = 0;
	completedTasks = 0;
	//AI
	managersMutex.acquire();
	completedMask = 1 << 1;
	completedAllMask |= completedMask;
	completedTasks = completedAllMask;
	TASKCHAIN(AI_chain, 1, false);
	GAMESUBMANAGER(GameAIManager, gameAIMgr, managersMutex, managersVar,
			completedMask, exiting, completedTasks, 0, 0, AI_chain);
	managersMutex.release();
	//Control
	managersMutex.acquire();
	completedMask = 1 << 2;
	completedAllMask |= completedMask;
	completedTasks = completedAllMask;
	TASKCHAIN(Control_chain, 1, false);
	GAMESUBMANAGER(GameControlManager, gameControlMgr, managersMutex,
			managersVar, completedMask, exiting, completedTasks, 0, 0, Control_chain);
	managersMutex.release();
	//Scene
	managersMutex.acquire();
	completedMask = 1 << 3;
	completedAllMask |= completedMask;
	completedTasks = completedAllMask;
	TASKCHAIN(Scene_chain, 1, false);
	GAMESUBMANAGER(GameSceneManager, gameSceneMgr, managersMutex, managersVar,
			completedMask, exiting, completedTasks, 0, 0, Scene_chain);
	managersMutex.release();
	//Physics
	managersMutex.acquire();
	completedMask = 1 << 4;
	completedAllMask |= completedMask;
	completedTasks = completedAllMask;
	TASKCHAIN(Physics_chain, 1, false);
	GAMESUBMANAGER(GamePhysicsManager, gamePhysicsMgr, managersMutex,
			managersVar, completedMask, exiting, completedTasks, 0, 0, Physics_chain);
	managersMutex.release();
	//Audio
	managersMutex.acquire();
	completedMask = 1 << 5;
	completedAllMask |= completedMask;
	completedTasks = completedAllMask;
	TASKCHAIN(Audio_chain, 1, false);
	GAMESUBMANAGER(GameAudioManager, gameAudioMgr, managersMutex, managersVar,
			completedMask, exiting, completedTasks, 0, 0, Audio_chain);
	managersMutex.release();
	//Behavior
	managersMutex.acquire();
	completedMask = 1 << 6;
	completedAllMask |= completedMask;
	completedTasks = completedAllMask;
	TASKCHAIN(Behavior_chain, 1, false);
	GAMESUBMANAGER(GameBehaviorManager, gameBehaviorMgr, managersMutex,
			managersVar, completedMask, exiting, completedTasks, 0, 0, Behavior_chain);
	managersMutex.release();
	///fireManagers
	SMARTPTR(AsyncTask)fireManagersTask = new GenericAsyncTask("fireManagersTask",
			&fireManagers, reinterpret_cast<void*>(NULL));
	fireManagersTask->set_task_chain("default");
	//sort values on default chain:
	//task_data_loop: -50
	//task_event: 0
	//task_record_frame: 45
	//task_garbage_collect: 46
	//task_igloop: 50
	//task_play_frame: 55
	//task_clear_screenshot_text: ?
	//task_clear_text: ?
	fireManagersTask->set_sort(10);
	AsyncTaskManager::get_global_ptr()->add(fireManagersTask);
#else
	//AI
	GameAIManager* gameAIMgr = new GameAIManager(10);
	//Control
	GameControlManager* gameControlMgr = new GameControlManager(10);
	//Scene
	GameSceneManager* gameSceneMgr = new GameSceneManager(10);
	//Physics
	GamePhysicsManager* gamePhysicsMgr = new GamePhysicsManager(10);
	//Audio
	GameAudioManager* gameAudioMgr = new GameAudioManager(10);
	//Behavior
	GameBehaviorManager* gameBehaviorMgr = new GameBehaviorManager(10);
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
	std::cout << "Number of AsyncTaskChains: " << numTaskChains << std::endl;
	for (int chain = 0; chain < numTaskChains; ++chain)
	{
		AsyncTaskChain* taskChain =
				AsyncTaskManager::get_global_ptr()->get_task_chain(chain);
		std::cout << "AsyncTaskChain '" << taskChain->get_name() << "':"
				<< std::endl;
		AsyncTaskCollection taskColl = taskChain->get_tasks();
		int numTasks = taskColl.get_num_tasks();
		std::cout << "\tNumber of AsyncTasks in chain: " << numTasks
				<< std::endl;
		for (int task = 0; task < numTasks; ++task)
		{
			AsyncTask* asyncTask = taskColl.get_task(task);
			std::cout << "\t\tAsyncTask: '" << asyncTask->get_name() << "'"
					<< std::endl;
			std::cout << "\t\t-- priority: " << asyncTask->get_priority()
					<< std::endl;
			std::cout << "\t\t-- sort: " << asyncTask->get_sort() << std::endl;
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
#if ELY_THREAD
	//exiting
	{
		HOLD_MUTEX(managersMutex)
		completedTasks = exiting;
		managersVar.notify_all();
	}
	AsyncTaskManager::get_global_ptr()->remove(fireManagersTask);
#endif
	delete gameBehaviorMgr;
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
		std::cerr << "Ely::main: Libtool error on libltdl shutting down"
				<< std::endl;
	}
	return 0;
}

#ifdef ELY_THREAD
Mutex managersMutex;
ConditionVarFull managersVar(managersMutex);
unsigned long int completedTasks;
unsigned long int completedAllMask;
const unsigned long int exiting = 1 << 0;
AsyncTask::DoneStatus fireManagers(GenericAsyncTask* task, void * data)
{
	HOLD_MUTEX(managersMutex)

	completedTasks = 0;
	managersVar.notify_all();
	while (completedTasks ^ completedAllMask)
	{
		managersVar.wait();
	}
	if(completedTasks & exiting)
	{
		return AsyncTask::DS_done;
	}
	return AsyncTask::DS_cont;
}
#endif

//explicit instantiations
template class PointerToArrayBase<PointerTo<AsyncTask> >;
template class PointerToBase<ReferenceCountedVector<PointerTo<AsyncTask> > >;
template TypeHandle _get_type_handle<ReferenceCountedVector<PointerTo<AsyncTask> > >(ReferenceCountedVector<PointerTo<AsyncTask> > const*);
template void _do_init_type<ReferenceCountedVector<PointerTo<AsyncTask> > >(ReferenceCountedVector<PointerTo<AsyncTask> > const*);
template void unref_delete<ReferenceCountedVector<PointerTo<AsyncTask> > >(ReferenceCountedVector<PointerTo<AsyncTask> >*);
