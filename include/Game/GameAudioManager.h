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
 * \author consultit
 */

#ifndef GAMEAUDIOMANAGER_H_
#define GAMEAUDIOMANAGER_H_

#include "Utilities/Tools.h"

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
#include "GameManager.h"
#include "ObjectModel/Component.h"

/**
 * \brief Singleton manager updating audio components.
 *
 * Prepared for multi-threading.
 */
class GameAudioManager: public Singleton<GameAudioManager>
{
public:

	/**
	 * \brief Constructor.
	 * @param sort The task sort.
	 * @param priority The task priority.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GameAudioManager(int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GameAudioManager();
	/**
	 * \brief Adds (if not present) an audio component to updating.
	 * @param audioComp The audio component.
	 */
	void addToAudioUpdate(SMARTPTR(Component) audioComp);
	/**
	 * \brief Removes (if present) an audio component from updating.
	 * @param audioComp The audio component.
	 */
	void removeFromAudioUpdate(SMARTPTR(Component) audioComp);

	/**
	 * \brief Gets a reference to the audio manager.
	 * @return The audio manager.
	 */
	SMARTPTR(AudioManager) audioMgr() const;

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
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();

private:
	/// Audio manager.
	SMARTPTR(AudioManager) mAudioMgr;

	///@{
	///List of audio components to be updated.
	typedef std::list<SMARTPTR(Component)> AudioComponentList;
	AudioComponentList mAudioComponents;
	///@}

	///@{
	///A task data for update.
	SMARTPTR(TaskInterface<GameAudioManager>::TaskData) mUpdateData;
	SMARTPTR(AsyncTask) mUpdateTask;
	///@}

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;

};

#endif /* GAMEAUDIOMANAGER_H_ */
