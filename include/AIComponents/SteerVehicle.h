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
#include <OpenSteer/SimpleVehicle.h>

namespace ely
{

class SteerVehicleTemplate;
template<typename Super> class SimpleVehicleMixin;
typedef SimpleVehicleMixin<OpenSteer::SimpleVehicle> SimpleVehicle;;

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
 * Ife enabled, this component will throw an event on starting to move, and
 * an event on stopping to move. The second argument of both is a reference
 * to the owner object.\n
 * This component can throw (if enabled) "OnStartCrowdAgent" and "OnStopCrowdAgent"
 * events.
 *
 * XML Param(s):
 * - "throw_events"				|single|"false"
 * - "mov_type"					|single|"opensteer" (values: opensteer|kinematic)
 * - "type"						|single|"one_turning" (values: one_turning|)
 * - "mass"						|single|"1.0"
 * - "radius"					|single|no default
 * - "speed"					|single|"0.0"
 * - "max_force"				|single|"0.1"
 * - "max_speed"				|single|"1.0"
 *
 * \note parts inside [] are optional.\n
 */
class SteerVehicle: public Component
{
protected:
	friend class SteerVehicleTemplate;

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
	 * \brief Updates OpenSteer underlying component.
	 *
	 * Will be called automatically by an ai manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \name SimpleVehicle reference getter & conversion function.
	 */
	///@{
	SimpleVehicle& getSimpleVehicle();
	operator SimpleVehicle&();
	///@}

private:
	///Current underlying Vehicle.
	SimpleVehicle* mVehicle;
	///Input radius.
	float mInputRadius;
	///The movement type.
	VehicleMovTypeEnum mMovType;

	///Throwing events.
	bool mThrowEvents, mVehicleStartSent, mVehicleStopSent;

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
	mInputRadius = 0.0;
	mMovType = OPENSTEER;
	mThrowEvents = mVehicleStartSent = mVehicleStopSent = false;
}

inline void SteerVehicle::onAddToSceneSetup()
{
}

inline void SteerVehicle::onRemoveFromSceneCleanup()
{
}

inline SimpleVehicle& SteerVehicle::getSimpleVehicle()
{
	return *mVehicle;
}

inline SteerVehicle::operator SimpleVehicle&()
{
	return *mVehicle;
}

} /* namespace ely */

#endif /* STEERVEHICLE_H_ */
