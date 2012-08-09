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
 * \file /Ely/include/Game/GameAudioManager.h
 *
 * \date 06/lug/2012 (20:15:17)
 * \author marco
 */

#ifndef GAMEAUDIOMANAGER_H_
#define GAMEAUDIOMANAGER_H_

#include <list>
#include <algorithm>
#include <audioManager.h>
#include <pandaFramework.h>
#include <clockObject.h>
#include <asyncTaskManager.h>
#include <asyncTask.h>
#include <asyncTaskChain.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Singleton manager updating audio components.
 *
 * Prepared for multi-threading.
 */
class GameAudioManager: public Singleton<GameAudioManager>
{
public:

	/**
	 * \Brief Constructor.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GameAudioManager(int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GameAudioManager();
	/**
	 * \brief Adds an audio component for updating.
	 * @param audioComp The audio component.
	 */
	void addToAudioUpdate(Component* audioComp);
	/**
	 * \brief Removes an audio component from updating.
	 * @param audioComp The audio component.
	 */
	void removeFromAudioUpdate(Component* audioComp);

	/**
	 * \brief Gets a reference to the audio manager.
	 * @return The audio manager.
	 */
	AudioManager* audioMgr() const;

	/**
	 * \brief Updates audio components.
	 *
	 * Will be called automatically in a task.
	 * @param task The task.
	 * @return The "done" status.
	 */
	AsyncTask::DoneStatus update(GenericAsyncTask* task);

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

private:
	/// Audio manager.
	PT(AudioManager) mAudioMgr;

	///@{
	///List of audio components to be updated.
	typedef std::list<Component*> AudioComponentList;
	AudioComponentList mAudioComponents;
	///@}

	///@{
	///A task data for update.
	PT(TaskInterface<GameAudioManager>::TaskData) mUpdateData;PT(AsyncTask) mUpdateTask;
	///@}

	///The last time update was called
	float mLastTime;

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;

};

#endif /* GAMEAUDIOMANAGER_H_ */
