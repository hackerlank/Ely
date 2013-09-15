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
 * \file /Ely/include/PhysicsComponents/Vehicle.h
 *
 * \date 15/set/2013 (10:40:10)
 * \author consultit
 */

#ifndef VEHICLE_H_
#define VEHICLE_H_

#include <bulletVehicle.h>
#include "ObjectModel/Component.h"

namespace ely
{
class VehicleTemplate;

/**
 * \brief Component representing a character controller attached to an object.
 *
 * The control is accomplished through physics.\n
 * It constructs a character controller with the single specified collision
 * shape_type along with relevant parameters.\n
 * The up axis is the Z axis.\n
 *
 * XML Param(s):
 * - "throw_events"				|single|"false"
 * - "step_height"  			|single|"1.0"
 * - "collide_mask"  			|single|"all_on"
 * - "shape_type"  				|single|"sphere"
 * - "shape_size"  				|single|"medium"  (min, medium, max)
 * - "use_shape_of"				|single|no default
 * - "shape_radius"  			|single|no default (sphere,cylinder,capsule,cone)
 * - "shape_height"  			|single|no default (cylinder,capsule,cone)
 * - "shape_up"  				|single|no default (cylinder,capsule,cone)
 * - "shape_half_x"  			|single|no default (box)
 * - "shape_half_y"  			|single|no default (box)
 * - "shape_half_z"  			|single|no default (box)
 * - "fall_speed"  				|single|"55.0"
 * - "gravity"  				|single|"29.4" (3G)
 * - "jump_speed"  				|single|"10.0"
 * - "max_slope"  				|single|"45.0" (degrees)
 * - "max_jump_height"  		|single|no default
 * - "forward"  				|single|"enabled"
 * - "backward"  				|single|"enabled"
 * - "up"  						|single|"enabled"
 * - "down"  					|single|"enabled"
 * - "roll_left"  				|single|"enabled"
 * - "roll_right"  				|single|"enabled"
 * - "strafe_left"  			|single|"enabled"
 * - "strafe_right"  			|single|"enabled"
 * - "jump"  					|single|"enabled"
 * - "linear_speed"  			|single|"10.0"
 * - "angular_speed"  			|single|"45.0"
 * - "is_local"  				|single|"true"
 */
class Vehicle: public Component
{
protected:
	friend class VehicleTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Vehicle();
	Vehicle(SMARTPTR(VehicleTemplate)tmpl);
	virtual ~Vehicle();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an control manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \brief Gets/sets the node path of this vehicle.
	 * @return The node path of this vehicle.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \name BulletVehicle reference getter & conversion function.
	 */
	///@{
	SMARTPTR(BulletVehicle) getBulletVehicle() const;
	operator SMARTPTR(BulletVehicle)() const;
	///@}

private:
	///The NodePath associated to this vehicle.
	NodePath mNodePath;
	///The underlying BulletVehicle (read-only after creation & before destruction).
	SMARTPTR(BulletVehicle) mVehicle;

	///Throwing events.
	bool mThrowEvents, mOnStartSent, mOnStopSent;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Vehicle", Component::get_class_type());
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

inline void Vehicle::reset()
{
	//
	mNodePath = NodePath();
	mVehicle.clear();

	mThrowEvents = mOnStart = mOnStop = false;
}

inline NodePath Vehicle::getNodePath() const
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	return mNodePath;
}

inline void Vehicle::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	mNodePath = nodePath;
}

inline SMARTPTR(BulletVehicle) Vehicle::getBulletVehicle() const
{
	return mVehicle;
}

inline Vehicle::operator SMARTPTR(BulletVehicle)() const
{
	return mVehicle;
}

} /* namespace ely */
#endif /* VEHICLE_H_ */
