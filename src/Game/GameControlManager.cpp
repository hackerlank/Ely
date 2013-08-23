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
 * \file /Ely/src/Game/GameControlManager.cpp
 *
 * \date 29/lug/2012 (10:07:02)
 * \author consultit
 */

#include "Game/GameControlManager.h"
#include "Game/GameManager.h"

namespace ely
{

GameControlManager::GameControlManager(int sort, int priority,
		const std::string& asyncTaskChain)
{
	CHECK_EXISTENCE(GameManager::GetSingletonPtr(),
			"GameControlManager::GameControlManager: invalid GameManager")
	mControlComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	//create the task for updating the control components
	mUpdateData = new TaskInterface<GameControlManager>::TaskData(this,
			&GameControlManager::update);
	mUpdateTask = new GenericAsyncTask("GameControlManager::update",
			&TaskInterface<GameControlManager>::taskFunction,
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

GameControlManager::~GameControlManager()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mControlComponents.clear();
}

void GameControlManager::addToControlUpdate(SMARTPTR(Component)controlComp)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	ControlComponentList::iterator iter = find(mControlComponents.begin(),
			mControlComponents.end(), controlComp);
	if (iter == mControlComponents.end())
	{
		mControlComponents.push_back(controlComp);
	}
}

void GameControlManager::removeFromControlUpdate(SMARTPTR(Component)controlComp)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	ControlComponentList::iterator iter = find(mControlComponents.begin(),
			mControlComponents.end(), controlComp);
	if (iter != mControlComponents.end())
	{
		mControlComponents.remove(controlComp);
	}
}

AsyncTask::DoneStatus GameControlManager::update(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all control components update functions, passing delta time
	ControlComponentList::iterator iter;
	for (iter = mControlComponents.begin(); iter != mControlComponents.end();
			++iter)
	{
		(*iter)->update(reinterpret_cast<void*>(&dt));
	}
	//
	return AsyncTask::DS_cont;
}

Mutex& GameControlManager::getMutex()
{
	return mMutex;
}

} // namespace ely
