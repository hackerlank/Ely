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
 * \file /Ely/src/Game/GameSceneManager.cpp
 *
 * \date 2012-08-28
 * \author consultit
 */

#include "Game/GameSceneManager.h"
#include "Game/GameManager.h"

namespace ely
{

GameSceneManager::GameSceneManager(
#ifdef ELY_THREAD
		Mutex& managersMutex, ConditionVarFull& managersVar,
		const unsigned long int completedMask,
		const unsigned long int exiting,
		unsigned long int& completedTasks,
#endif
		int sort, int priority,
		const std::string& asyncTaskChain)
#ifdef ELY_THREAD
		:mManagersMutex(managersMutex), mManagersVar(managersVar),
		mCompletedMask(completedMask), mExiting(exiting),
		mCompletedTasks(completedTasks)
#endif
{
	CHECK_EXISTENCE_DEBUG(GameManager::GetSingletonPtr(),
			"GameSceneManager::GameSceneManager: invalid GameManager")
	mSceneComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	//create the task for updating the control components
	mUpdateData = new TaskInterface<GameSceneManager>::TaskData(this,
			&GameSceneManager::update);
	mUpdateTask = new GenericAsyncTask("GameSceneManager::update",
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
}

GameSceneManager::~GameSceneManager()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mSceneComponents.clear();
}

void GameSceneManager::addToSceneUpdate(SMARTPTR(Component)sceneComp)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

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
	HOLD_REMUTEX(mMutex)

	SceneComponentList::iterator iter = find(mSceneComponents.begin(),
			mSceneComponents.end(), sceneComp);
	if (iter != mSceneComponents.end())
	{
		mSceneComponents.remove(sceneComp);
	}
}

AsyncTask::DoneStatus GameSceneManager::update(GenericAsyncTask* task)
{
#ifdef ELY_THREAD
	//manager multithread
	{
		HOLD_MUTEX(mManagersMutex)
		while (mCompletedTasks & mCompletedMask)
		{
			mManagersVar.wait();
		}
		if(mCompletedTasks & mExiting)
		{
			return AsyncTask::DS_done;
		}
	}
#endif
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
		dt = 0.016666667; //60 fps
#endif

		// call all scene components update functions, passing delta time
		SceneComponentList::iterator iter;
		for (iter = mSceneComponents.begin(); iter != mSceneComponents.end();
				++iter)
		{
			(*iter)->update(reinterpret_cast<void*>(&dt));
		}
	}
#ifdef ELY_THREAD
	//manager multithread
	{
		HOLD_MUTEX(mManagersMutex)
		mCompletedTasks |= mCompletedMask;
		mManagersVar.notify_all();
	}
#endif
	//
	return AsyncTask::DS_cont;
}

} // namespace ely
