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
 * \author consultit
 */

#include "Game/GamePhysicsManager.h"

GamePhysicsManager::GamePhysicsManager(int sort, int priority,
		const std::string& asyncTaskChain)
{
	CHECKEXISTENCE(GameManager::GetSingletonPtr(),
			"GamePhysicsManager::GamePhysicsManager: invalid GameManager")
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
#ifdef ELY_DEBUG
	// set up Bullet Debug Renderer (disabled by default)
	mBulletDebugNodePath = NodePath(new BulletDebugNode("Debug"));
#endif
}

GamePhysicsManager::~GamePhysicsManager()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	mPhysicsComponents.clear();
}

void GamePhysicsManager::addToPhysicsUpdate(SMARTPTR(Component) physicsComp)
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

void GamePhysicsManager::removeFromPhysicsUpdate(SMARTPTR(Component) physicsComp)
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

SMARTPTR(BulletWorld) GamePhysicsManager::bulletWorld() const
{
	return mBulletWorld;
}

AsyncTask::DoneStatus GamePhysicsManager::update(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = ClockObject::get_global_clock()->get_dt();

	int maxSubSteps;

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all physics components update functions, passing delta time
	PhysicsComponentList::iterator iter;
	for (iter = mPhysicsComponents.begin(); iter != mPhysicsComponents.end();
			++iter)
	{
		(*iter).p()->update(reinterpret_cast<void*>(&dt));
	}
	// do physics step simulation
	// timeStep < maxSubSteps * fixedTimeStep (=1/60.0=0.016666667) -->
	// supposing a minimum of 6,666666667 fps, we have a maximum
	// timeStep of 0.15 secs so: maxSubSteps <= 60 * 0.15 = 9
	if (dt < 0.016666667)
	{
		maxSubSteps = 1;
	}
	else if (dt < 0.033333333)
	{
		maxSubSteps = 2;
	}
	else if (dt < 0.05)
	{
		maxSubSteps = 3;
	}
	else if (dt < 0.066666668)
	{
		maxSubSteps = 4;
	}
	else if (dt < 0.083333335)
	{
		maxSubSteps = 5;
	}
	else if (dt < 0.100000002)
	{
		maxSubSteps = 6;
	}
	else if (dt < 0.116666669)
	{
		maxSubSteps = 7;
	}
	else if (dt < 0.133333336)
	{
		maxSubSteps = 8;
	}
	else
	{
		maxSubSteps = 9;
	}
	mBulletWorld->do_physics(dt, maxSubSteps);
	//
	return AsyncTask::DS_cont;
}

ReMutex& GamePhysicsManager::getMutex()
{
	return mMutex;
}

SMARTPTR(BulletShape)GamePhysicsManager::createShape(NodePath modelNP,
		ShapeType shapeType, ShapeSize shapeSize, LVector3& modelDims,
		LVector3& modelDeltaCenter, float& modelRadius,
		float& dim1, float& dim2, float& dim3, float& dim4,
		bool automaticShaping, BulletUpAxis upAxis,
		const Filename& heightfieldFile, bool dynamic)
{
	// create the current shape
	SMARTPTR(BulletShape) collisionShape = NULL;
	NodePathCollection geomNodes;
	//some preliminary check
	if (modelNP.is_empty())
	{
		//a bullet shape is requested without an associated model:
		//force automaticShaping to false
		automaticShaping = false;
	}
	else
	{
		//check if there are some GeomNode
		geomNodes = modelNP.find_all_matches("**/+GeomNode");
		if (not geomNodes.is_empty())
		{
			//get the bounding dimensions of object node path, that
			//should represents a model
			getBoundingDimensions(modelNP, modelDims, modelDeltaCenter, modelRadius);
		}
		else
		{
			//a bullet shape is requested without GeomNodes:
			//force automaticShaping to false
			automaticShaping = false;
		}
	}
	//
	switch (shapeType)
	{
		case SPHERE:
		if (automaticShaping)
		{
			//modify radius
			dim1 = modelRadius;
		}
		collisionShape = new BulletSphereShape(dim1);
		break;
		case PLANE:
		if (automaticShaping)
		{
			//modify normal and d
			dim1 = 0.0;
			dim2 = 0.0;
			dim3 = 1.0;
			dim4 = 0.0;
		}
		collisionShape = new BulletPlaneShape(LVector3(dim1, dim2, dim3),
				dim4);
		break;
		case BOX:
		if (automaticShaping)
		{
			//modify half dimensions
			dim1 = modelDims.get_x() / 2.0;
			dim2 = modelDims.get_y() / 2.0;
			dim3 = modelDims.get_z() / 2.0;
		}
		collisionShape = new BulletBoxShape(LVector3(dim1, dim2, dim3));
		break;
		case CYLINDER:
		if (automaticShaping)
		{
			//modify radius and height
			if (upAxis == X_up)
			{
				dim1 = getDim(shapeSize, modelDims.get_y(), modelDims.get_z());
				dim2 = modelDims.get_x();
			}
			else if (upAxis == Y_up)
			{
				dim1 = getDim(shapeSize, modelDims.get_x(), modelDims.get_z());
				dim2 = modelDims.get_y();
			}
			else
			{
				dim1 = getDim(shapeSize, modelDims.get_x(), modelDims.get_y());
				dim2 = modelDims.get_z();
			}
		}
		collisionShape = new BulletCylinderShape(dim1, dim2, upAxis);
		break;
		case CAPSULE:
		if (automaticShaping)
		{
			//modify radius and height
			if (upAxis == X_up)
			{
				dim1 = getDim(shapeSize, modelDims.get_y(), modelDims.get_z());
				dim2 = modelDims.get_x() - 2 * dim1;
			}
			else if (upAxis == Y_up)
			{
				dim1 = getDim(shapeSize, modelDims.get_x(), modelDims.get_z());
				dim2 = modelDims.get_y() - 2 * dim1;
			}
			else
			{
				dim1 = getDim(shapeSize, modelDims.get_x(), modelDims.get_y());
				dim2 = modelDims.get_z() - 2 * dim1;
			}
		}
		if (dim2 <= 0.0)
		{
			dim2 = 0.0;
		}
		collisionShape = new BulletCapsuleShape(dim1, dim2, upAxis);
		break;
		case CONE:
		if (automaticShaping)
		{
			//modify radius and height
			if (upAxis == X_up)
			{
				dim1 = getDim(shapeSize, modelDims.get_y(), modelDims.get_z());
				dim2 = modelDims.get_x();
			}
			else if (upAxis == Y_up)
			{
				dim1 = getDim(shapeSize, modelDims.get_x(), modelDims.get_z());
				dim2 = modelDims.get_y();
			}
			else
			{
				dim1 = getDim(shapeSize, modelDims.get_x(), modelDims.get_y());
				dim2 = modelDims.get_z();
			}
		}
		collisionShape = new BulletConeShape(dim1, dim2, upAxis);
		break;
		case HEIGHTFIELD:
		collisionShape = new BulletHeightfieldShape(heightfieldFile, 1.0, upAxis);
		break;
		case TRIANGLEMESH:
		{
			//see: https://www.panda3d.org/forums/viewtopic.php?t=13981
			BulletTriangleMesh* triMesh = new BulletTriangleMesh();
			//add geoms from geomNodes to the mesh
			for (int i = 0; i < geomNodes.get_num_paths(); ++i)
			{
				SMARTPTR(GeomNode) geomNode = DCAST(GeomNode,
						geomNodes.get_path(i).node());
				//BulletShape::set_local_scale doesn't work anymore
				//see: https://www.panda3d.org/forums/viewtopic.php?f=9&t=10231&start=690#p93583
				CSMARTPTR(TransformState) ts = (geomNode->get_transform()->
						compose(TransformState::make_scale(
							modelNP.get_net_transform()->get_scale()))).p();
				GeomNode::Geoms geoms = geomNode->get_geoms();
				for (int j = 0; j < geoms.get_num_geoms(); ++j)
				{
					triMesh->add_geom(geoms.get_geom(j), true, ts.p());
				}
			}
			collisionShape = new BulletTriangleMeshShape(triMesh, dynamic);
		}
		break;
		//
		default:
		break;
	}
	//
	return collisionShape;
}

void GamePhysicsManager::getBoundingDimensions(NodePath modelNP,
		LVector3& modelDims, LVector3& modelDeltaCenter, float& modelRadius)
{
	//get "tight" dimensions of model
	LPoint3 minP, maxP;
	modelNP.calc_tight_bounds(minP, maxP);
	//
	LVecBase3 delta = maxP - minP;
	//
	modelDims = LVector3(abs(delta.get_x()), abs(delta.get_y()),
			abs(delta.get_z()));
	modelDeltaCenter = -(minP + delta / 2.0);
	modelRadius = max(max(modelDims.get_x(), modelDims.get_y()),
			modelDims.get_z()) / 2.0;
}

float GamePhysicsManager::getDim(ShapeSize shapeSize, float d1, float d2)
{
	float dim;
	if (shapeSize == MINIMUN)
	{
		dim = min(d1, d2) / 2.0;
	}
	else if (shapeSize == MAXIMUM)
	{
		dim = max(d1, d2) / 2.0;
	}
	else
	{
		dim = (d1 + d2) / 4.0;
	}
	//
	return dim;
}

#ifdef ELY_DEBUG
NodePath GamePhysicsManager::getDebugNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mBulletDebugNodePath;
}

void GamePhysicsManager::initDebug(WindowFramework* windowFramework)
{
	mBulletDebugNodePath.reparent_to(windowFramework->get_render());
	SMARTPTR(BulletDebugNode) bulletDebugNode =
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
