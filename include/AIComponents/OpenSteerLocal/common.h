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

#ifndef COMMON_H_
#define COMMON_H_

#include <algorithm>
#include <nodePath.h>
#include <animControlCollection.h>
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
 * \brief SimpleVehicle settings.
 */
struct SimpleVehicleSettings
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

template<typename Super>
class SimpleVehicleMixin: public Super
{
public:

	void updateNodePath(const float currentTime, const float elapsedTime)
	{
		//update node path
		LPoint3f pos = OpenSteerVec3ToLVecBase3f(this->position());
		m_nodePath.set_pos(pos);
		m_nodePath.heads_up(pos - OpenSteerVec3ToLVecBase3f(this->forward()),
				OpenSteerVec3ToLVecBase3f(this->up()));
	}

	void setNodePath(NodePath nodePath)
	{
		m_nodePath = nodePath;
		// set size of bounding sphere
		LPoint3f minP, maxP;
		nodePath.calc_tight_bounds(minP, maxP);
		this->setRadius((maxP - minP).length() / 2.0);
	}

	SimpleVehicleSettings& getSettings()
	{
		return m_settings;
	}

	void setSettings(const SimpleVehicleSettings& settings)
	{
		m_settings = settings;
	}

protected:
	NodePath m_nodePath;
	SimpleVehicleSettings m_settings;
};

typedef SimpleVehicleMixin<OpenSteer::SimpleVehicle> SimpleVehicle;

}

#endif /* COMMON_H_ */
