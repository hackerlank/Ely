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
 * \file /Ely/include/Game/GamePhysicsManager.h
 *
 * \date 07/lug/2012 (10:58:20)
 * \author marco
 */

#ifndef GAMEPHYSICSMANAGER_H_
#define GAMEPHYSICSMANAGER_H_

#include <list>
#include <algorithm>
#include <bulletWorld.h>
#include <bulletDebugNode.h>
#include <pandaFramework.h>

#ifdef DEBUG
#include <windowFramework.h>
#endif

#include <clockObject.h>
#include <asyncTask.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Singleton manager updating attributes of physics components.
 *
 * Prepared for multi-threading.
 */
class GamePhysicsManager: public Singleton<GamePhysicsManager>
{
public:
	GamePhysicsManager();
	virtual ~GamePhysicsManager();

	/**
	 * \brief Adds a physics component for updating.
	 * @param physicsComp The physics component.
	 */
	void addToPhysicsUpdate(Component* physicsComp);
	/**
	 * \brief Removes a physics component from updating.
	 * @param physicsComp The physics component.
	 */
	void removeFromPhysicsUpdate(Component* physicsComp);

	/**
	 * \brief Gets a reference to the Bullet world.
	 * @return The Bullet world.
	 */
	BulletWorld* bulletWorld() const;

	/**
	 * \brief Updates step simulation and physics components.
	 *
	 * Will be called automatically in a task.
	 * @param task The task.
	 * @return The "done" status.
	 */
	AsyncTask::DoneStatus update(GenericAsyncTask* task);

#ifdef DEBUG
	/**
	 * \brief Gets a reference to the Bullet Debug node.
	 * @return The Bullet Debug node.
	 */
	BulletDebugNode* bulletDebugNodePath();

	/**
	 * \brief Initializes debugging.
	 */
	void initDebug(WindowFramework* windowFramework);
	/**
	 * \brief Enables/disables debugging.
	 * @param enable True to enable, false to disable.
	 */
	void debug(bool enable);
#endif

private:
	/// Bullet world.
	PT(BulletWorld) mBulletWorld;

#ifdef DEBUG
	/// Bullet Debug node path.
	NodePath mBulletDebugNodePath;
#endif

	///@{
	///List of physics components to be updated.
	typedef std::list<Component*> PhysicsComponentList;
	PhysicsComponentList mPhysicsComponents;
	///@}

	///@{
	///A task data for step simulation update.
	PT(TaskInterface<GamePhysicsManager>::TaskData) mUpdateData;
	PT(AsyncTask) mUpdateTask;
	///@}

	///The last time update was called
	float mLastTime;

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* GAMEPHYSICSMANAGER_H_ */
