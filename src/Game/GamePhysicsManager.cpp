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
 * \file /Ely/src/Game/GamePhysicsManager.cpp
 *
 * \date 07/lug/2012 (10:58:21)
 * \author marco
 */

#include "Game/GamePhysicsManager.h"

GamePhysicsManager::GamePhysicsManager(int sort, int priority,
		const std::string& asyncTaskChain)
{
	mPhysicsComponents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
	mBulletWorld = new BulletWorld();
	mBulletWorld->set_gravity(0.0, 0.0, -9.81);
	//create the task for updating step simulation and physics component
	mUpdateData = new TaskInterface<GamePhysicsManager>::TaskData(this,
			&GamePhysicsManager::update);
	mUpdateTask = new GenericAsyncTask("GamePhysicsManager::update",
			&TaskInterface<GamePhysicsManager>::taskFunction,
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
	mLastTime = ClockObject::get_global_clock()->get_real_time();
#ifdef DEBUG
	// set up Bullet Debug Renderer (disabled by default)
	mBulletDebugNodePath = NodePath(new BulletDebugNode("Debug"));
#endif
}

GamePhysicsManager::~GamePhysicsManager()
{
	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mPhysicsComponents.clear();
}

void GamePhysicsManager::addToPhysicsUpdate(Component* physicsComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	PhysicsComponentList::iterator iter = find(mPhysicsComponents.begin(),
			mPhysicsComponents.end(), physicsComp);
	if (iter == mPhysicsComponents.end())
	{
		mPhysicsComponents.push_back(physicsComp);
	}
}

void GamePhysicsManager::removeFromPhysicsUpdate(Component* physicsComp)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	PhysicsComponentList::iterator iter = find(mPhysicsComponents.begin(),
			mPhysicsComponents.end(), physicsComp);
	if (iter != mPhysicsComponents.end())
	{
		mPhysicsComponents.remove(physicsComp);
	}
}

BulletWorld* GamePhysicsManager::bulletWorld() const
{
	return mBulletWorld.p();
}

AsyncTask::DoneStatus GamePhysicsManager::update(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float timeStep;
//	timeStep = ClockObject::get_global_clock()->get_dt();
	float currTime = ClockObject::get_global_clock()->get_real_time();
	timeStep = currTime - mLastTime;
	mLastTime = currTime;

	float maxSubSteps;

#ifdef TESTING
	timeStep = 0.016666667; //60 fps
#endif

	// call all physics components update functions, passing delta time
	PhysicsComponentList::iterator iter;
	for (iter = mPhysicsComponents.begin(); iter != mPhysicsComponents.end();
			++iter)
	{
		(*iter)->update(reinterpret_cast<void*>(&timeStep));
	}
	// do physics step simulation
	// timeStep < maxSubSteps * fixedTimeStep (=1/60.0=0.016666667) -->
	// supposing a minimum of 6,666666667 fps, we have a maximum
	// timeStep of 0.15 secs so: maxSubSteps <= 60 * 0.15 = 9
	if (timeStep < 0.016666667)
	{
		maxSubSteps = 1;
	}
	else if (timeStep < 0.033333333)
	{
		maxSubSteps = 2;
	}
	else if (timeStep < 0.05)
	{
		maxSubSteps = 3;
	}
	else if (timeStep < 0.066666668)
	{
		maxSubSteps = 4;
	}
	else if (timeStep < 0.083333335)
	{
		maxSubSteps = 5;
	}
	else if (timeStep < 0.100000002)
	{
		maxSubSteps = 6;
	}
	else if (timeStep < 0.116666669)
	{
		maxSubSteps = 7;
	}
	else if (timeStep < 0.133333336)
	{
		maxSubSteps = 8;
	}
	else
	{
		maxSubSteps = 9;
	}
	mBulletWorld->do_physics(timeStep, maxSubSteps);
	//
	return AsyncTask::DS_cont;

}

ReMutex& GamePhysicsManager::getMutex()
{
	return mMutex;
}

#ifdef DEBUG
BulletDebugNode* GamePhysicsManager::bulletDebugNodePath() const
{
	return DCAST(BulletDebugNode,mBulletDebugNodePath.node());
}

void GamePhysicsManager::initDebug(WindowFramework* windowFramework)
{
	mBulletDebugNodePath.reparent_to(windowFramework->get_render());
	BulletDebugNode* bulletDebugNode =
			DCAST(BulletDebugNode,mBulletDebugNodePath.node());
	mBulletWorld->set_debug_node(bulletDebugNode);
	bulletDebugNode->show_wireframe(true);
	bulletDebugNode->show_constraints(true);
	bulletDebugNode->show_bounding_boxes(false);
	bulletDebugNode->show_normals(false);
	mBulletDebugNodePath.hide();
}

void GamePhysicsManager::debug(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (enable)
	{
		if (mBulletDebugNodePath.is_hidden())
		{
			mBulletDebugNodePath.show();
		}
	}
	else
	{
		if (not mBulletDebugNodePath.is_hidden())
		{
			mBulletDebugNodePath.hide();
		}
	}
}
#endif
