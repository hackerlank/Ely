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
 * \file /Ely/include/AIComponents/SteerVehicle.h
 *
 * \date 04/dic/2013 (09:20:41)
 * \author consultit
 */
#ifndef STEERVEHICLE_H_
#define STEERVEHICLE_H_

#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "OpenSteerLocal/common.h"
#include <bulletWorld.h>
#include <bulletClosestHitRayResult.h>
#include <throw_event.h>

namespace ely
{

class SteerVehicleTemplate;
class SteerPlugIn;

///SteerVehicle movement type.
enum SteerVehicleMovType
{
	OPENSTEER,
	OPENSTEER_KINEMATIC,
	VehicleMovType_NONE
};

///Throw event data.
struct ThrowEventData
{
	ThrowEventData() :
			mEnable(false), mEventName(std::string("")), mFrameCount(0), mDeltaFrame(
					1)
	{
	}
	bool mEnable;
	std::string mEventName;
	int mFrameCount;
	int mDeltaFrame;
};

///SteerVehicle event.
enum SteerVehicleEvent
{
	STARTEVENT,
	STOPEVENT,
	PATHFOLLOWINGEVENT,
	AVOIDOBSTACLEEVENT,
	AVOIDCLOSENEIGHBOREVENT,
	AVOIDNEIGHBOREVENT
};

/**
 * \brief Component implementing OpenSteer Vehicles.
 *
 * \see http://opensteer.sourceforge.net
 *
 * This component should be associated to a "Scene" component.\n
 * If specified in "thrown_events", this component can throw
 * these events (shown with default names):
 * - on starting to move (<ObjectId>_SteerVehicle_Start)
 * - on stopping to move (<ObjectId>_SteerVehicle_Stop)
 * - when steering is required to follow a path
 * (<ObjectId>_SteerVehicle_PathFollowing)
 * - when steering is required to avoid an obstacle
 * (<ObjectId>_SteerVehicle_AvoidObstacle)
 * - when steering is required to avoid a close neighbor (i.e. when
 * there is a collision) (<ObjectId>_SteerVehicle_AvoidCloseNeighbor)
 * - when steering is required to avoid a neighbor (i.e. when there
 * is a potential collision) (<ObjectId>_SteerVehicle_AvoidNeighbor)
 * The first argument of them is a reference to this component,
 * the second one is a reference to the owner object.\n
 * \see annotate* SteerLibraryMixin member functions in SteerLibrary.h
 * for more information.
 *
 * \note debug drawing works correctly only if the owner object's
 * parent is "render".\n
 *
 * XML Param(s):
 * - "thrown_events"			|single|no default (specified as "event1[@event_name1[@delta_frame1]][:...[:eventN[@event_nameN[@delta_frameN]]]]" with eventX = start|stop|path_following|avoid_obstacle|avoid_close_neighbor|avoid_neighbor)
 * - "type"						|single|"one_turning" (values: one_turning|pedestrian)
 * - "external_update"			|single|"false"
 * - "add_to_plugin"			|single|no default
 * - "mov_type"					|single|"opensteer" (values: opensteer|kinematic)
 * - "mass"						|single|"1.0"
 * - "radius"					|single|no default
 * - "speed"					|single|"0.0"
 * - "max_force"				|single|"0.1"
 * - "max_speed"				|single|"1.0"
 * - "ray_mask"					|single|"all_on"
 *
 * \note parts inside [] are optional.\n
 */
class SteerVehicle: public Component
{
protected:
	friend class SteerVehicleTemplate;
	friend class SteerPlugIn;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	SteerVehicle();
	SteerVehicle(SMARTPTR(SteerVehicleTemplate)tmpl);
	virtual ~SteerVehicle();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \name Getters/setters of SteerVehicle default settings.
	 */
	///@{
	void setSettings(const VehicleSettings& settings);
	VehicleSettings getSettings();
	///@}

	/**
	 * \name AbstractVehicle reference getter & conversion function.
	 */
	///@{
	OpenSteer::AbstractVehicle& getAbstractVehicle();
	operator OpenSteer::AbstractVehicle&();
	///@}

	/**
	 * \brief Enables/disables the SteerVehicle event to be thrown.
	 * @param event The steer event.
	 * @param eventData The steer event data. ThrowEventData::mEnable
	 * will enable/disable the event.
	 */
	void enableSteerVehicleEvent(SteerVehicleEvent event, ThrowEventData eventData);

#ifdef ELY_THREAD
	/**
	 * \brief Get the SteerPlugIn object reference mutex.
	 * @return The SteerPlugIn mutex.
	 */
	ReMutex& getSteerPlugInMutex();
#endif

private:
	///Current underlying Vehicle.
	OpenSteer::AbstractVehicle* mVehicle;
	///The SteerPlugIn owner object.
	SMARTPTR(SteerPlugIn) mSteerPlugIn;
	ObjectId mSteerPlugInObjectId;
	///Input radius.
	float mInputRadius;
	///The movement type.
	SteerVehicleMovType mMovType;
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

	///Called by the underlying OpenSteer component update.
	///@{
	void doUpdateSteerVehicle(const float currentTime, const float elapsedTime);
	//Called when component is updated outside of OpenSteer.
	void doExternalUpdateSteerVehicle(const float currentTime, const float elapsedTime);
	bool mExternalUpdate;
	///@}

	/**
	 * \name Throwing SteerVehicle events.
	 */
	///@{
	ThrowEventData mStart, mStop, mPathFollowing, mAvoidObstacle,
	mAvoidCloseNeighbor, mAvoidNeighbor;
	void doThrowPathFollowing(const OpenSteer::Vec3& future,
	const OpenSteer::Vec3& onPath, const OpenSteer::Vec3& target,
	const float outside);
	void doThrowAvoidObstacle(const float minDistanceToCollision);
	void doThrowAvoidCloseNeighbor(const OpenSteer::AbstractVehicle& other,
	const float additionalDistance);
	void doThrowAvoidNeighbor(const OpenSteer::AbstractVehicle& threat,
	const float steer, const OpenSteer::Vec3& ourFuture,
	const OpenSteer::Vec3& threatFuture);
	///Helper.
	void doEnableSteerVehicleEvent(SteerVehicleEvent event, ThrowEventData eventData);
	///@}

#ifdef ELY_THREAD
	///Protects the SteerPlugIn object reference (mNavMesh).
	ReMutex mSteerPlugInMutex;
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
		register_type(_type_handle, "SteerVehicle", Component::get_class_type());
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

inline void SteerVehicle::reset()
{
	//
	mVehicle = NULL;
	mSteerPlugInObjectId = ObjectId();
	mInputRadius = 0.0;
	mMovType = OPENSTEER;
	mBulletWorld.clear();
	mMaxError = 0.0;
	mDeltaRayDown = mDeltaRayOrig = LVector3f::zero();
	mHitResult = BulletClosestHitRayResult::empty();
	mRayMask = BitMask32::all_off();
	mCorrectHeightRigidBody = 0.0;
	mExternalUpdate = false;
	mStart = mStop = mPathFollowing = mAvoidObstacle =
			mAvoidCloseNeighbor = mAvoidNeighbor = ThrowEventData();
}

inline OpenSteer::AbstractVehicle& SteerVehicle::getAbstractVehicle()
{
	return *mVehicle;
}

inline SteerVehicle::operator OpenSteer::AbstractVehicle&()
{
	return *mVehicle;
}

inline void SteerVehicle::enableSteerVehicleEvent(SteerVehicleEvent event, ThrowEventData eventData)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	doEnableSteerVehicleEvent(event, eventData);
}

inline void SteerVehicle::doThrowPathFollowing(const OpenSteer::Vec3& future,
		const OpenSteer::Vec3& onPath, const OpenSteer::Vec3& target,
		const float outside)
{
	int frameCount = ClockObject::get_global_clock()->get_frame_count();
	if (frameCount > mPathFollowing.mFrameCount + mPathFollowing.mDeltaFrame)
	{
		//enough frames are passed: throw the event
		throw_event(mPathFollowing.mEventName, EventParameter(this),
				EventParameter(std::string(mOwnerObject->objectId())));
		//update frame count
		mPathFollowing.mFrameCount = frameCount;
	}
}

inline void SteerVehicle::doThrowAvoidObstacle(const float minDistanceToCollision)
{
	int frameCount = ClockObject::get_global_clock()->get_frame_count();
	if (frameCount > mAvoidObstacle.mFrameCount + mAvoidObstacle.mDeltaFrame)
	{
		//enough frames are passed: throw the event
		throw_event(mAvoidObstacle.mEventName, EventParameter(this),
				EventParameter(std::string(mOwnerObject->objectId())));
		//update frame count
		mAvoidObstacle.mFrameCount = frameCount;
	}
}

inline void SteerVehicle::doThrowAvoidCloseNeighbor(const OpenSteer::AbstractVehicle& other,
		const float additionalDistance)
{
	int frameCount = ClockObject::get_global_clock()->get_frame_count();
	if (frameCount > mAvoidCloseNeighbor.mFrameCount + mAvoidCloseNeighbor.mDeltaFrame)
	{
		//enough frames are passed: throw the event
		throw_event(mAvoidCloseNeighbor.mEventName, EventParameter(this),
				EventParameter(std::string(mOwnerObject->objectId())));
		//update frame count
		mAvoidCloseNeighbor.mFrameCount = frameCount;
	}
}

inline void SteerVehicle::doThrowAvoidNeighbor(const OpenSteer::AbstractVehicle& threat,
		const float steer, const OpenSteer::Vec3& ourFuture,
		const OpenSteer::Vec3& threatFuture)
{
	int frameCount = ClockObject::get_global_clock()->get_frame_count();
	if (frameCount > mAvoidNeighbor.mFrameCount + mAvoidNeighbor.mDeltaFrame)
	{
		//enough frames are passed: throw the event
		throw_event(mAvoidNeighbor.mEventName, EventParameter(this),
				EventParameter(std::string(mOwnerObject->objectId())));
		//update frame count
		mAvoidNeighbor.mFrameCount = frameCount;
	}
}

#ifdef ELY_THREAD
inline ReMutex& SteerVehicle::getSteerPlugInMutex()
{
	return mSteerPlugInMutex;
}
#endif

} /* namespace ely */

#endif /* STEERVEHICLE_H_ */
