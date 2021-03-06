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
 * \file /Ely/src/Game/GameParticlesManager.cpp
 *
 * \date 2016-02-06
 * \author consultit
 */

#include "GameParticlesManager.h"
#include "Game/GameManager.h"
#include <linearEulerIntegrator.h>
#include <angularEulerIntegrator.h>

namespace ely
{

GameParticlesManager::GameParticlesManager(
#ifdef ELY_THREAD
		Mutex& managersMutex, ConditionVarFull& managersVar,
		const unsigned long int completedMask,
		const unsigned long int exiting,
		unsigned long int& completedTasks,
#endif
		int sort, int priority,
		const std::string& asyncTaskChain)
#ifdef ELY_THREAD
		:mManagersMutex(managersMutex), mManagersVar(managersVar),
		mCompletedMask(completedMask), mExiting(exiting),
		mCompletedTasks(completedTasks)
#endif
{
	CHECK_EXISTENCE_DEBUG(GameManager::GetSingletonPtr(),
			"GameParticlesManager::GameParticlesManager: invalid GameManager")
	mUpdateData.clear();
	mUpdateTask.clear();
	// setup ParticleSystemManager
	mParticleSystemMgr = new ParticleSystemManager();
	mParticleSystemMgr->set_frame_stepping(1);
	// setup PhysicsManager
	mPhysicsMgr = new PhysicsManager();
	// attach a (linear) integrator
	mPhysicsMgr->attach_linear_integrator(new LinearEulerIntegrator());
	mPhysicsMgrAngular = false;
	//create the task for updating the managers
	mUpdateData = new TaskInterface<GameParticlesManager>::TaskData(this,
			&GameParticlesManager::update);
	mUpdateTask = new GenericAsyncTask("GameParticlesManager::update",
			&TaskInterface<GameParticlesManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	//set sort/priority
	mUpdateTask->set_sort(sort);
	mUpdateTask->set_priority(priority);
	//Add the task for updating the controlled object
#ifdef ELY_THREAD
	if (not asyncTaskChain.empty())
	{
		//Specifies the AsyncTaskChain on which mUpdateTask will be running.
		mUpdateTask->set_task_chain(asyncTaskChain);
	}
#endif
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
}

GameParticlesManager::~GameParticlesManager()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	// delete managers
	delete mPhysicsMgr;
	delete mParticleSystemMgr;
	mParticleSystemMgr = NULL;
	mPhysicsMgr = NULL;
}

ParticleSystemManager* GameParticlesManager::particleSystemMgr() const
{
	return mParticleSystemMgr;
}

PhysicsManager* GameParticlesManager::physicsMgr() const
{
	return mPhysicsMgr;
}

void GameParticlesManager::addAngularIntegrator()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

    if (not mPhysicsMgrAngular)
    {
		mPhysicsMgrAngular = true;
        mPhysicsMgr->attach_angular_integrator(new AngularEulerIntegrator());
    }
}

AsyncTask::DoneStatus GameParticlesManager::update(GenericAsyncTask* task)
{
#ifdef ELY_THREAD
	//manager multithread
	{
		HOLD_MUTEX(mManagersMutex)
		while (mCompletedTasks & mCompletedMask)
		{
			mManagersVar.wait();
		}
		if(mCompletedTasks & mExiting)
		{
			return AsyncTask::DS_done;
		}
	}
#endif
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
		dt = 0.016666667; //60 fps
#endif

		//update particle system manager
        mParticleSystemMgr->do_particles(dt);
		//update physics manager
        mPhysicsMgr->do_physics(dt);

	}
#ifdef ELY_THREAD
	//manager multithread
	{
		HOLD_MUTEX(mManagersMutex)
		mCompletedTasks |= mCompletedMask;
		mManagersVar.notify_all();
	}
#endif
	//
	return AsyncTask::DS_cont;
}

} // namespace ely
