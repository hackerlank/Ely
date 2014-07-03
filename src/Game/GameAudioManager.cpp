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
 * \author consultit
 */

#include "Game/GameAudioManager.h"
#include "Game/GameManager.h"

namespace ely
{

GameAudioManager::GameAudioManager(
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
		mCompletedMask(completedMask), mCompletedTasks(completedTasks),
		mExiting(exiting)
#endif
{
	CHECK_EXISTENCE_DEBUG(GameManager::GetSingletonPtr(),
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
}

GameAudioManager::~GameAudioManager()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mAudioComponents.clear();
}

void GameAudioManager::addToAudioUpdate(SMARTPTR(Component) audioComp)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

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
	HOLD_REMUTEX(mMutex)

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

		// call all audio components update functions, passing delta time
		AudioComponentList::iterator iter;
		for (iter = mAudioComponents.begin(); iter != mAudioComponents.end();
				++iter)
		{
			(*iter)->update(reinterpret_cast<void*>(&dt));
		}
		//Update audio manager
		mAudioMgr->update();
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
