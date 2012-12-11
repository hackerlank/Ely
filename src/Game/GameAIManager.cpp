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
 * \file /Ely/src/Game/GameAIManager.cpp
 *
 * \date 03/dic/2012 (13:57:43)
 * \author marco
 */

#include "Game/GameAIManager.h"

GameAIManager::GameAIManager(int sort, int priority,
		const std::string& asyncTaskChain)
{
	CHECKEXISTENCE(GameManager::GetSingletonPtr(),
			"GameAIManager::GameAIManager: invalid GameManager")
	mAIComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	mAIWorld = new AIWorld(
			GameManager::GetSingletonPtr()->windowFramework()->get_render());
	//create the task for updating AI components
	mUpdateData = new TaskInterface<GameAIManager>::TaskData(this,
			&GameAIManager::update);
	mUpdateTask = new GenericAsyncTask("GameAIManager::update",
			&TaskInterface<GameAIManager>::taskFunction,
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

GameAIManager::~GameAIManager()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mAIComponents.clear();
	//
	delete mAIWorld;
}

void GameAIManager::addToAIUpdate(SMARTPTR(Component)aiComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	AIComponentList::iterator iter = find(mAIComponents.begin(),
			mAIComponents.end(), aiComp);
	if (iter == mAIComponents.end())
	{
		mAIComponents.push_back(aiComp);
	}
}

void GameAIManager::removeFromAIUpdate(SMARTPTR(Component)aiComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	AIComponentList::iterator iter = find(mAIComponents.begin(),
			mAIComponents.end(), aiComp);
	if (iter != mAIComponents.end())
	{
		mAIComponents.remove(aiComp);
	}
}

AIWorld* GameAIManager::aiWorld() const
{
	return mAIWorld;
}

AsyncTask::DoneStatus GameAIManager::update(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all AI components update functions, passing delta time
	AIComponentList::iterator iter;
	for (iter = mAIComponents.begin(); iter != mAIComponents.end();
			++iter)
	{
		(*iter).p()->update(reinterpret_cast<void*>(&dt));
	}

	//
	return AsyncTask::DS_cont;
}

ReMutex& GameAIManager::getMutex()
{
	return mMutex;
}