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
 * \date 2012-07-29 
 * \author consultit
 */

#ifndef GAMEINPUTMANAGER_H_
#define GAMEINPUTMANAGER_H_

#include "Utilities/Tools.h"
#include <list>
#include "ObjectModel/Component.h"

namespace ely
{
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
	GameControlManager(
#ifdef ELY_THREAD
			Mutex& managersMutex, ConditionVarFull& managersVar,
			const unsigned long int completedMask,
			const unsigned long int exiting,
			unsigned long int& completedTasks,
#endif
			int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GameControlManager();

	/**
	 * \brief Adds (if not present) an control component to updating.
	 * @param controlComp The control component.
	 */
	void addToControlUpdate(SMARTPTR(Component)controlComp);
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

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

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

#ifdef ELY_THREAD
	///Multithreaded managers stuff
	///@{
	Mutex& mManagersMutex;
	ConditionVarFull& mManagersVar;
	const unsigned long int mCompletedMask, mExiting;
	unsigned long int& mCompletedTasks;
	///@}
	///The mutex associated with this manager.
	ReMutex mMutex;
#endif

};

///inline definitions

#ifdef ELY_THREAD
inline ReMutex& GameControlManager::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* GAMEINPUTMANAGER_H_ */
