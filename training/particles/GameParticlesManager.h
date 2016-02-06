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
 * \file /Ely/include/Game/GameParticlesManager.h
 *
 * \date 2016-02-06
 * \author consultit
 */

#ifndef GAMEPARTICLESMANAGER_H_
#define GAMEPARTICLESMANAGER_H_

#include "Utilities/Tools.h"
#include <particleSystemManager.h>
#include <physicsManager.h>

namespace ely
{

/**
 * \brief GameParticlesManager.
 */
class GameParticlesManager: public Singleton<GameParticlesManager>
{
public:
	/**
	 * \brief Constructor.
	 * @param sort The task sort.
	 * @param priority The task priority.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GameParticlesManager(
#ifdef ELY_THREAD
			Mutex& managersMutex, ConditionVarFull& managersVar,
			const unsigned long int completedMask,
			const unsigned long int exiting,
			unsigned long int& completedTasks,
#endif
			int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	/**
	 * \brief Destructor
	 */
	virtual ~GameParticlesManager();

	/**
	 * \brief Gets a reference to the particle system manager.
	 * @return The particle system manager.
	 */
	ParticleSystemManager* particleSystemMgr() const;

	/**
	 * \brief Gets a reference to the physics manager.
	 * @return The physics manager.
	 */
	PhysicsManager* physicsMgr() const;

	/**
	 * \brief Attach angular integrator into physics manager..
	 */
	void addAngularIntegrator();

	/**
	 * \brief Updates particle system and physics managers.
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
	/// The particle system manager.
	ParticleSystemManager* mParticleSystemMgr;
	/// The physics manager.
	PhysicsManager* mPhysicsMgr;
	/// Flag of angular integrator presence into physics manager.
	bool mPhysicsMgrAngular;

	///@{
	///A task data for update.
	SMARTPTR(TaskInterface<GameParticlesManager>::TaskData) mUpdateData;
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
inline ReMutex& GameGUIManager::getMutex()
{
	return mMutex;
}
#endif

} // namespace ely

#endif /* GAMEPARTICLESMANAGER_H_ */
