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
#include <pandaFramework.h>
#include <clockObject.h>
#include <asyncTask.h>
#include <reMutex.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Singleton manager updating attributes of physics components.
 *
 * Thread-safe during utilization.
 */
class GamePhysicsManager: public Singleton<GamePhysicsManager>
{
public:
	GamePhysicsManager(PandaFramework* pandaFramework);
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
	BulletWorld* bulletWorld();

	/**
	 * \brief Updates step simulation and physics components.
	 *
	 * Will be called automatically in a task.
	 * @param task The task.
	 * @return The "done" status.
	 */
	AsyncTask::DoneStatus update(GenericAsyncTask* task);

private:
	/// Bullet world.
	PT(BulletWorld) mBulletWorld;
	///The PandaFramework.
	PandaFramework* mPandaFramework;

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

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* GAMEPHYSICSMANAGER_H_ */
