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
 * \file /Ely/src/Game/GameAudioManager.cpp
 *
 * \date 06/lug/2012 (20:15:17)
 * \author marco
 */

#include "Game/GameAudioManager.h"

GameAudioManager::GameAudioManager(int sort, int priority,
		const std::string& asyncTaskChain)
{
	CHECKEXISTENCE(GameManager::GetSingletonPtr(),
			"GameAudioManager::GameAudioManager: invalid GameManager")
	mAudioComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	mAudioMgr = AudioManager::create_AudioManager();
	//create the task for updating the active audio components
	mUpdateData = new TaskInterface<GameAudioManager>::TaskData(this,
			&GameAudioManager::update);
	mUpdateTask = new GenericAsyncTask("GameAudioManager::update",
			&TaskInterface<GameAudioManager>::taskFunction,
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

GameAudioManager::~GameAudioManager()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mAudioComponents.clear();
}

void GameAudioManager::addToAudioUpdate(SMARTPTR(Component) audioComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	AudioComponentList::iterator iter = find(mAudioComponents.begin(),
			mAudioComponents.end(), audioComp);
	if (iter == mAudioComponents.end())
	{
		mAudioComponents.push_back(audioComp);
	}
}

void GameAudioManager::removeFromAudioUpdate(SMARTPTR(Component) audioComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	AudioComponentList::iterator iter = find(mAudioComponents.begin(),
			mAudioComponents.end(), audioComp);
	if (iter != mAudioComponents.end())
	{
		mAudioComponents.remove(audioComp);
	}
}

SMARTPTR(AudioManager) GameAudioManager::audioMgr() const
{
	return mAudioMgr;
}

AsyncTask::DoneStatus GameAudioManager::update(GenericAsyncTask* task)
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

	// call all audio components update functions, passing delta time
	AudioComponentList::iterator iter;
	for (iter = mAudioComponents.begin(); iter != mAudioComponents.end();
			++iter)
	{
		(*iter).p()->update(reinterpret_cast<void*>(&dt));
	}
	//Update audio manager
	mAudioMgr->update();
	//
	return AsyncTask::DS_cont;
}

ReMutex& GameAudioManager::getMutex()
{
	return mMutex;
}
