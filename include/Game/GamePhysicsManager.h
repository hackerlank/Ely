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
 * \date 2012-07-07 
 * \author consultit
 */

#ifndef GAMEPHYSICSMANAGER_H_
#define GAMEPHYSICSMANAGER_H_

#include "Utilities/Tools.h"
#include <list>
#include <bulletWorld.h>
#include <windowFramework.h>
#include "ObjectModel/Component.h"

namespace ely
{
/**
 * \brief Singleton manager updating attributes of physics components.
 *
 * Prepared for multi-threading.
 * .
 * This manager could throw events when objects collide. By default this
 * feature is disabled.\n
 * It throws:
 * - when two objects collide, the event "<CollidingObjectType1>_<CollidingObjectType2>_Collision",
 * with the two type names ordered in alphabetical ascending order (A to Z)
 * (i.e. using the std::string::operator<()).
 * This event is thrown continuously at a frequency which is the minimum between the fps and
 * the frequency specified (which defaults to 30 times per seconds) until
 * the object keeps overlapping
 * - when the two objects stop collide, the event
 * "<CollidingObjectType1>_<CollidingObjectType2>_CollisionOff"; this event is thrown only once\n
 * The first argument of each event is a reference of the overlapping object's
 * Physics component, the second argument is a reference to this component.
 *
 */
class GamePhysicsManager: public Singleton<GamePhysicsManager>
{
public:
	/**
	 * \brief Constructor.
	 * @param sort The task sort.
	 * @param priority The task priority.
	 * @param asyncTaskChain If ELY_THREAD is defined this indicates if
	 * this manager should run in another async task chain.
	 */
	GamePhysicsManager(
#ifdef ELY_THREAD
			Mutex& managersMutex, ConditionVarFull& managersVar,
			const unsigned long int completedMask,
			const unsigned long int exiting,
			unsigned long int& completedTasks,
#endif
			int sort = 0, int priority = 0,
			const std::string& asyncTaskChain = std::string(""));
	virtual ~GamePhysicsManager();

	/**
	 * \brief Adds (if not present) a physics component to updating.
	 * @param physicsComp The physics component.
	 */
	void addToPhysicsUpdate(SMARTPTR(Component)physicsComp);
	/**
	 * \brief Removes (if present) a physics component from updating.
	 * @param physicsComp The physics component.
	 */
	void removeFromPhysicsUpdate(SMARTPTR(Component) physicsComp);

	/**
	 * \brief Gets a reference to the Bullet world.
	 * @return The Bullet world.
	 */
	SMARTPTR(BulletWorld) bulletWorld() const;

	/**
	 * \brief Updates step simulation and physics components.
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

#ifdef ELY_DEBUG
	/**
	 * \brief Gets a reference to the Bullet Debug node.
	 * @return The Bullet Debug node.
	 */
	NodePath getDebugNodePath() const;

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

	///Helper functions, data structures
	///@{
	/**
	 * \brief Shape type.
	 */
	enum ShapeType
	{
		SPHERE, //!< SPHERE (radius)
		PLANE,//!< PLANE (norm_x, norm_y, norm_z, d)
		BOX,//!< BOX (half_x, half_y, half_z)
		CYLINDER,//!< CYLINDER (radius, height, up)
		CAPSULE,//!< CAPSULE (radius, height, up)
		CONE,//!< CONE (radius, height, up)
		HEIGHTFIELD,//!< HEIGHTFIELD (image, height, up, scale_w, scale_d)
		TRIANGLEMESH,//!< TRIANGLEMESH (dynamic)
	};
	/**
	 * \brief Shape size.
	 */
	enum ShapeSize
	{
		MINIMUN, //!< MINIMUN shape
		MAXIMUM,//!< MAXIMUM shape
		MEDIUM,//!< MEDIUM shape
	};
	/**
	 * \brief Creates a wrapping bullet shape for a given model node path.
	 *
	 * If automaticShaping is true this method builds a shape based on a
	 * tight bounding box around the model, with the shape's tightness
	 * controlled by the shapeSize parameter.\n
	 * If automaticShaping is false shape is built according to this scheme:
	 * - BulletSphereShape(dim1)
	 * - BulletPlaneShape(LVector3f(dim1, dim2, dim3), dim4)
	 * - BulletBoxShape(LVector3f(dim1, dim2, dim3))
	 * - BulletCylinderShape(dim1, dim2, upAxis)
	 * - BulletCapsuleShape(dim1, dim2, upAxis)
	 * - BulletConeShape(dim1, dim2, upAxis)
	 * - BulletHeightfieldShape(heightfieldFile, dim1, upAxis)
	 * - BulletTriangleMeshShape (dynamic)
	 *
	 * @param modelNP The model node path.
	 * @param shapeType The shape type.
	 * @param shapeSize The shape size, i.e. its tightness around the model.
	 * @param modelDims Returns the model bounding box dimensions for each axis
	 * (out parameter).
	 * @param modelDeltaCenter Returns the middle point of the model bounding box
	 * (out parameter).
	 * @param modelRadius Returns the radius of the model bounding box
	 * (out parameter).
	 * @param dim1 Returns/sets the first shape parameter (in/out parameter).
	 * @param dim2 Returns/sets the second shape parameter (in/out parameter).
	 * @param dim3 Returns/sets the third shape parameter (in/out parameter).
	 * @param dim4 Returns/sets the fourth shape parameter (in/out parameter).
	 * @param automaticShaping If true the dimXs are output parameters,
	 * otherwise input parameter.
	 * @param upAxis The up axis.
	 * @param heightfieldFile The height field file.
	 * @return A (smart) pointer to the created shape.
	 */
	SMARTPTR(BulletShape) createShape(NodePath modelNP, ShapeType shapeType,
			ShapeSize shapeSize, LVecBase3f& modelDims, LVector3f& modelDeltaCenter,
			float& modelRadius, float& dim1, float& dim2, float& dim3, float& dim4,
			bool automaticShaping = true, BulletUpAxis upAxis=Z_up,
			const Filename& heightfieldFile = Filename(""), bool dynamic = false);
	/**
	 * \brief Calculates geometric characteristics of a GeomNode.
	 *
	 * It takes a NodePath, (supposedly) referring to a GeomNode, and
	 * calculates a tight AABB surrounding it, hence sets the
	 * related dimensions into mModelDims, mModelCenter, mModelRadius
	 * member variables.\n
	 * \note Remember that AABB takes into account model rotation/orientation
	 * so to get a bounding box of the model into the reference pose, no
	 * rotation/orientation should be applied to the model node path.\n
	 *
	 * @param modelNP The model node path.
	 * @param modelDims Returns the model AABB dimensions for each axis.
	 * @param modelDeltaCenter Returns -(MAXP + minP)/2.0 (MAXP/minP=max/min point
	 * of the model AABB).
	 * @param modelRadius Returns the radius of the model AABB.
	 */
	void getBoundingDimensions(NodePath modelNP, LVecBase3f& modelDims,
			LVector3f& modelDeltaCenter, float& modelRadius);
	/**
	 * \brief Calculates the desired dimension given the shape size.
	 *
	 * @param shapeSize
	 * @param d1 First dimension.
	 * @param d2 Second dimension.
	 * @return The desired dimension
	 */
	float getDim(ShapeSize shapeSize, float d1, float d2);
	///@}

	/**
	 * \brief Gets physics component given the (underlying) Bullet PandaNode.
	 *
	 * @param pandaNode The (underlying) Bullet PandaNode.
	 * @return The physics component.
	 */
	SMARTPTR(Component) getPhysicsComponentByPandaNode(SMARTPTR(PandaNode) pandaNode);

	/**
	 * \brief Sets physics component given the (underlying) Bullet PandaNode.
	 *
	 * @param pandaNode The (underlying) Bullet PandaNode.
	 * @param physicsComponent The physics component, if set to NULL the
	 * component related to pandaNode will be erased.
	 */
	void setPhysicsComponentByPandaNode(SMARTPTR(PandaNode) pandaNode,
			SMARTPTR(Component) physicsComponent);

	///Thrown events.
	enum EventThrown
	{
		COLLISIONNOTIFY
	};

	/**
	 * \brief Enable/disable collisions' notification through events.
	 *
	 */
	/**
	 * \brief Enable/disable collisions' notification through events.
	 * @param enable Enabling flag.
	 */
	void enableCollisionNotify(EventThrown event,
			ThrowEventData eventData);


private:
	/// Bullet world.
	SMARTPTR(BulletWorld) mBulletWorld;

#ifdef ELY_DEBUG
	/// Bullet Debug node path.
	NodePath mBulletDebugNodePath;
#endif

	///@{
	///List of physics components to be updated.
	typedef std::list<SMARTPTR(Component)> PhysicsComponentList;
	PhysicsComponentList mPhysicsComponents;
	///@}

	///Table of all physics components indexed by (underlying) Bullet PandaNodes.
	///This is used, for example, during ray casting.
	std::map<SMARTPTR(PandaNode), SMARTPTR(Component)> mPhysicsComponentPandaNodeTable;

	///@{
	///A task data for step simulation update.
	SMARTPTR(TaskInterface<GamePhysicsManager>::TaskData) mUpdateData;
	SMARTPTR(AsyncTask) mUpdateTask;
	///@}

	/**
	 * \name Collision notification  through events.
	 */
	///@{
	struct CollidingNodePair
	{
		struct CollidingNodePairData
		{
			std::pair<PandaNode*, PandaNode*> mPnode;
			std::string mEventName;
			EventParameter mEventParameters[2];
			int mCount;
		};
		//main constructors
		CollidingNodePair(PandaNode *_pnode0, PandaNode *_pnode1) :
				mCollidingNodePairData(new CollidingNodePairData), mRefCount(new int)
		{
			// always create the pair in a predictable order
			// (use the pointer value..)
			if (_pnode0 > _pnode1)
			{
				mCollidingNodePairData->mPnode = std::make_pair(_pnode1, _pnode0);
			}
			else
			{
				mCollidingNodePairData->mPnode = std::make_pair(_pnode0, _pnode1);
			}
			*mRefCount = 1;
		}
		CollidingNodePair(PandaNode *_pnode0, PandaNode *_pnode1, const std::string& _name, int _count) :
				mCollidingNodePairData(new CollidingNodePairData), mRefCount(new int)
		{
			// always create the pair in a predictable order
			// (use the pointer value..)
			if (_pnode0 > _pnode1)
			{
				mCollidingNodePairData->mPnode = std::make_pair(_pnode1, _pnode0);
			}
			else
			{
				mCollidingNodePairData->mPnode = std::make_pair(_pnode0, _pnode1);
			}
			mCollidingNodePairData->mEventName = _name;
			mCollidingNodePairData->mCount = _count;
			*mRefCount = 1;
		}
		//copy constructor
		CollidingNodePair(const CollidingNodePair& on) :
				mCollidingNodePairData(on.mCollidingNodePairData), mRefCount(on.mRefCount)
		{
			++(*mRefCount);
		}
		//destructor
		~CollidingNodePair()
		{
			if (--(*mRefCount) == 0)
			{
				delete mCollidingNodePairData;
				delete mRefCount;
			}
		}
		//assignment operator
		CollidingNodePair& operator=(const CollidingNodePair& on)
		{
			mCollidingNodePairData = on.mCollidingNodePairData;
			++(*mRefCount);
			return *this;
		}
		//comparison operator
		bool operator<(const CollidingNodePair& on) const
		{
			return mCollidingNodePairData->mPnode < on.mCollidingNodePairData->mPnode;
		}
		//owned resource
		CollidingNodePairData* mCollidingNodePairData;
	private:
		int* mRefCount;
	};
	ThrowEventData mCollisionNotify;
	std::set<CollidingNodePair> mCollidingNodePairs;
	btCollisionDispatcher* mCollisionDispatcher;
	///Helper.
	void doEnableCollisionNotify(EventThrown event, ThrowEventData eventData);
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
	///The mutex associated with table of all physics components.
	ReMutex mTableMutex;
#endif
};

///inline definitions

inline SMARTPTR(Component) GamePhysicsManager::getPhysicsComponentByPandaNode(
		SMARTPTR(PandaNode) pandaNode)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mTableMutex)

	return mPhysicsComponentPandaNodeTable[pandaNode];
}

inline void GamePhysicsManager::setPhysicsComponentByPandaNode(SMARTPTR(PandaNode) pandaNode,
		SMARTPTR(Component) physicsComponent)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mTableMutex)

	if (physicsComponent)
	{
		mPhysicsComponentPandaNodeTable[pandaNode] = physicsComponent;
	}
	else
	{
		mPhysicsComponentPandaNodeTable.erase(pandaNode);
	}
}

inline void GamePhysicsManager::enableCollisionNotify(EventThrown event, ThrowEventData eventData)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	doEnableCollisionNotify(event, eventData);
}

#ifdef ELY_THREAD
inline ReMutex& GamePhysicsManager::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* GAMEPHYSICSMANAGER_H_ */
