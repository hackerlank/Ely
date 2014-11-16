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
 * \file /Ely/include/Game/GameAIManager.h
 *
 * \date 03/dic/2012 (13:57:43)
 * \author consultit
 */

#ifndef GAMEAIMANAGER_H_
#define GAMEAIMANAGER_H_

#include "Utilities/Tools.h"
#include <list>
#include "ObjectModel/Component.h"

namespace ely
{
/**
 * \brief Singleton manager updating AI components.
 *
 * Prepared for multi-threading.
 * \note AI components handled by this manager should belong only to
 * objects reparented to the root scene nodepath (i.e. render).
 */
class GameAIManager: public Singleton<GameAIManager>
{
public:
	/**
	 * \brief Constructor.
	 * @param sort The task sort.
	 * @param priority The task priority.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GameAIManager(
#ifdef ELY_THREAD
			Mutex& managersMutex, ConditionVarFull& managersVar,
			const unsigned long int completedMask,
			const unsigned long int exiting,
			unsigned long int& completedTasks,
#endif
			int sort = 0, int priority = 0, const std::string& asyncTaskChain =
					std::string(""));
	virtual ~GameAIManager();

	/**
	 * \brief Adds (if not present) an AI component to updating.
	 * @param aiComp The AI component.
	 */
	void addToAIUpdate(SMARTPTR(Component)aiComp);
	/**
	 * \brief Removes (if present) an AI component from updating.
	 * @param aiComp The AI component.
	 */
	void removeFromAIUpdate(SMARTPTR(Component) aiComp);

	/**
	 * \brief Updates AI components.
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
	///List of AI components to be updated.
	typedef std::list<SMARTPTR(Component)> AIComponentList;
	AIComponentList mAIComponents;
	///@}

	///@{
	///A task data for step simulation update.
	SMARTPTR(TaskInterface<GameAIManager>::TaskData) mUpdateData;
	SMARTPTR(AsyncTask) mUpdateTask;
	///@}

	///HACK: start frame in which components' updates begin.
	int mStartFrame;

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
inline ReMutex& GameAIManager::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* GAMEAIMANAGER_H_ */
