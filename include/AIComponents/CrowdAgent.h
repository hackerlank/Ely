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
 * \file /Ely/include/AIComponents/CrowdAgent.h
 *
 * \date 2013-06-06 
 * \author consultit
 */

#ifndef CROWDAGENT_H_
#define CROWDAGENT_H_

#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include <DetourCrowd.h>
#include <throw_event.h>
#include <bulletWorld.h>
#include <bulletClosestHitRayResult.h>

namespace ely
{

class CrowdAgentTemplate;
class NavMesh;

///CrowdAgent movement type.
enum CrowdAgentMovType
{
	RECAST,
	RECAST_KINEMATIC,
	AgentMovType_NONE
};

/**
 * \brief Component implementing dtCrowdAgent from Recast Navigation library.
 *
 * \see
 * 		- https://github.com/recastnavigation/recastnavigation.git
 * 		- http://digestingduck.blogspot.it
 * 		- https://groups.google.com/forum/?fromgroups#!forum/recastnavigation
 *
 * This component should be associated to a "Scene" component.\n
 * It could be associated to a kinematic rigid body, if the associated NavMesh
 * movement type is "kinematic".\n
 * If specified in "thrown_events", this component can throw
 * these events (shown with default names):
 * - on moving (<ObjectType>_CrowdAgent_Move)
 * - on being steady (<ObjectType>_CrowdAgent_Steady)
 * Events are thrown continuously at a frequency which is the minimum between
 * the fps and the frequency specified (which defaults to 30 times per seconds).\n
 * The argument of each event is a reference to this component.\n
 *
 * \note the owner object of this component will be reparented (if necessary)
 * when added to a NavMesh, to the same reference node (i.e. parent) of
 * the NavMesh owner object.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *thrown_events*				|single| - | specified as "event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]" with eventX = move,steady
 * | *add_to_navmesh*				|single| - | -
 * | *mov_type*						|single| *recast* | values: recast,kinematic
 * | *move_target*					|single| 0.0,0.0,0.0 | -
 * | *move_velocity*				|single| 0.0,0.0,0.0 | -
 * | *max_acceleration*				|single| 8.0 | -
 * | *max_speed*					|single| 3.5 | -
 * | *collision_query_range*		|single| 12.0 | * NavMesh::agent_radius
 * | *path_optimization_range*		|single| 30.0 | * NavMesh::agent_radius
 * | *separation_weight* 			|single| 2.0 | -
 * | *update_flags*					|single| *0x1b* | -
 * | *obstacle_avoidance_type*		|single| *3* | values: 0,1,2,3
 * | *ray_mask*						|single| *all_on* | -
 *
 * \note parts inside [] are optional.\n
 */
class CrowdAgent: public Component
{
protected:
	friend class CrowdAgentTemplate;
	friend class NavMesh;

	CrowdAgent(SMARTPTR(CrowdAgentTemplate)tmpl);
	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	virtual ~CrowdAgent();

	/**
	 * \name Helper getters/setters of CrowdAgent data.
	 *
	 * \note Getters/setters from NavMesh should be used for
	 * performance and detailed return result.
	 */
	///@{
	void setParams(const dtCrowdAgentParams& agentParams);
	dtCrowdAgentParams getParams();
	void setMoveTarget(const LPoint3f& pos);
	LPoint3f getMoveTarget();
	void setMoveVelocity(const LVector3f& vel);
	LVector3f getMoveVelocity();
	void setMovType(CrowdAgentMovType movType);
	CrowdAgentMovType getMovType() const;
	///@}

	/**
	 * \name Get the NavMesh owner object.
	 *
	 * \return The NavMesh object.
	 */
	SMARTPTR(NavMesh) getNavMesh() const;

	///CrowdAgent thrown events.
	enum EventThrown
	{
		MOVEEVENT,
		STEADYEVENT
	};

	/**
	 * \brief Enables/disables the CrowdAgent event to be thrown.
	 * @param event The CrowdAgent event.
	 * @param eventData The CrowdAgent event data. ThrowEventData::mEnable
	 * will enable/disable the event.
	 */
	void enableCrowdAgentEvent(EventThrown event, ThrowEventData eventData);

#ifdef ELY_THREAD
	/**
	 * \brief Get the NavMesh object reference mutex.
	 * @return The NavMesh mutex.
	 */
	ReMutex& getNavMeshMutex();
#endif

private:

	///The NavMesh owner object.
	SMARTPTR(NavMesh) mNavMesh;
	ObjectId mNavMeshObjectId;
	///The movement type.
	CrowdAgentMovType mMovType;
	///The CrowdAgent index.
	int mAgentIdx;
	///The associated dtCrowdAgent data.
	///@{
	dtCrowdAgentParams mAgentParams;
	LPoint3f mMoveTarget;
	LVector3f mMoveVelocity;
	///@}
	/**
	 * \brief Physics data.
	 */
	///@{
	SMARTPTR(BulletWorld) mBulletWorld;
	float mMaxError;
	LVector3f mDeltaRayDown, mDeltaRayOrig;
	BulletClosestHitRayResult mHitResult;
	BitMask32 mRayMask;
	float mCorrectHeightRigidBody;
	///@}
	/**
	 * \brief Updates position/velocity/orientation of the controlled object.
	 *
	 * Will be called automatically by the NavMesh update.
	 * @param dt The delta frame time.
	 * @param pos The new position.
	 * @param vel The new velocity.
	 */
	void doUpdatePosDir(float dt, const LPoint3f& pos, const LVector3f& vel);

	/**
	 * \name Throwing CrowdAgent events.
	 */
	///@{
	ThrowEventData mMove, mSteady;
	///Helper.
	void doEnableCrowdAgentEvent(EventThrown event, ThrowEventData eventData);
	void doThrowEvent(ThrowEventData& eventData);
	std::string mThrownEventsParam;
	///@}

	///@{
	///A task data to do asynchronous adding to NavMesh during creation.
	SMARTPTR(TaskInterface<CrowdAgent>::TaskData) mAddData;
	SMARTPTR(AsyncTask) mAddTask;
	AsyncTask::DoneStatus addCrowdAgentAsync(GenericAsyncTask* task);
	SMARTPTR(NavMesh) mStartNavMesh;
#ifdef ELY_THREAD
	std::string mTaskChainName;
#endif
	///@}

#ifdef ELY_THREAD
	///Protects the NavMesh object reference (mNavMesh).
	ReMutex mNavMeshMutex;
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "CrowdAgent", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

///inline definitions

inline void CrowdAgent::reset()
{
	//
	mNavMeshObjectId = ObjectId();
	mMovType = RECAST;
	mAgentIdx = -1;
	mAgentParams = dtCrowdAgentParams();
	mMoveTarget = LPoint3f::zero();
	mMoveVelocity = LVector3f::zero();
	mBulletWorld.clear();
	mMaxError = 0.0;
	mDeltaRayDown = mDeltaRayOrig = LVector3f::zero();
	mRayMask = BitMask32::all_off();
	mCorrectHeightRigidBody = 0.0;
	mMove = mSteady = ThrowEventData();
	mThrownEventsParam.clear();
}

inline dtCrowdAgentParams CrowdAgent::getParams()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAgentParams;
}

inline LPoint3f CrowdAgent::getMoveTarget()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMoveTarget;
}

inline LVector3f CrowdAgent::getMoveVelocity()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMoveVelocity;
}

inline void CrowdAgent::setMovType(CrowdAgentMovType movType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mMovType = movType;
}

inline CrowdAgentMovType CrowdAgent::getMovType() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMovType;
}

inline void CrowdAgent::enableCrowdAgentEvent(EventThrown event,
		ThrowEventData eventData)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	doEnableCrowdAgentEvent(event, eventData);
}

inline void CrowdAgent::doThrowEvent(ThrowEventData& eventData)
{
	if (eventData.mThrown)
	{
		eventData.mTimeElapsed += ClockObject::get_global_clock()->get_dt();
		if (eventData.mTimeElapsed >= eventData.mPeriod)
		{
			//enough time is passed: throw the event
			throw_event(eventData.mEventName, EventParameter(this));
			//update elapsed time
			eventData.mTimeElapsed -= eventData.mPeriod;
		}
	}
	else
	{
		//throw the event
		throw_event(eventData.mEventName, EventParameter(this));
		eventData.mThrown = true;
	}
}

#ifdef ELY_THREAD
inline ReMutex& CrowdAgent::getNavMeshMutex()
{
	return mNavMeshMutex;
}
#endif

///Template

class CrowdAgentTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	CrowdAgentTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~CrowdAgentTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType componentFamilyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "CrowdAgentTemplate",
				ComponentTemplate::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};
}  // namespace ely


#endif /* CROWDAGENT_H_ */
