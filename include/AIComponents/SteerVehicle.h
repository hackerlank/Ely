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

///Vehicle movement type.
enum VehicleMovTypeEnum
{
	OPENSTEER,
	OPENSTEER_KINEMATIC,
	VehicleMovType_NONE
};

/**
 * \brief Component implementing OpenSteer Vehicles.
 *
 * \see http://opensteer.sourceforge.net
 *
 * This component should be associated to a "Scene" component.\n
 * If enabled, this component will throw an event on starting to move
 * ("OnStartSteerVehicle"), and  an event on stopping to move
 * ("OnStopSteerVehicle"). The second argument of both is a reference
 * to the owner object.\n
 * \note debug drawing works correctly only if the owner object's
 * parent is "render".\n
 *
 * XML Param(s):
 * - "throw_events"				|single|"false"
 * - "type"						|single|"one_turning" (values: one_turning|pedestrian)
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
	 * \brief Get the SteerPlugIn object reference mutex.
	 * @return The SteerPlugIn mutex.
	 */
	ReMutex& getSteerPlugInMutex();

private:
	///Current underlying Vehicle.
	OpenSteer::AbstractVehicle* mVehicle;
	///The SteerPlugIn owner object.
	SMARTPTR(SteerPlugIn) mSteerPlugIn;
	ObjectId mSteerPlugInObjectId;
	///Input radius.
	float mInputRadius;
	///The movement type.
	VehicleMovTypeEnum mMovType;
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
	void doUpdateSteerVehicle(const float currentTime, const float elapsedTime);

	///Throwing events.
	bool mThrowEvents, mSteerVehicleStartSent, mSteerVehicleStopSent;

	///Protects the SteerPlugIn object reference (mNavMesh).
	ReMutex mSteerPlugInMutex;

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
	mThrowEvents = mSteerVehicleStartSent = mSteerVehicleStopSent = false;
}

inline OpenSteer::AbstractVehicle& SteerVehicle::getAbstractVehicle()
{
	return *mVehicle;
}

inline SteerVehicle::operator OpenSteer::AbstractVehicle&()
{
	return *mVehicle;
}

inline ReMutex& SteerVehicle::getSteerPlugInMutex()
{
	return mSteerPlugInMutex;
}

} /* namespace ely */

#endif /* STEERVEHICLE_H_ */
