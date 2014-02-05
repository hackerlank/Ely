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
// Multiple pursuit (for testing pursuit)
//
// 08-22-02 cwr: created 
//
//
// ----------------------------------------------------------------------------
/**
 * \file /Ely/include/AIComponents/OpenSteerLocal/PlugIn_MultiplePursuit.h
 *
 * \date 03/feb/2014 (19:10:52)
 * \author consultit
 */

#ifndef PLUGIN_MULTIPLEPURSUIT_H_
#define PLUGIN_MULTIPLEPURSUIT_H_

#include <OpenSteer/SimpleVehicle.h>
#include <OpenSteer/Color.h>
#include <OpenSteer/PlugIn.h>
#include "common.h"

namespace ely
{

using namespace OpenSteer;

// ----------------------------------------------------------------------------
// This PlugIn uses two vehicle types: MpWanderer and MpPursuer.  They have
// a common base class, MpBase, which is a specialization of SimpleVehicle.

template<typename Entity>
class MpBase: public VehicleAddOnMixin<OpenSteer::SimpleVehicle, Entity>
{
public:

	// type for a flock: an STL vector of MpBase pointers
	typedef std::vector<MpBase*> groupType;

	// constructor
	MpBase()
	{
		reset();
	}

	// reset state
	void reset(void)
	{
		SimpleVehicle::reset(); // reset the vehicle
		VehicleAddOnMixin<OpenSteer::SimpleVehicle, Entity>::reset();
///		setSpeed(0);            // speed along Forward direction.
///		setMaxForce(5.0);       // steering force is clipped to this magnitude
///		setMaxSpeed(3.0);       // velocity is clipped to this magnitude
		this->clearTrailHistory();    // prevent long streaks due to teleportation
		this->gaudyPursuitAnnotation = true; // select use of 9-color annotation
	}

	// draw into the scene
	void draw(void)
	{
		drawBasic2dCircularVehicle(*this, bodyColor);
		this->drawTrail();
	}

	// for draw method
	Color bodyColor;
};

template<typename Entity>
class MpWanderer: public MpBase<Entity>
{
public:

	// constructor
	MpWanderer()
	{
		reset();
	}

	// reset state
	void reset(void)
	{
		MpBase<Entity>::reset();
		bodyColor.set(0.4f, 0.6f, 0.4f); // greenish
	}

	// one simulation step
	void update(const float currentTime, const float elapsedTime)
	{
		const Vec3 wander2d = this->steerForWander(elapsedTime).setYtoZero();
		const Vec3 steer = this->forward() + (wander2d * 3);
		this->applySteeringForce(steer, elapsedTime);

		///call the entity update
		this->entityUpdate(currentTime, elapsedTime);

		// for annotation
		this->recordTrailVertex(currentTime, this->position());
	}

};

//MpWanderer externally updated.
template<typename Entity>
class ExternalMpWanderer: public MpWanderer<Entity>
{
public:
	void update(const float currentTime, const float elapsedTime)
	{
		//call the entity update
		this->entityUpdate(currentTime, elapsedTime);

		// for annotation
		this->recordTrailVertex(currentTime, this->position());
	}
};

template<typename Entity>
class MpPursuer: public MpBase<Entity>
{
public:

	// constructor
///	MpPursuer(MpWanderer* w)
	MpPursuer()
	{
		wanderer = NULL;
		reset();
	}

	// reset state
	void reset(void)
	{
		MpBase<Entity>::reset();
		bodyColor.set(0.6f, 0.4f, 0.4f); // redish
///		randomizeStartingPositionAndHeading();
	}

	// one simulation step
	void update(const float currentTime, const float elapsedTime)
	{
		// when pursuer touches quarry ("wanderer"), reset its position
		const float d = Vec3::distance(this->position(), wanderer->position());
		const float r = this->radius() + wanderer->radius();
		if (d < r)
			reset();

		const float maxTime = 20; // xxx hard-to-justify value
		this->applySteeringForce(this->steerForPursuit(*wanderer, maxTime), elapsedTime);

		///call the entity update
		this->entityUpdate(currentTime, elapsedTime);

		// for annotation
		this->recordTrailVertex(currentTime, position());
	}

	// randomize heading only
	void randomizeStartingPositionAndHeading(void)
	{
///		// randomize position on a ring between inner and outer radii
///		// centered around the home base
///		const float inner = 20;
///		const float outer = 30;
///		const float radius = frandom2(inner, outer);
///		const Vec3 randomOnRing = RandomUnitVectorOnXZPlane() * radius;
///		this->setPosition(wanderer->position() + randomOnRing);

		// randomize 2D heading
		this->randomizeHeadingOnXZPlane();
	}

	MpWanderer<Entity>* wanderer;
};

//MpPursuer externally updated.
template<typename Entity>
class ExternalMpPursuer: public MpPursuer<Entity>
{
public:
	void update(const float currentTime, const float elapsedTime)
	{
		//call the entity update
		this->entityUpdate(currentTime, elapsedTime);

		// for annotation
		this->recordTrailVertex(currentTime, position());
	}
};

// ----------------------------------------------------------------------------
// PlugIn for OpenSteerDemo

//used only initially by the MpPlugIn
struct DummyEntity
{
	void update(const float, const float){}
};

template<typename Entity>
class MpPlugIn: public PlugIn
{
public:

	const char* name(void)
	{
		return "Multiple Pursuit";
	}

	float selectionOrderSortKey(void)
	{
		return 0.04f;
	}

	virtual ~MpPlugIn()
	{
	} // be more "nice" to avoid a compiler warning

	void open(void)
	{
		/// XXX
		//create a dummy wanderer based on DummyEntity
		DummyEntity* entity = new DummyEntity;
		wanderer = new MpWanderer<DummyEntity>;
		wanderer->setEntity(entity);
		wanderer->setEntityUpdateMethod(&DummyEntity::update);
		/// XXX

///		// create the wanderer, saving a pointer to it
///		wanderer = new MpWanderer;
///		allMP.push_back(wanderer);
///		// create the specified number of pursuers, save pointers to them
///		const int pursuerCount = 30;
///		for (int i = 0; i < pursuerCount; i++)
///			allMP.push_back(new MpPursuer(wanderer));
///		pBegin = allMP.begin() + 1;  // iterator pointing to first pursuer
///		pEnd = allMP.end();          // iterator pointing to last pursuer

	}

	void update(const float currentTime, const float elapsedTime)
	{
		// update each vehicles
		iterator iter;
		for (iter = allMP.begin(); iter != allMP.end(); ++iter)
		{
			(*iter)->update(currentTime, elapsedTime);
		}
	}

	void redraw(const float currentTime, const float elapsedTime)
	{
		// draw each vehicles
		iterator iter;
		for (iter = allMP.begin(); iter != allMP.end(); ++iter)
		{
			(*iter)->draw();
		}
	}

	void close(void)
	{
		// delete wanderer, all pursuers, and clear list
///		delete (wanderer);
///		for (iterator i = pBegin; i != pEnd; i++)
///			delete ((MpPursuer*) *i);
///		allMP.clear();
	}

	void reset(void)
	{
		// reset each vehicles
		iterator iter;
		for (iter = allMP.begin(); iter != allMP.end(); ++iter)
		{
			(*iter)->reset();
		}
	}

	virtual void addVehicle(AbstractVehicle* vehicle)
	{
		PlugInAddOnMixin<OpenSteer::PlugIn>::addVehicle(vehicle);
		//check if this is a MpWanderer or MpPursuer
		MpWanderer<Entity>* wandererVehicle =
				dynamic_cast<MpWanderer<Entity>*>(vehicle);
		MpPursuer<Entity>* pursuerVehicle =
				dynamic_cast<MpPursuer<Entity>*>(vehicle);
		if (wandererVehicle)
		{
			//delete old wanderer
			delete wanderer;
			// set the plugin's wanderer
			wanderer = wandererVehicle;
			//update each pursuer's wanderer
			iterator iter;
			for (iter = allMP.begin(); iter != allMP.end(); ++iter)
			{
				MpPursuer<Entity>* pursuerVehicle =
						dynamic_cast<MpPursuer<Entity>*>(*iter);
				// update the pursuer's wanderer
				pursuerVehicle->wanderer = wanderer;
			}
		}
		if (pursuerVehicle)
		{
			//if not ExternalMpPursuer then randomize
			if (not dynamic_cast<ExternalMpPursuer<Entity>*>(pursuerVehicle))
			{
				// randomize only 2D heading
				pursuerVehicle->randomizeStartingPositionAndHeading();
			}
			// set the pursuer's wanderer
			pursuerVehicle->wanderer = wanderer;
		}
	}

	const AVGroup& allVehicles(void)
	{
		return (const AVGroup&) allMP;
	}

	// a group (STL vector) of all vehicles
	typename MpBase<Entity>::groupType allMP;
	typedef typename MpBase<Entity>::groupType::const_iterator iterator;

///	iterator pBegin, pEnd;

	MpWanderer<Entity>* wanderer;
};

//MpPlugIn gMpPlugIn;

// ----------------------------------------------------------------------------

}// ely namespace

#endif /* PLUGIN_MULTIPLEPURSUIT_H_ */
