#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <conditionVarFull.h>
#include <conditionVar.h>
#include <cstdlib>

#define ELY_THREAD
#include "Utilities/Tools.h"

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");
const int maxSec = 2, maxIter = 3;

Mutex mutex1;
ConditionVarFull var1(mutex1);
const int complete1 = 1 << 0, completeM1 = 1 << 1, completeM2 = 1 << 2,
		complete3 = 1 << 3;
unsigned int completedTask = complete3;

AsyncTask::DoneStatus taskFunction1(GenericAsyncTask* task, void * data)
{
	//
	{
		HOLD_MUTEX(mutex1)
		while ((completedTask & complete1) or not (completedTask & complete3))
		{
			var1.wait();
		}
		completedTask &= (~completeM1 & ~completeM2);
	}
	//Do work
	int t = maxSec > 0 ? rand() % maxSec : 0;
	sleep(t);
	std::cout << "taskFunction1 -> " << task->get_elapsed_time() << std::endl;
	//
	{
		HOLD_MUTEX(mutex1)
		completedTask |= complete1;
		var1.notify_all();
	}
	//
	return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus taskM1(GenericAsyncTask* task, void * data)
{
	//
	{
		HOLD_MUTEX(mutex1)
		while ((completedTask & completeM1) or not (completedTask & complete1))
		{
			var1.wait();
		}
		completedTask &= ~complete3;
	}
	//Do work
	for (int i = 0; i < maxIter; ++i)
	{
		sleep(maxSec > 0 ? rand() % maxSec : 0);
		std::cout << "\ttaskM1 -> " << task->get_elapsed_time() << std::endl;
	}
	//
	{
		HOLD_MUTEX(mutex1)
		completedTask |= completeM1;
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
		while ((completedTask & completeM2) or not (completedTask & complete1))
		{
			var1.wait();
		}
		completedTask &= ~complete3;
	}
	//Do work
	for (int i = 0; i < maxIter; ++i)
	{
		sleep(maxSec > 0 ? rand() % maxSec : 0);
		std::cout << "\ttaskM2 -> " << task->get_elapsed_time() << std::endl;
	}
	//
	{
		HOLD_MUTEX(mutex1)
		completedTask |= completeM2;
		var1.notify_all();
	}
	//
	return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus taskFunction3(GenericAsyncTask* task, void * data)
{
	//
	{
		HOLD_MUTEX(mutex1)
		while ((completedTask & complete3)
				or (not ((completedTask & completeM1)
						and (completedTask & completeM2))))
		{
			var1.wait();
		}
		completedTask &= ~complete1;
	}
	//Do work
	int t = maxSec > 0 ? rand() % maxSec : 0;
	sleep(t);
	std::cout << "taskFunction3 -> " << task->get_elapsed_time() << std::endl;
	//
	{
		HOLD_MUTEX(mutex1)
		completedTask |= complete3;
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
	//TASK3
	AsyncTaskChain* chain3 =
			AsyncTaskManager::get_global_ptr()->make_task_chain("chain3");
	chain3->set_num_threads(1);
	chain3->set_frame_sync(true);
	PT(AsyncTask)task3 = new GenericAsyncTask("task3",
			&taskFunction3, reinterpret_cast<void*>(NULL));
	task3->set_task_chain("chain3");
	AsyncTaskManager::get_global_ptr()->add(task3);

	//TASKM1
	AsyncTaskChain* chainM1 =
			AsyncTaskManager::get_global_ptr()->make_task_chain("chainM1");
	chainM1->set_num_threads(1);
	chainM1->set_frame_sync(true);
	PT(AsyncTask)taskm1 = new GenericAsyncTask("taskM1",
			&taskM1, reinterpret_cast<void*>(NULL));
	taskm1->set_task_chain("chainM1");
	AsyncTaskManager::get_global_ptr()->add(taskm1);
	//TASKM2
	AsyncTaskChain* chainM2 =
			AsyncTaskManager::get_global_ptr()->make_task_chain("chainM2");
	chainM2->set_num_threads(1);
	chainM2->set_frame_sync(true);
	PT(AsyncTask)taskm2 = new GenericAsyncTask("taskM2",
			&taskM2, reinterpret_cast<void*>(NULL));
	taskm2->set_task_chain("chainM2");
	AsyncTaskManager::get_global_ptr()->add(taskm2);

	//TASK1
	AsyncTaskChain* chain1 =
			AsyncTaskManager::get_global_ptr()->make_task_chain("chain1");
	chain1->set_num_threads(1);
	chain1->set_frame_sync(true);
	PT(AsyncTask)task1 = new GenericAsyncTask("task1",
			&taskFunction1, reinterpret_cast<void*>(NULL));
	task1->set_task_chain("chain1");
	AsyncTaskManager::get_global_ptr()->add(task1);

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}
