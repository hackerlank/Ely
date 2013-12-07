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

namespace ely
{

class SteerVehicleTemplate;
class SimpleVehicle;

/**
 * \brief Component implementing OpenSteer Vehicles.
 *
 * \see http://opensteer.sourceforge.net
 *
 * XML Param(s):
 * - "type"						|single|"one_turning" (values: one_turning|)
 * - "mass"						|single|"1.0"
 * - "radius"					|single|no default
 * - "speed"					|single|"1.0"
 * - "maxForce"					|single|"1.0"
 * - "maxSpeed"					|single|"1.0"
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
