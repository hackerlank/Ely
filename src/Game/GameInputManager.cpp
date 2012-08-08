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
 * \file /Ely/src/Game/GameInputManager.cpp
 *
 * \date 29/lug/2012 (10:07:02)
 * \author marco
 */

#include "Game/GameInputManager.h"

GameInputManager::GameInputManager(const std::string& asyncTaskChain = std::string(""))
{
	mInputComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	//create the task for updating the input components
	mUpdateData = new TaskInterface<GameInputManager>::TaskData(this,
			&GameInputManager::update);
	mUpdateTask = new GenericAsyncTask("GameInputManager::update",
			&TaskInterface<GameInputManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
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

GameInputManager::~GameInputManager()
{
	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mInputComponents.clear();
}

void GameInputManager::addToInputUpdate(Component* inputComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	InputComponentList::iterator iter = find(mInputComponents.begin(),
			mInputComponents.end(), inputComp);
	if (iter == mInputComponents.end())
	{
		mInputComponents.push_back(inputComp);
	}
}

void GameInputManager::removeFromInputUpdate(Component* inputComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	InputComponentList::iterator iter = find(mInputComponents.begin(),
			mInputComponents.end(), inputComp);
	if (iter != mInputComponents.end())
	{
		mInputComponents.remove(inputComp);
	}
}

AsyncTask::DoneStatus GameInputManager::update(GenericAsyncTask* task)
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

	// call all input components update functions, passing delta time
	InputComponentList::iterator iter;
	for (iter = mInputComponents.begin(); iter != mInputComponents.end();
			++iter)
	{
		(*iter)->update(reinterpret_cast<void*>(&dt));
	}
	//
	return AsyncTask::DS_cont;

}

ReMutex& GameInputManager::getMutex()
{
	return mMutex;
}

