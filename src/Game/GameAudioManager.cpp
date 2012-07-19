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

GameAudioManager::GameAudioManager(PandaFramework* pandaFramework)
{
	if (not pandaFramework)
	{
		throw GameException(
				"GameAudioManager::GameAudioManager: invalid PandaFramework");
	}
	mAudioComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	mPandaFramework = pandaFramework;
	mAudioMgr = AudioManager::create_AudioManager();
	//create the task for updating the active audio components
	mUpdateData = new TaskInterface<GameAudioManager>::TaskData(this,
			&GameAudioManager::update);
	mUpdateTask = new GenericAsyncTask("GameAudioManager::update",
			&TaskInterface<GameAudioManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	//Add the task for updating the controlled object
	mPandaFramework->get_task_mgr().add(mUpdateTask);
}

GameAudioManager::~GameAudioManager()
{
	if (mUpdateTask)
	{
		mPandaFramework->get_task_mgr().remove(mUpdateTask);
	}
	mAudioComponents.clear();
}

void GameAudioManager::addToAudioUpdate(Component* audioComp)
{
	//lock (guard) the mutex
	lock_guard<ReMutex> guard(mMutex);

	AudioComponentList::iterator iter = find(mAudioComponents.begin(),
			mAudioComponents.end(), audioComp);
	if (iter == mAudioComponents.end())
	{
		mAudioComponents.push_back(audioComp);
	}
}

void GameAudioManager::removeFromAudioUpdate(Component* audioComp)
{
	//lock (guard) the mutex
	lock_guard<ReMutex> guard(mMutex);

	AudioComponentList::iterator iter = find(mAudioComponents.begin(),
			mAudioComponents.end(), audioComp);
	if (iter != mAudioComponents.end())
	{
		mAudioComponents.remove(audioComp);
	}
}

AudioManager* GameAudioManager::audioMgr()
{
	//lock (guard) the mutex
	lock_guard<ReMutex> guard(mMutex);

	return mAudioMgr.p();
}

AsyncTask::DoneStatus GameAudioManager::update(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	lock_guard<ReMutex> guard(mMutex);

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
	//
	return AsyncTask::DS_cont;

}

