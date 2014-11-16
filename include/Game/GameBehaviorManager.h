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
 * \file /Ely/include/Game/GameBehaviorManager.h
 *
 * \date 01/set/2013 (17:47:10)
 * \author consultit
 */


#ifndef GAMEBEHAVIORMANAGER_H_
#define GAMEBEHAVIORMANAGER_H_

#include "Utilities/Tools.h"
#include <list>
#include "ObjectModel/Component.h"

namespace ely
{
/**
 * \brief Singleton manager updating Behavior components.
 *
 * Prepared for multi-threading.
 */
class GameBehaviorManager: public Singleton<GameBehaviorManager>
{
public:
	/**
	 * \brief Constructor.
	 * @param sort The task sort.
	 * @param priority The task priority.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GameBehaviorManager(
#ifdef ELY_THREAD
			Mutex& managersMutex, ConditionVarFull& managersVar,
			const unsigned long int completedMask,
			const unsigned long int exiting,
			unsigned long int& completedTasks,
#endif
			int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GameBehaviorManager();

	/**
	 * \brief Adds (if not present) an Behavior component to updating.
	 * @param behaviorComp The Behavior component.
	 */
	void addToBehaviorUpdate(SMARTPTR(Component)behaviorComp);
	/**
	 * \brief Removes (if present) an Behavior component from updating.
	 * @param behaviorComp The Behavior component.
	 */
	void removeFromBehaviorUpdate(SMARTPTR(Component) behaviorComp);

	/**
	 * \brief Updates Behavior components.
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
	///List of Behavior components to be updated.
	typedef std::list<SMARTPTR(Component)> BehaviorComponentList;
	BehaviorComponentList mBehaviorComponents;
	///@}

	///@{
	///A task data for step simulation update.
	SMARTPTR(TaskInterface<GameBehaviorManager>::TaskData) mUpdateData;
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
inline ReMutex& GameBehaviorManager::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* GAMEBEHAVIORMANAGER_H_ */
