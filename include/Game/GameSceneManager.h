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
 * \file  /Ely/include/Game/GameSceneManager.h
 *
 * \date 2012-08-28
 * \author consultit
 */

#ifndef GAMESCENEMANAGER_H_
#define GAMESCENEMANAGER_H_

#include "Utilities/Tools.h"
#include <list>
#include "ObjectModel/Component.h"

namespace ely
{
/**
 * \brief Singleton manager updating scene components.
 *
 * Prepared for multi-threading.
 */
class GameSceneManager: public Singleton<GameSceneManager>
{
public:

	/**
	 * \brief Constructor.
	 * @param sort The task sort.
	 * @param priority The task priority.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GameSceneManager(
#ifdef ELY_THREAD
			Mutex& managersMutex, ConditionVarFull& managersVar,
			const unsigned long int completedMask,
			const unsigned long int exiting,
			unsigned long int& completedTasks,
#endif
			int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GameSceneManager();

	/**
	 * \brief Adds (if not present) a scene component to updating.
	 * @param sceneComp The scene component.
	 */
	void addToSceneUpdate(SMARTPTR(Component)sceneComp);
	/**
	 * \brief Removes (if present) a scene component from updating.
	 * @param sceneComp The scene component.
	 */
	void removeFromSceneUpdate(SMARTPTR(Component) sceneComp);

	/**
	 * \brief Updates scene components.
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
	///List of scene components to be updated.
	typedef std::list<SMARTPTR(Component)> SceneComponentList;
	SceneComponentList mSceneComponents;
	///@}

	///@{
	///A task data for update.
	SMARTPTR(TaskInterface<GameSceneManager>::TaskData) mUpdateData;
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
inline ReMutex& GameSceneManager::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* GAMESCENEMANAGER_H_ */
