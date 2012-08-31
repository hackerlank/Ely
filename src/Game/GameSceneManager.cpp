/*
 * GameSceneManager.cpp
 *
 *  Created on: 28/ago/2012
 *      Author: marco
 */

#include "Game/GameSceneManager.h"

GameSceneManager::GameSceneManager(int sort, int priority,
		const std::string& asyncTaskChain)
{
	mSceneComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	//create the task for updating the control components
	mUpdateData = new TaskInterface<GameSceneManager>::TaskData(this,
			&GameSceneManager::update);
	mUpdateTask = new GenericAsyncTask("GameControlManager::update",
			&TaskInterface<GameSceneManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	//set sort/priority
	mUpdateTask->set_sort(sort);
	mUpdateTask->set_priority(priority);
	//Add the task for updating the controlled object
#ifdef ELY_THREAD
	if (not asyncTaskChain.empty())
	{
		//Specifies the AsyncTaskChain on which mUpdateTask will be running.
		mUpdateTask->set_task_chain(asyncTaskChain);
	}
#endif
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
	mLastTime = ClockObject::get_global_clock()->get_real_time();
}

GameSceneManager::~GameSceneManager()
{
	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mSceneComponents.clear();
}

void GameSceneManager::addToSceneUpdate(SMARTPTR(Component)sceneComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SceneComponentList::iterator iter = find(mSceneComponents.begin(),
			mSceneComponents.end(), sceneComp);
	if (iter == mSceneComponents.end())
	{
		mSceneComponents.push_back(sceneComp);
	}
}

void GameSceneManager::removeFromSceneUpdate(SMARTPTR(Component)sceneComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SceneComponentList::iterator iter = find(mSceneComponents.begin(),
			mSceneComponents.end(), sceneComp);
	if (iter != mSceneComponents.end())
	{
		mSceneComponents.remove(sceneComp);
	}
}

AsyncTask::DoneStatus GameSceneManager::update(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt;
//	dt = ClockObject::get_global_clock()->get_dt();
	float currTime = ClockObject::get_global_clock()->get_real_time();
	dt = currTime - mLastTime;
	mLastTime = currTime;

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all scene components update functions, passing delta time
	SceneComponentList::iterator iter;
	for (iter = mSceneComponents.begin(); iter != mSceneComponents.end();
			++iter)
	{
		(*iter).p()->update(reinterpret_cast<void*>(&dt));
	}
	//
	return AsyncTask::DS_cont;
}

ReMutex& GameSceneManager::getMutex()
{
	return mMutex;
}
