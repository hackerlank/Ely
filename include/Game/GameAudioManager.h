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
#include <asyncTask.h>
#include <reMutex.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Singleton manager updating 3d attributes of audio components.
 *
 * Thread-safe during utilization.
 */
class GameAudioManager: public Singleton<GameAudioManager>
{
public:

	GameAudioManager(PandaFramework* pandaFramework);
	virtual ~GameAudioManager();
	/**
	 * \brief Adds an audio component for updating the 3d attributes.
	 * @param audioComp The audio component.
	 */
	void addToAudioUpdate(Component* audioComp);
	/**
	 * \brief Removes an audio component from updating the 3d attributes.
	 * @param audioComp The audio component.
	 */
	void removeFromAudioUpdate(Component* audioComp);

	/**
	 * \brief Gets a reference to the audio manager.
	 * @return The audio manager.
	 */
	AudioManager* audioMgr();

	/**
	 * \brief Updates 3d attributes of audio components.
	 *
	 * Will be called automatically in a task.
	 * @param task The task.
	 * @return The "done" status.
	 */
	AsyncTask::DoneStatus update(GenericAsyncTask* task);

private:
	/// Audio manager.
	PT(AudioManager) mAudioMgr;
	///The PandaFramework.
	PandaFramework* mPandaFramework;

	///@{
	///List of audio components to be updated.
	typedef std::list<Component*> AudioComponentList;
	AudioComponentList mAudioComponents;
	///@}

	///@{
	///A task data for update.
	PT(TaskInterface<GameAudioManager>::TaskData) mUpdateData;
	PT(AsyncTask) mUpdateTask;
	///@}

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* GAMEAUDIOMANAGER_H_ */
