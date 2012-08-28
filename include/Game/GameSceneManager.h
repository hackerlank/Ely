/*
 * GameSceneManager.h
 *
 *  Created on: 28/ago/2012
 *      Author: marco
 */

#ifndef GAMESCENEMANAGER_H_
#define GAMESCENEMANAGER_H_

#include <list>
#include <algorithm>
#include <clockObject.h>
#include <asyncTaskManager.h>
#include <asyncTask.h>
#include <asyncTaskChain.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

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
	GameSceneManager(int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GameSceneManager();

	/**
	 * \brief Adds a scene component for updating.
	 * @param sceneComp The scene component.
	 */
	void addToSceneUpdate(SMARTPTR(Component) sceneComp);
	/**
	 * \brief Removes a scene component from updating.
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

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

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

	///The last time update was called
	float mLastTime;

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;

};

#endif /* GAMESCENEMANAGER_H_ */
