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
 * \file /Ely/include/Game/GameControlManager.h
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
#include "GameManager.h"
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Singleton manager updating control components.
 *
 * Prepared for multi-threading.
 */
class GameControlManager: public Singleton<GameControlManager>
{
public:

	/**
	 * \brief Constructor.
	 * @param sort The task sort.
	 * @param priority The task priority.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GameControlManager(int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GameControlManager();

	/**
	 * \brief Adds (if not present) an control component to updating.
	 * @param controlComp The control component.
	 */
	void addToControlUpdate(SMARTPTR(Component) controlComp);
	/**
	 * \brief Removes (if present) an control component from updating.
	 * @param controlComp The control component.
	 */
	void removeFromControlUpdate(SMARTPTR(Component) controlComp);

	/**
	 * \brief Updates control components.
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
	///@{
	///List of control components to be updated.
	typedef std::list<SMARTPTR(Component)> ControlComponentList;
	ControlComponentList mControlComponents;
	///@}

	///@{
	///A task data for update.
	SMARTPTR(TaskInterface<GameControlManager>::TaskData) mUpdateData;
	SMARTPTR(AsyncTask) mUpdateTask;
	///@}

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;

};

#endif /* GAMEINPUTMANAGER_H_ */
