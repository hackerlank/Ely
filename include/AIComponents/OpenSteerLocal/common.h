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
#include <OpenSteer/SimpleVehicle.h>
#include <OpenSteer/PlugIn.h>
#include <OpenSteer/PolylineSegmentedPathwaySegmentRadii.h>
#include <OpenSteer/PolylineSegmentedPathwaySingleRadius.h>
#include "DrawMeshDrawer.h"

extern ely::DrawMeshDrawer *gDrawer3d, *gDrawer2d;
extern ReMutex gOpenSteerDebugMutex;

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
	// forward-pointing unit basis vector
	OpenSteer::Vec3 m_forward;
	// side-pointing unit basis vector
	OpenSteer::Vec3 m_side;
	// upward-pointing unit basis vector
	OpenSteer::Vec3 m_up;
	// origin of local space
	OpenSteer::Vec3 m_position;
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
		//set vehicle settings effectively
		reset();
	}

	void reset()
	{
		Super::setMass(m_settings.m_mass);
		Super::setRadius(m_settings.m_radius);
		Super::setSpeed(m_settings.m_speed); // speed along Forward direction.
		Super::setMaxForce(m_settings.m_maxForce); // steering force is clipped to this magnitude
		Super::setMaxSpeed(m_settings.m_maxSpeed); // velocity is clipped to this magnitude
		Super::setForward(m_settings.m_forward);
		Super::setSide(Super::localRotateForwardToSide(m_settings.m_forward));
		Super::setUp(m_settings.m_up);
		Super::setPosition(m_settings.m_position);
		m_start = Super::position();
	}

	OpenSteer::Vec3 getStart()
	{
		return m_start;
	}

protected:
	///The entity updated by the vehicle.
	ENTITY m_entity;
	///The entity update method.
	ENTITYUPDATEMETHOD m_entityUpdateMethod;
	///The vehicle settings.
	VehicleSettings m_settings;
	///The vehicle start position.
	OpenSteer::Vec3 m_start;
};

//Obstacles: redefinition of draw
class SphereObstacle: public OpenSteer::SphereObstacle
{
public:
	SphereObstacle(float r, OpenSteer::Vec3 c) :
			OpenSteer::SphereObstacle(r, c)
	{
	}
	virtual void draw(const bool filled, const OpenSteer::Color& color,
			const OpenSteer::Vec3& viewpoint) const
	{
	}
};

class BoxObstacle: public OpenSteer::BoxObstacle
{
public:
	BoxObstacle(float w, float h, float d) :
			OpenSteer::BoxObstacle(w, h, d)
	{
	}
	virtual void draw(const bool filled, const OpenSteer::Color& color,
			const OpenSteer::Vec3& viewpoint) const
	{
	}
};

class PlaneObstacle: public OpenSteer::PlaneObstacle
{
public:
	PlaneObstacle(const OpenSteer::Vec3& s, const OpenSteer::Vec3& u,
			const OpenSteer::Vec3& f, const OpenSteer::Vec3& p) :
			OpenSteer::PlaneObstacle(s, u, f, p)
	{
	}
	virtual void draw(const bool filled, const OpenSteer::Color& color,
			const OpenSteer::Vec3& viewpoint) const
	{
	}
};

class RectangleObstacle: public OpenSteer::RectangleObstacle
{
public:
	RectangleObstacle(float w, float h, const OpenSteer::Vec3& s,
			const OpenSteer::Vec3& u, const OpenSteer::Vec3& f,
			const OpenSteer::Vec3& p, seenFromState sf) :
			OpenSteer::RectangleObstacle(w, h, s, u, f, p, sf)
	{
	}
	virtual void draw(const bool filled, const OpenSteer::Color& color,
			const OpenSteer::Vec3& viewpoint) const
	{
	}
};

template<typename Super>
class PlugInAddOnMixin: public Super
{
public:

	PlugInAddOnMixin() :
			selectedVehicle(NULL), m_pathway(NULL)
	{
		m_obstacles.clear();
	}

	virtual ~PlugInAddOnMixin()
	{
		delete m_pathway;
		OpenSteer::ObstacleIterator iter;
		for (iter = m_obstacles.begin(); iter != m_obstacles.end(); ++iter)
		{
			delete *iter;
		}
	}

	virtual void addVehicle(OpenSteer::AbstractVehicle* vehicle)
	{
		OpenSteer::AVGroup::iterator iter;
		//check if vehicle has not been already added
		for (iter = const_cast<OpenSteer::AVGroup&>(this->allVehicles()).begin();
				iter
						!= const_cast<OpenSteer::AVGroup&>(this->allVehicles()).end();
				++iter)
		{
			if (*iter == vehicle)
			{
				break;
			}
		}
		if (iter == this->allVehicles().end())
		{
			//vehicle needs to be added
			(const_cast<OpenSteer::AVGroup&>(this->allVehicles())).push_back(
					vehicle);
			//this is the new selected vehicle
			selectedVehicle = vehicle;
		}
	}

	virtual void removeVehicle(OpenSteer::AbstractVehicle* vehicle)
	{
		OpenSteer::AVGroup::iterator iter;
		//check if vehicle has been already removed or not
		for (iter = const_cast<OpenSteer::AVGroup&>(this->allVehicles()).begin();
				iter
						!= const_cast<OpenSteer::AVGroup&>(this->allVehicles()).end();
				++iter)
		{
			if (*iter == vehicle)
			{
				break;
			}
		}
		if (iter != this->allVehicles().end())
		{
			//vehicle needs to be removed
			(const_cast<OpenSteer::AVGroup&>(this->allVehicles())).erase(iter);
			//if this is the selected vehicle set to NULL
			if (selectedVehicle == vehicle)
			{
				selectedVehicle = NULL;
			}
		}
	}

	void setSelectedVehicle(OpenSteer::AbstractVehicle* _selectedVehicle)
	{
		selectedVehicle = _selectedVehicle;
	}

	OpenSteer::Pathway* getPathway()
	{
		return m_pathway;
	}

	void setPathway(OpenSteer::Vec3 const points[], bool singleRadius,
			float const radii[], bool closedCycle)
	{
		//delete old pathway
		delete m_pathway;
		m_pathway = NULL;
		//create a new pathway
		OpenSteer::SegmentedPathway::size_type numOfPoints = sizeof(points)
				/ sizeof(OpenSteer::Vec3);
		if (numOfPoints < 2)
		{
			m_pathway = NULL;
			return;
		}
		//check if single radius
		if (singleRadius)
		{
			m_pathway = new OpenSteer::PolylineSegmentedPathwaySingleRadius(
					numOfPoints, points, radii[0], closedCycle);
		}
		else
		{
			m_pathway = new OpenSteer::PolylineSegmentedPathwaySegmentRadii(
					numOfPoints, points, radii, closedCycle);
		}
	}

	OpenSteer::ObstacleGroup getObstacles()
	{
		return m_obstacles;
	}

	OpenSteer::AbstractObstacle* addObstacle(const std::string& type,
			float width, float height, float depth, float radius,
			const OpenSteer::Vec3& side, const OpenSteer::Vec3& up,
			const OpenSteer::Vec3& forward, const OpenSteer::Vec3& position,
			OpenSteer::AbstractObstacle::seenFromState seenFromState)
	{
		OpenSteer::AbstractObstacle* obstacle = NULL;
		if (type == std::string("box"))
		{
			BoxObstacle* box = new BoxObstacle(width, height, depth);
			obstacle = box;
			box->setSide(side.normalize());
			box->setUp(up.normalize());
			box->setForward(forward.normalize());
			box->setPosition(position);
			obstacle->setSeenFrom(seenFromState);
		}
		if (type == std::string("plane"))
		{
			obstacle = new PlaneObstacle(side.normalize(), up.normalize(),
					forward.normalize(), position);
			obstacle->setSeenFrom(seenFromState);
		}
		if (type == std::string("rectangle"))
		{
			obstacle = new RectangleObstacle(width, height, side.normalize(),
					up.normalize(), forward.normalize(), position,
					seenFromState);
		}
		if (type == std::string("sphere"))
		{
			obstacle = new SphereObstacle(radius, position);
			obstacle->setSeenFrom(seenFromState);
		}
		//store obstacle
		if (obstacle)
		{
			m_obstacles.push_back(obstacle);
		}
		return obstacle;
	}

protected:
	///The selected vehicle (for debug draw only).
	OpenSteer::AbstractVehicle* selectedVehicle;
	///The pathway handled by this plugin.
	OpenSteer::Pathway* m_pathway;
	///The obstacles handled by this plugin.
	OpenSteer::ObstacleGroup m_obstacles;
};

//common typedef
typedef PlugInAddOnMixin<OpenSteer::PlugIn> PlugIn;

}

#endif /* OPENSTEERLOCALCOMMON_H_ */
