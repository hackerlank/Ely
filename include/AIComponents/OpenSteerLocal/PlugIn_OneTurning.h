// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2005, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ----------------------------------------------------------------------------
//
//
// One vehicle turning way: a (near) minimal OpenSteerDemo PlugIn
//
// 06-24-02 cwr: created 
//
//
// ----------------------------------------------------------------------------
/**
 * \file /Ely/include/AIComponents/OpenSteerLocal/PlugIn_OneTurning.h
 *
 * \date 01/dic/2013 (08:53:52)
 * \author consultit
 */

#ifndef PLUGIN_ONETURNING_H_
#define PLUGIN_ONETURNING_H_

#include <iomanip>
#include <sstream>
#include <OpenSteer/SimpleVehicle.h>
#include <OpenSteer/PlugIn.h>
#include "common.h"

namespace ely
{

using namespace OpenSteer;

// ----------------------------------------------------------------------------

template<typename Entity>
class OneTurning: public VehicleAddOnMixin<OpenSteer::SimpleVehicle, Entity>
{
public:

	// type for a one turning: an STL vector of _OneTurning pointers
	typedef typename std::vector<OneTurning<Entity>*> groupType;

	// constructor
	OneTurning()
	{
		reset();
	}

	// reset state
	void reset(void)
	{
		SimpleVehicle::reset(); // reset the vehicle
		VehicleAddOnMixin<OpenSteer::SimpleVehicle, Entity>::reset();
		this->clearTrailHistory();  // prevent long streaks due to teleportation
	}

	// per frame simulation update
	void update(const float currentTime, const float elapsedTime)
	{
		this->applySteeringForce(Vec3(-2, 0, -3), elapsedTime);

		///call the entity update
		this->entityUpdate(currentTime, elapsedTime);

		this->annotationVelocityAcceleration();
		this->recordTrailVertex(currentTime, this->position());
	}

	// draw this character/vehicle into the scene
	void draw(void)
	{
		drawBasic2dCircularVehicle(*this, gGray50);
		this->drawTrail();
	}
};

//OneTurning externally updated.
template<typename Entity>
class ExternalOneTurning: public OneTurning<Entity>
{
public:
	void update(const float currentTime, const float elapsedTime)
	{
		//call the entity update
		this->entityUpdate(currentTime, elapsedTime);
	}
};

// ----------------------------------------------------------------------------
// PlugIn for OpenSteerDemo

template<typename Entity>
class OneTurningPlugIn: public PlugIn
{
public:

	const char* name(void)
	{
		return "One Turning Away";
	}

	float selectionOrderSortKey(void)
	{
		return 0.06f;
	}

	// be more "nice" to avoid a compiler warning
	virtual ~OneTurningPlugIn()
	{
	}

	void open(void)
	{
	}

	void update(const float currentTime, const float elapsedTime)
	{
		// update simulation of test vehicle
		iterator iter;
		for (iter = theVehicle.begin(); iter != theVehicle.end(); ++iter)
		{
			(*iter)->update(currentTime, elapsedTime);
		}
	}

	void redraw(const float currentTime, const float elapsedTime)
	{
		// draw test vehicle
		iterator iter;
		for (iter = theVehicle.begin(); iter != theVehicle.end(); ++iter)
		{
			(*iter)->draw();

			// textual annotation (following the test vehicle's screen position)
			std::ostringstream annote;
			annote << std::setprecision(2) << std::setiosflags(std::ios::fixed);
			annote << "      speed: " << (*iter)->speed() << std::ends;

			draw2dTextAt3dLocation(annote, (*iter)->position(), gRed, 0.0, 0.0);
			draw2dTextAt3dLocation(*"start", (*iter)->getStart(), gGreen, 0.0,
					0.0);
		}
	}

	void close(void)
	{
		theVehicle.clear();
	}

	void reset(void)
	{
		// reset vehicle
		iterator iter;
		for (iter = theVehicle.begin(); iter != theVehicle.end(); ++iter)
		{
			(*iter)->reset();
		}
	}

	const AVGroup& allVehicles(void)
	{
		return (const AVGroup&) theVehicle;
	}

	typename OneTurning<Entity>::groupType theVehicle; // for allVehicles
	typedef typename OneTurning<Entity>::groupType::const_iterator iterator;
};

} // ely namespace

#endif /* PLUGIN_ONETURNING_H_ */
