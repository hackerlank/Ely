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
 * \file /Ely/include/Game/GameInputManager.h
 *
 * \date 29/lug/2012 (10:07:02)
 * \author marco
 */

#ifndef GAMEINPUTMANAGER_H_
#define GAMEINPUTMANAGER_H_

#include <list>
#include <algorithm>
#include <pandaFramework.h>
#include <clockObject.h>
#include <asyncTask.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Singleton manager updating input components.
 *
 * Prepared for multi-threading.
 */
class GameInputManager: public Singleton<GameInputManager>
{
public:

	GameInputManager(PandaFramework* pandaFramework);
	virtual ~GameInputManager();

	/**
	 * \brief Adds an input component for updating.
	 * @param inputComp The input component.
	 */
	void addToInputUpdate(Component* inputComp);
	/**
	 * \brief Removes an input component from updating.
	 * @param inputComp The input component.
	 */
	void removeFromInputUpdate(Component* inputComp);

	/**
	 * \brief Updates input components.
	 *
	 * Will be called automatically in a task.
	 * @param task The task.
	 * @return The "done" status.
	 */
	AsyncTask::DoneStatus update(GenericAsyncTask* task);

private:
	///The PandaFramework.
	PandaFramework* mPandaFramework;

	///@{
	///List of input components to be updated.
	typedef std::list<Component*> InputComponentList;
	InputComponentList mInputComponents;
	///@}

	///@{
	///A task data for update.
	PT(TaskInterface<GameInputManager>::TaskData) mUpdateData;
	PT(AsyncTask) mUpdateTask;
	///@}

	///The last time update was called
	float mLastTime;

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;

};

#endif /* GAMEINPUTMANAGER_H_ */
