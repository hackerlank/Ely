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
 * \file /Ely/training/threads.cpp
 *
 * \date 2012-06-30
 * \author consultit
 */

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <conditionVarFull.h>
#include <conditionVar.h>
#include <cstdlib>
#include <clockObject.h>

#define ELY_THREAD
#include "Utilities/Tools.h"

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");
const int maxSec = 2, maxIter = 3;
PT(ClockObject)clockObj;

Mutex mutex1;
ConditionVarFull var1(mutex1);
const unsigned long int completeM1 = 1 << 1, completeM2 = 1 << 2;
const unsigned long int exiting = 1 << 0;
const unsigned long int completeAllMs = completeM1 | completeM2;
unsigned long int completedMs = completeAllMs;

//const unsigned long int completePre = 1 << 0, completeM1 = 1 << 1, completeM2 = 1 << 2,
//		completePost = 1 << 3;
//unsigned long int completedTask = completePost;
//AsyncTask::DoneStatus taskPre(GenericAsyncTask* task, void * data)
//{
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		while ((completedTask & completePre) or not (completedTask & completePost))
//		{
//			var1.wait();
//		}
//		completedTask &= (~completeM1 & ~completeM2);
//	}
//	//Do work
//	int t = maxSec > 0 ? rand() % maxSec : 0;
//	sleep(t);
//	std::cout << "taskPre -> " << task->get_elapsed_time() << std::endl;
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		completedTask |= completePre;
//		var1.notify_all();
//	}
//	//
//	return AsyncTask::DS_cont;
//}
//AsyncTask::DoneStatus taskPost(GenericAsyncTask* task, void * data)
//{
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		while ((completedTask & completePost)
//				or (not ((completedTask & completeM1)
//						and (completedTask & completeM2))))
//		{
//			var1.wait();
//		}
//		completedTask &= ~completePre;
//	}
//	//Do work
//	int t = maxSec > 0 ? rand() % maxSec : 0;
//	sleep(t);
//	std::cout << "taskPost -> " << task->get_elapsed_time() << std::endl;
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		completedTask |= completePost;
//		var1.notify_all();
//	}
//	//
//	return AsyncTask::DS_cont;
//}
//AsyncTask::DoneStatus taskM1(GenericAsyncTask* task, void * data)
//{
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		while ((completedTask & completeM1) or not (completedTask & completePre))
//		{
//			var1.wait();
//		}
//		completedTask &= ~completePost;
//	}
//	//Do work
//	for (int i = 0; i < maxIter; ++i)
//	{
//		sleep(maxSec > 0 ? rand() % maxSec : 0);
//		std::cout << "\ttaskM1 -> " << task->get_elapsed_time() << std::endl;
//	}
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		completedTask |= completeM1;
//		var1.notify_all();
//	}
//	//
//	return AsyncTask::DS_cont;
//}
//AsyncTask::DoneStatus taskM2(GenericAsyncTask* task, void * data)
//{
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		while ((completedTask & completeM2) or not (completedTask & completePre))
//		{
//			var1.wait();
//		}
//		completedTask &= ~completePost;
//	}
//	//Do work
//	for (int i = 0; i < maxIter; ++i)
//	{
//		sleep(maxSec > 0 ? rand() % maxSec : 0);
//		std::cout << "\ttaskM2 -> " << task->get_elapsed_time() << std::endl;
//	}
//	//
//	{
//		HOLD_MUTEX(mutex1)
//		completedTask |= completeM2;
//		var1.notify_all();
//	}
//	//
//	return AsyncTask::DS_cont;
//}

AsyncTask::DoneStatus fireTasksM(GenericAsyncTask* task, void * data)
{
	HOLD_MUTEX(mutex1)
	std::cout << "start fireTasksM -> " << clockObj->get_long_time() << std::endl;
	completedMs = 0;
	var1.notify_all();
	while (completedMs ^ completeAllMs)
	{
		var1.wait();
	}
	if(completedMs & exiting)
	{
		return AsyncTask::DS_done;
	}
	std::cout << "end fireTasksM -> " << clockObj->get_long_time() << std::endl;
	return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus taskM1(GenericAsyncTask* task, void * data)
{
	//
	{
		HOLD_MUTEX(mutex1)
		while (completedMs & completeM1)
		{
			var1.wait();
		}
		if(completedMs & exiting)
		{
			return AsyncTask::DS_done;
		}
	}
	//Do work
	for (int i = 0; i < maxIter; ++i)
	{
		sleep(maxSec > 0 ? rand() % maxSec : 0);
		std::cout << "\ttaskM1 -> " << clockObj->get_long_time() << std::endl;
	}
	//
	{
		HOLD_MUTEX(mutex1)
		completedMs |= completeM1;
		var1.notify_all();
	}
	//
	return AsyncTask::DS_cont;
}
AsyncTask::DoneStatus taskM2(GenericAsyncTask* task, void * data)
{
	//
	{
		HOLD_MUTEX(mutex1)
		while (completedMs & completeM2)
		{
			var1.wait();
		}
		if(completedMs & exiting)
		{
			return AsyncTask::DS_done;
		}
	}
	//Do work
	for (int i = 0; i < maxIter; ++i)
	{
		sleep(maxSec > 0 ? rand() % maxSec : 0);
		std::cout << "\ttaskM2 -> " << clockObj->get_long_time() << std::endl;
	}
	//
	{
		HOLD_MUTEX(mutex1)
		completedMs |= completeM2;
		var1.notify_all();
	}
	//
	return AsyncTask::DS_cont;
}

int threads_main(int argc, char *argv[])
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
	trackball->set_pos(0, 200, 0);
	trackball->set_hpr(0, 15, 0);

	///here is room for your own code
	clockObj = ClockObject::get_global_clock();
//	//TASKPRE
//	AsyncTaskChain* chain1 =
//			AsyncTaskManager::get_global_ptr()->make_task_chain("chain1");
//	chain1->set_num_threads(1);
//	chain1->set_frame_sync(true);
//	PT(AsyncTask)TASKPRE = new GenericAsyncTask("TASKPRE",
//			&taskPre, reinterpret_cast<void*>(NULL));
////	TASKPRE->set_task_chain("chain1");
//	TASKPRE->set_task_chain("default");
//	TASKPRE->set_sort(10);
//	AsyncTaskManager::get_global_ptr()->add(TASKPRE);
//	//TASKPOST
//	AsyncTaskChain* chain3 =
//			AsyncTaskManager::get_global_ptr()->make_task_chain("chain3");
//	chain3->set_num_threads(1);
//	chain3->set_frame_sync(true);
//	PT(AsyncTask)TASKPOST = new GenericAsyncTask("TASKPOST",
//			&taskPost, reinterpret_cast<void*>(NULL));
////	TASKPOST->set_task_chain("chain3");
//	TASKPOST->set_task_chain("default");
//	TASKPOST->set_sort(20);
//	AsyncTaskManager::get_global_ptr()->add(TASKPOST);

	//GOTASKM
	PT(AsyncTask)GOTASKM = new GenericAsyncTask("GOTASKM",
			&fireTasksM, reinterpret_cast<void*>(NULL));
//	GOTASKM->set_task_chain("default");
	GOTASKM->set_sort(10);
	AsyncTaskManager::get_global_ptr()->add(GOTASKM);

	//TASKM1
	AsyncTaskChain* chainM1 =
			AsyncTaskManager::get_global_ptr()->make_task_chain("chainM1");
	chainM1->set_num_threads(1);
//	chainM1->set_frame_sync(true);
	PT(AsyncTask)taskm1 = new GenericAsyncTask("taskM1",
			&taskM1, reinterpret_cast<void*>(NULL));
	taskm1->set_task_chain("chainM1");
	AsyncTaskManager::get_global_ptr()->add(taskm1);
	//TASKM2
	AsyncTaskChain* chainM2 =
			AsyncTaskManager::get_global_ptr()->make_task_chain("chainM2");
	chainM2->set_num_threads(1);
//	chainM2->set_frame_sync(true);
	PT(AsyncTask)taskm2 = new GenericAsyncTask("taskM2",
			&taskM2, reinterpret_cast<void*>(NULL));
	taskm2->set_task_chain("chainM2");
	AsyncTaskManager::get_global_ptr()->add(taskm2);

	//do the main loop, equal to run() in python
	framework.main_loop();
	//exiting
	{
		HOLD_MUTEX(mutex1)
		completedMs = exiting;
		var1.notify_all();
	}
	//close the window framework
	framework.close_framework();
	bool t = AsyncTaskManager::get_global_ptr()->remove(GOTASKM);
	t = AsyncTaskManager::get_global_ptr()->remove(taskm1);
	t = AsyncTaskManager::get_global_ptr()->remove(taskm2);
	std::cout << t << std::endl;
	return (0);
}
