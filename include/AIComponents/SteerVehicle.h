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

///Steer event.
enum SteerEvent
{
	PATHFOLOWINGEVENT,
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
 * If enabled (with "throw_events"), this component will throw an event on starting to move
 * ("OnStartSteerVehicle"), and an event on stopping to move
 * ("OnStopSteerVehicle"). The second argument of both is a reference
 * to the owner object.\n
 * If specified in "steer_events" (regardless of "throw_events"),
 * this component will throw events when steering is required to:
 * - follow path ("OnPathFollowing")
 * - avoid obstacle ("OnAvoidObstacle")
 * - avoid close neighbors (i.e. when there is a collision) ("OnAvoidCloseNeighbor")
 * - avoid neighbors (i.e. when there is a potential collision) ("OnAvoidNeighbor")
 * The second argument of them is a reference to the owner object.\n
 * \see annotate* SteerLibraryMixin member functions in SteerLibrary.h for more information.
 *
 * \note debug drawing works correctly only if the owner object's
 * parent is "render".\n
 *
 * XML Param(s):
 * - "throw_events"				|single|"false"
 * - "throw_steer_events"		|single|"false"
 * - "steer_events"				|single|no default (specified as "event1[:event2[:event3[:event4]]]" with eventX = path_following|avoid_obstacle|avoid_close_neighbor|avoid_neighbor)
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
	 * \brief Enables throwing events.
	 * @param enable True to enable, false to disable.
	 */
	void enableThrowEvents(bool enable);

	/**
	 * \brief Enables throwing steer events.
	 * @param enable True to enable, false to disable.
	 */
	void enableThrowSteerEvents(bool enable);

	/**
	 * \brief Enables/disables the steer event to be thrown.
	 * @param event The steer event.
	 * @param enable True to enable, false to disable.
	 */
	void enableSteerEvent(SteerEvent event, bool enable);

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

	///Throwing events.
	bool mThrowEvents, mSteerVehicleStartSent, mSteerVehicleStopSent;

	///Throwing steer events.
	bool mThrowSteerEvents;
	bool mPathFollowing, mPathFollowingSent;
	bool mAvoidObstacle, mAvoidObstacleSent;
	bool mAvoidCloseNeighbor, mAvoidCloseNeighborSent;
	bool mAvoidNeighbor, mAvoidNeighbor;

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
	mThrowEvents = mSteerVehicleStartSent = mSteerVehicleStopSent = false;
	mThrowSteerEvents = false;
	mPathFollowing = mPathFollowingSent = false;
	mAvoidObstacle = mAvoidObstacleSent = false;
	mAvoidCloseNeighbor = mAvoidCloseNeighborSent = false;
	mAvoidNeighbor = mAvoidNeighbor = false;
}

inline OpenSteer::AbstractVehicle& SteerVehicle::getAbstractVehicle()
{
	return *mVehicle;
}

inline SteerVehicle::operator OpenSteer::AbstractVehicle&()
{
	return *mVehicle;
}

inline void SteerVehicle::enableThrowEvents(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mThrowEvents = enable;
}

inline void SteerVehicle::enableThrowSteerEvents(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mThrowSteerEvents = enable;
}

inline void SteerVehicle::enableSteerEvent(SteerEvent event, bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	switch (event)
	{
	case PATHFOLOWINGEVENT:
		enable ? mPathFollowing = true : mPathFollowing = false;
		break;
	case AVOIDOBSTACLEEVENT:
		enable ? mAvoidObstacle = true : mAvoidObstacle = false;
		break;
	case AVOIDCLOSENEIGHBOREVENT:
		enable ? mAvoidCloseNeighbor = true : mAvoidCloseNeighbor = false;
		break;
	case AVOIDNEIGHBOREVENT:
		enable ? mAvoidNeighbor = true : mAvoidNeighbor = false;
		break;
	default:
		break;
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
