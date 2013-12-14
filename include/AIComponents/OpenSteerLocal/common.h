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
 * \file /Ely/include/AIComponents/OpenSteerLocal/common.h
 *
 * \date 30/nov/2013 (11:11:37)
 * \author consultit
 */

#ifndef OPENSTEERLOCALCOMMON_H_
#define OPENSTEERLOCALCOMMON_H_

#include <algorithm>
#include <nodePath.h>
#include <OpenSteer/Vec3.h>
#include <OpenSteer/Color.h>
#include <OpenSteer/AbstractVehicle.h>
#include <OpenSteer/SimpleVehicle.h>

namespace ely
{

//LVecBase3f-OpenSteer::Vec3 conversion functions
inline OpenSteer::Vec3 LVecBase3fToOpenSteerVec3(const LVecBase3f& v)
{
	return OpenSteer::Vec3(v.get_x(), v.get_z(), -v.get_y());
}
inline LVecBase3f OpenSteerVec3ToLVecBase3f(const OpenSteer::Vec3& v)
{
	return LVecBase3f(v.x, -v.z, v.y);
}
//LVecBase4f-OpenSteer::Color conversion functions
inline OpenSteer::Color LVecBase4fToOpenSteerColor(const LVecBase4f& c)
{
	return OpenSteer::Color(c.get_x(), c.get_y(), c.get_z(), c.get_w());
}
inline LVecBase4f OpenSteerColorToLVecBase4f(const OpenSteer::Color& c)
{
	return LVecBase4f(c.r(), c.g(), c.b(), c.a());
}

/**
 * \brief Vehicle settings.
 */
struct VehicleSettings
{
	// mass
	float m_mass;
	// size of bounding sphere, for obstacle avoidance, etc.
	float m_radius;
	// speed of vehicle
	float m_speed;
	// the maximum steering force this vehicle can apply
	float m_maxForce;
	// the maximum speed this vehicle is allowed to move
	float m_maxSpeed;
};

template<typename Super, typename Entity>
class VehicleAddOnMixin: public Super
{
public:

	typedef Entity* ENTITY;
	typedef void (Entity::*ENTITYUPDATEMETHOD)(const float, const float);

	void setEntity(ENTITY entity)
	{
		m_entity = entity;
	}

	void setEntityUpdateMethod(ENTITYUPDATEMETHOD entityUpdateMethod)
	{
		m_entityUpdateMethod = entityUpdateMethod;
	}

	void entityUpdate(const float currentTime, const float elapsedTime)
	{
		(m_entity->*m_entityUpdateMethod)(currentTime, elapsedTime);
	}

	VehicleSettings& getSettings()
	{
		return m_settings;
	}

	void setSettings(const VehicleSettings& settings)
	{
		m_settings = settings;
	}

	void reset()
	{
		Super::setMass(m_settings.m_mass);
		Super::setRadius(m_settings.m_radius);
		Super::setSpeed(m_settings.m_speed); // speed along Forward direction.
		Super::setMaxForce(m_settings.m_maxForce); // steering force is clipped to this magnitude
		Super::setMaxSpeed(m_settings.m_maxSpeed); // velocity is clipped to this magnitude
	}

protected:
	ENTITY m_entity;
	ENTITYUPDATEMETHOD m_entityUpdateMethod;
	VehicleSettings m_settings;
};

}

#endif /* OPENSTEERLOCALCOMMON_H_ */
