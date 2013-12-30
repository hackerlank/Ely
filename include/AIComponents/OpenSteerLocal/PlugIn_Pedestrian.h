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
// An autonomous "pedestrian":
// follows paths, avoids collisions with obstacles and other pedestrians
//
// 10-29-01 cwr: created
//
//
// ----------------------------------------------------------------------------
/**
 * \file /Ely/include/AIComponents/OpenSteerLocal/PlugIn_Pedestrian.h
 *
 * \date 28/dic/2013 (11:48:52)
 * \author consultit
 */

#ifndef PLUGIN_PEDESTRIAN_H_
#define PLUGIN_PEDESTRIAN_H_

#include <iomanip>
#include <sstream>
#include "OpenSteer/PolylineSegmentedPathwaySingleRadius.h"
#include <OpenSteer/SimpleVehicle.h>
#include <OpenSteer/Proximity.h>
#include <OpenSteer/Color.h>
#include <OpenSteer/PlugIn.h>
#include "common.h"

namespace ely
{

using namespace OpenSteer;

// ----------------------------------------------------------------------------

typedef AbstractProximityDatabase<AbstractVehicle*> ProximityDatabase;
typedef AbstractTokenForProximityDatabase<AbstractVehicle*> ProximityToken;

// ----------------------------------------------------------------------------

// How many pedestrians to create when the plugin starts first?
extern int const gPedestrianStartCount;
// creates a path for the PlugIn
///PolylineSegmentedPathwaySingleRadius* getTestPath(void);
///extern PolylineSegmentedPathwaySingleRadius* gTestPath;
extern SphereObstacle gObstacle1;
extern SphereObstacle gObstacle2;
//extern ObstacleGroup gObstacles;
extern Vec3 gEndpoint0;
extern Vec3 gEndpoint1;
extern bool gUseDirectedPathFollowing;
// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
extern RectangleObstacle gObstacle3;
// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid

// this was added for debugging tool, but I might as well leave it in
extern bool gWanderSwitch;

// ----------------------------------------------------------------------------

template<typename Entity>
class Pedestrian: public VehicleAddOnMixin<OpenSteer::SimpleVehicle, Entity>
{
public:

	// type for a group of Pedestrians
	typedef std::vector<Pedestrian*> groupType;

	// constructor
///	Pedestrian(ProximityDatabase& pd)
	Pedestrian()
	{
		// allocate a token for this boid in the proximity database
		proximityToken = NULL;
///		newPD(pd);

		// reset Pedestrian state
		reset();
	}

	// destructor
	virtual ~Pedestrian()
	{
		// delete this boid's token in the proximity database
		delete proximityToken;
	}

	// reset all instance state
	void reset(void)
	{
		// reset the vehicle
		SimpleVehicle::reset();
		VehicleAddOnMixin<OpenSteer::SimpleVehicle, Entity>::reset();

		// max speed and max steering force (maneuverability)
///		setMaxSpeed(2.0);
///		setMaxForce(8.0);
		// initially stopped
///		setSpeed(0);
		// size of bounding sphere, for obstacle avoidance, etc.
///		setRadius(0.5); // width = 0.7, add 0.3 margin, take half

		// set the path for this Pedestrian to follow
///		path = getTestPath();

		// set initial position
		// (random point on path + random horizontal offset)
///		const float d = path->length() * frandom01();
///		const float r = path->radius();
///		const Vec3 randomOffset = randomVectorOnUnitRadiusXZDisk() * r;
///		setPosition(path->mapPathDistanceToPoint(d) + randomOffset);

		// randomize 2D heading
		this->randomizeHeadingOnXZPlane();

		// pick a random direction for path following (upstream or downstream)
		pathDirection = (frandom01() > 0.5) ? -1 : +1;

		// trail parameters: 3 seconds with 60 points along the trail
		this->setTrailParameters(3, 60);

///		// notify proximity database that our position has changed
///		proximityToken->updateForNewPosition(this->position());
	}

	// per frame simulation update
	void update(const float currentTime, const float elapsedTime)
	{
		// apply steering force to our momentum
		this->applySteeringForce(determineCombinedSteering(elapsedTime), elapsedTime);

		// reverse direction when we reach an endpoint
		if (gUseDirectedPathFollowing)
		{
			const Color darkRed(0.7f, 0, 0);
			float const pathRadius = path->radius();

			if (Vec3::distance(this->position(), gEndpoint0) < pathRadius)
			{
				pathDirection = +1;
				this->annotationXZCircle(pathRadius, gEndpoint0, darkRed, 20);
			}
			if (Vec3::distance(this->position(), gEndpoint1) < pathRadius)
			{
				pathDirection = -1;
				this->annotationXZCircle(pathRadius, gEndpoint1, darkRed, 20);
			}
		}

		// annotation
		this->annotationVelocityAcceleration(5, 0);
		this->recordTrailVertex(currentTime, this->position());

		// notify proximity database that our position has changed
		proximityToken->updateForNewPosition(this->position());

		//eventually update the entity
		this->entityUpdate(currentTime, elapsedTime);
	}

	// compute combined steering force: move forward, avoid obstacles
	// or neighbors if needed, otherwise follow the path and wander
	Vec3 determineCombinedSteering(const float elapsedTime)
	{
		// move forward
		Vec3 steeringForce = this->forward();

		// probability that a lower priority behavior will be given a
		// chance to "drive" even if a higher priority behavior might
		// otherwise be triggered.
		const float leakThrough = 0.1f;

		// determine if obstacle avoidance is required
		Vec3 obstacleAvoidance;
		if (leakThrough < frandom01())
		{
			const float oTime = 6; // minTimeToCollision = 6 seconds
			// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
			// just for testing
			//             obstacleAvoidance = steerToAvoidObstacles (oTime, gObstacles);
			//             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle1);
			//             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle3);
			obstacleAvoidance = this->steerToAvoidObstacles(oTime, *gObstacles);
			// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
		}

		// if obstacle avoidance is needed, do it
		if (obstacleAvoidance != Vec3::zero)
		{
			steeringForce += obstacleAvoidance;
		}
		else
		{
			// otherwise consider avoiding collisions with others
			Vec3 collisionAvoidance;
			const float caLeadTime = 3;

			// find all neighbors within maxRadius using proximity database
			// (radius is largest distance between vehicles traveling head-on
			// where a collision is possible within caLeadTime seconds.)
			const float maxRadius = caLeadTime * this->maxSpeed() * 2;
			neighbors->clear();
			proximityToken->findNeighbors(this->position(), maxRadius,
					*neighbors);

			if (leakThrough < frandom01())
				collisionAvoidance = this->steerToAvoidNeighbors(caLeadTime,
						*neighbors) * 10;

			// if collision avoidance is needed, do it
			if (collisionAvoidance != Vec3::zero)
			{
				steeringForce += collisionAvoidance;
			}
			else
			{
				// add in wander component (according to user switch)
				if (gWanderSwitch)
					steeringForce += this->steerForWander(elapsedTime);

				// do (interactively) selected type of path following
				const float pfLeadTime = 3;
				const Vec3 pathFollow = (
						gUseDirectedPathFollowing ?
								this->steerToFollowPath(pathDirection,
										pfLeadTime, *path) :
								this->steerToStayOnPath(pfLeadTime, *path));

				// add in to steeringForce
				steeringForce += pathFollow * 0.5;
			}
		}

		// return steering constrained to global XZ "ground" plane
		return steeringForce.setYtoZero();
	}

	// draw this pedestrian into scene
	void draw(void)
	{
		drawBasic2dCircularVehicle(*this, gGray50);
		this->drawTrail();
	}

	// called when steerToFollowPath decides steering is required
	void annotatePathFollowing(const Vec3& future, const Vec3& onPath,
			const Vec3& target, const float outside)
	{
		const Color yellow(1, 1, 0);
		const Color lightOrange(1.0f, 0.5f, 0.0f);
		const Color darkOrange(0.6f, 0.3f, 0.0f);
		const Color yellowOrange(1.0f, 0.75f, 0.0f);

		// draw line from our position to our predicted future position
		this->annotationLine(this->position(), future, yellow);

		// draw line from our position to our steering target on the path
		this->annotationLine(this->position(), target, yellowOrange);

		// draw a two-toned line between the future test point and its
		// projection onto the path, the change from dark to light color
		// indicates the boundary of the tube.
		const Vec3 boundaryOffset = (onPath - future).normalize() * outside;
		const Vec3 onPathBoundary = future + boundaryOffset;
		this->annotationLine(onPath, onPathBoundary, darkOrange);
		this->annotationLine(onPathBoundary, future, lightOrange);
	}

	// called when steerToAvoidCloseNeighbors decides steering is required
	// (parameter names commented out to prevent compiler warning from "-W")
	void annotateAvoidCloseNeighbor(const AbstractVehicle& other,
			const float /*additionalDistance*/)
	{
		// draw the word "Ouch!" above colliding vehicles
		const float headOn = this->forward().dot(other.forward()) < 0;
		const Color green(0.4f, 0.8f, 0.1f);
		const Color red(1, 0.1f, 0);
		const Color color = headOn ? red : green;
		const char* string = headOn ? "OUCH!" : "pardon me";
		const Vec3 location = this->position() + Vec3(0, 0.5f, 0);
		if (OpenSteer::annotationIsOn())
		{
//			draw2dTextAt3dLocation(*string, location, color,
//					drawGetWindowWidth(), drawGetWindowHeight());
			draw2dTextAt3dLocation(*string, location, color, 0.0, 0.0);
		}
	}

	// (parameter names commented out to prevent compiler warning from "-W")
	void annotateAvoidNeighbor(const AbstractVehicle& threat,
			const float /*steer*/, const Vec3& ourFuture,
			const Vec3& threatFuture)
	{
		const Color green(0.15f, 0.6f, 0.0f);

		this->annotationLine(this->position(), ourFuture, green);
		this->annotationLine(threat.position(), threatFuture, green);
		this->annotationLine(ourFuture, threatFuture, gRed);
		this->annotationXZCircle(this->radius(), ourFuture, green, 12);
		this->annotationXZCircle(this->radius(), threatFuture, green, 12);
	}

	// xxx perhaps this should be a call to a general purpose annotation for
	// xxx "local xxx axis aligned box in XZ plane" -- same code in in
	// xxx CaptureTheFlag.cpp
	void annotateAvoidObstacle(const float minDistanceToCollision)
	{
		const Vec3 boxSide = this->side() * this->radius();
		const Vec3 boxFront = this->forward() * minDistanceToCollision;
		const Vec3 FR = this->position() + boxFront - boxSide;
		const Vec3 FL = this->position() + boxFront + boxSide;
		const Vec3 BR = this->position() - boxSide;
		const Vec3 BL = this->position() + boxSide;
		const Color white(1, 1, 1);
		this->annotationLine(FR, FL, white);
		this->annotationLine(FL, BL, white);
		this->annotationLine(BL, BR, white);
		this->annotationLine(BR, FR, white);
	}

	// switch to new proximity database -- just for demo purposes
	void newPD(ProximityDatabase& pd)
	{
		// delete this boid's token in the old proximity database
		delete proximityToken;

		// allocate a token for this boid in the proximity database
		proximityToken = pd.allocateToken(this);
	}

	// a pointer to this boid's interface object for the proximity database
	ProximityToken* proximityToken;

	// allocate one and share amoung instances just to save memory usage
	// (change to per-instance allocation to be more MP-safe)
///	static AVGroup neighbors;
	AVGroup* neighbors;

	// path to be followed by this pedestrian
	// XXX Ideally this should be a generic Pathway, but we use the
	// XXX getTotalPathLength and radius methods (currently defined only
	// XXX on PolylinePathway) to set random initial positions.  Could
	// XXX there be a "random position inside path" method on Pathway?
	PolylineSegmentedPathwaySingleRadius* path;

	// direction for path following (upstream or downstream)
	int pathDirection;

	ObstacleGroup* gObstacles;
};

//template<typename Entity> AVGroup Pedestrian<Entity>::neighbors;

// ----------------------------------------------------------------------------
// create path for PlugIn
//
//
//        | gap |
//
//        f      b
//        |\    /\        -
//        | \  /  \       ^
//        |  \/    \      |
//        |  /\     \     |
//        | /  \     c   top
//        |/    \g  /     |
//        /        /      |
//       /|       /       V      z     y=0
//      / |______/        -      ^
//     /  e      d               |
//   a/                          |
//    |<---out-->|               o----> x
//

// ----------------------------------------------------------------------------
// OpenSteerDemo PlugIn

template<typename Entity>
class PedestrianPlugIn: public PlugIn
{
public:

	const char* name(void)
	{
		return "Pedestrians";
	}

	float selectionOrderSortKey(void)
	{
		return 0.02f;
	}

	// be more "nice" to avoid a compiler warning
	virtual ~PedestrianPlugIn()
	{
	}

	void open(void)
	{
		// make the database used to accelerate proximity queries
		cyclePD = -1;
		pd = NULL;
		nextPD();

///		// create the specified number of Pedestrians
///		population = 0;
///		for (int i = 0; i < gPedestrianStartCount; i++)
///			addPedestrianToCrowd();
	}

	void update(const float currentTime, const float elapsedTime)
	{
		// update each Pedestrian
		iterator iter;
		for (iter = crowd.begin(); iter < crowd.end(); ++iter)
		{
			(*iter)->update(currentTime, elapsedTime);
		}
	}

	void redraw(const float currentTime, const float elapsedTime)
	{
		// draw and annotate each Pedestrian
		iterator iter;
		for (iter = crowd.begin(); iter < crowd.end(); ++iter)
		{
			(*iter)->draw();
		}

		// draw the path they follow and obstacles they avoid
		gDrawer3d->setTwoSided(true);
		drawPathAndObstacles();
		gDrawer3d->setTwoSided(false);

		// textual annotation for selected Pedestrian
		if (selectedVehicle && OpenSteer::annotationIsOn())
		{
			const Color color(0.8f, 0.8f, 1.0f);
			const Vec3 textOffset(0, 0.25f, 0);
			const Vec3 textPosition = selectedVehicle->position() + textOffset;
			const char* spacer = "      ";
			std::ostringstream annote;
			annote << std::setprecision(2);
			annote << std::setiosflags(std::ios::fixed);
			annote << spacer << "1: speed: " << selectedVehicle->speed()
					<< std::endl;
			annote << std::setprecision(1);
			annote << spacer << "2: no third thing" << std::ends;
/////			draw2dTextAt3dLocation(annote, textPosition, color,
/////					drawGetWindowWidth(), drawGetWindowHeight());
			draw2dTextAt3dLocation(annote, textPosition, color, 0.0, 0.0);
		}

		// display status in the upper left corner of the window
		std::ostringstream status;
		status << "Crowd size: " << crowd.size();
		status << "\nPD type: ";
		switch (cyclePD)
		{
		case 0:
			status << "LQ bin lattice";
			break;
		case 1:
			status << "brute force";
			break;
		}
		status << "\n ";
		if (gUseDirectedPathFollowing)
			status << "Directed path following.";
		else
			status << "Stay on the path.";
		status << "\n Wander: ";
		if (gWanderSwitch)
			status << "yes";
		else
			status << "no";
		status << std::endl;
/////		const float h = drawGetWindowHeight();
/////		const Vec3 screenLocation(10, h - 50, 0);
		const Vec3 screenLocation(-1.0, 0.9, 0);
/////		draw2dTextAt2dLocation(status, screenLocation, gGray80,
/////			drawGetWindowWidth(), drawGetWindowHeight());
		draw2dTextAt2dLocation(status, screenLocation, gGray80, 0.0, 0.0);
	}

	void serialNumberAnnotationUtility(const AbstractVehicle& selected,
			const AbstractVehicle& nearMouse)
	{
		// display a Pedestrian's serial number as a text label near its
		// screen position when it is near the selected vehicle or mouse.
		if (&selected && &nearMouse && OpenSteer::annotationIsOn())
		{
			for (iterator i = crowd.begin(); i != crowd.end(); i++)
			{
				AbstractVehicle* vehicle = *i;
				const float nearDistance = 6;
				const Vec3& vp = vehicle->position();
				const Vec3& np = nearMouse.position();
				if ((Vec3::distance(vp, selected.position()) < nearDistance)
						|| (&nearMouse
								&& (Vec3::distance(vp, np) < nearDistance)))
				{
					std::ostringstream sn;
					sn << "#" << ((Pedestrian<Entity>*) vehicle)->serialNumber
							<< std::ends;
					const Color textColor(0.8f, 1, 0.8f);
					const Vec3 textOffset(0, 0.25f, 0);
					const Vec3 textPos = vehicle->position() + textOffset;
/////					draw2dTextAt3dLocation(sn, textPos, textColor,
/////							drawGetWindowWidth(), drawGetWindowHeight());
					draw2dTextAt3dLocation(sn, textPos, textColor, 0.0, 0.0);
				}
			}
		}
	}

	void drawPathAndObstacles(void)
	{
		typedef PolylineSegmentedPathwaySingleRadius::size_type size_type;

		// draw a line along each segment of path
///		const PolylineSegmentedPathwaySingleRadius& path = *getTestPath();
		const PolylineSegmentedPathwaySingleRadius& path =
				dynamic_cast<PolylineSegmentedPathwaySingleRadius&>(*m_pathway);
		for (size_type i = 1; i < path.pointCount(); ++i)
		{
			drawLine(path.point(i), path.point(i - 1), gRed);
		}

		// draw obstacles
		drawXZCircle(gObstacle1.radius, gObstacle1.center, gWhite, 40);
		drawXZCircle(gObstacle2.radius, gObstacle2.center, gWhite, 40);
		// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
		{
			float w = gObstacle3.width * 0.5f;
			Vec3 p = gObstacle3.position();
			Vec3 s = gObstacle3.side();
			drawLine(p + (s * w), p + (s * -w), gWhite);

			Vec3 v1 = gObstacle3.globalizePosition(Vec3(w, w, 0));
			Vec3 v2 = gObstacle3.globalizePosition(Vec3(-w, w, 0));
			Vec3 v3 = gObstacle3.globalizePosition(Vec3(-w, -w, 0));
			Vec3 v4 = gObstacle3.globalizePosition(Vec3(w, -w, 0));

			drawLine(v1, v2, gWhite);
			drawLine(v2, v3, gWhite);
			drawLine(v3, v4, gWhite);
			drawLine(v4, v1, gWhite);
		}
		// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
	}

	void close(void)
	{
///		// delete all Pedestrians
///		while (population > 0)
///			removePedestrianFromCrowd();
	}

	void reset(void)
	{
		// reset each Pedestrian
		iterator iter;
		for (iter = crowd.begin(); iter != crowd.end(); ++iter)
		{
			(*iter)->reset();
		}
	}

///	void handleFunctionKeys(int keyNumber)
///	{
///		switch (keyNumber)
///		{
///		case 1:
///			addPedestrianToCrowd();
///			break;
///		case 2:
///			removePedestrianFromCrowd();
///			break;
///		case 3:
///			nextPD();
///			break;
///		case 4:
///			gUseDirectedPathFollowing = !gUseDirectedPathFollowing;
///			break;
///		case 5:
///			gWanderSwitch = !gWanderSwitch;
///			break;
///		}
///	}

///	void printMiniHelpForFunctionKeys(void)
///	{
///		std::ostringstream message;
///		message << "Function keys handled by ";
///		message << '"' << name() << '"' << ':' << std::ends;
/////		OpenSteerDemo::printMessage(message);
/////		OpenSteerDemo::printMessage(message);
/////		OpenSteerDemo::printMessage("  F1     add a pedestrian to the crowd.");
/////		OpenSteerDemo::printMessage("  F2     remove a pedestrian from crowd.");
/////		OpenSteerDemo::printMessage("  F3     use next proximity database.");
/////		OpenSteerDemo::printMessage("  F4     toggle directed path follow.");
/////		OpenSteerDemo::printMessage("  F5     toggle wander component on/off.");
/////		OpenSteerDemo::printMessage("");
///	}

	virtual void addVehicle(AbstractVehicle* vehicle)
	{
		PlugInAddOnMixin<OpenSteer::PlugIn>::addVehicle(vehicle);
		// allocate a token for this pedestrian in the proximity database
		Pedestrian<Entity>* pedestrian =
				dynamic_cast<Pedestrian<Entity>*>(vehicle);
		if (pedestrian)
		{
			pedestrian->newPD(*pd);
			// notify proximity database that our position has changed
			pedestrian->proximityToken->updateForNewPosition(
					pedestrian->position());
			//set path
			pedestrian->path =
					dynamic_cast<PolylineSegmentedPathwaySingleRadius*>(m_pathway);
			//set obstacles
			pedestrian->gObstacles = &m_obstacles;
			//set neighbors
			pedestrian->neighbors = &neighbors;
		}
	}

///	void addPedestrianToCrowd(void)
///	{
///		population++;
///		Pedestrian<Entity>* pedestrian = new Pedestrian<Entity>(*pd);
///		crowd.push_back(pedestrian);
///		if (population == 1)
///		{
///			selectedVehicle = pedestrian;
///		}
///	}

///	void removePedestrianFromCrowd(void)
///	{
///		if (population > 0)
///		{
///			// save pointer to last pedestrian, then remove it from the crowd
///			const Pedestrian<Entity>* pedestrian = crowd.back();
///			crowd.pop_back();
///			population--;
///			// if it is OpenSteerDemo's selected vehicle, unselect it
///			if (pedestrian == selectedVehicle)
///			{
///				selectedVehicle = NULL;
///			}
///			// delete the Pedestrian
///			delete pedestrian;
///		}
///	}

	// for purposes of demonstration, allow cycling through various
	// types of proximity databases.  this routine is called when the
	// OpenSteerDemo user pushes a function key.
	void nextPD(void)
	{
		// save pointer to old PD
		ProximityDatabase* oldPD = pd;

		// allocate new PD
		const int totalPD = 2;
		switch (cyclePD = (cyclePD + 1) % totalPD)
		{
		case 0:
		{
			const Vec3 center;
			const float div = 20.0f;
			const Vec3 divisions(div, 1.0f, div);
			const float diameter = 80.0f; //XXX need better way to get this
			const Vec3 dimensions(diameter, diameter, diameter);
			typedef LQProximityDatabase<AbstractVehicle*> LQPDAV;
			pd = new LQPDAV(center, dimensions, divisions);
			break;
		}
		case 1:
		{
			pd = new BruteForceProximityDatabase<AbstractVehicle*>();
			break;
		}
		}

		// switch each boid to new PD
		for (iterator i = crowd.begin(); i != crowd.end(); i++)
			(**i).newPD(*pd);

		// delete old PD (if any)
		delete oldPD;
	}

	const AVGroup& allVehicles(void)
	{
		return (const AVGroup&) crowd;
	}

	// crowd: a group (STL vector) of all Pedestrians
	typename Pedestrian<Entity>::groupType crowd;
	typedef typename Pedestrian<Entity>::groupType::const_iterator iterator;

	AVGroup neighbors;

	// pointer to database used to accelerate proximity queries
	ProximityDatabase* pd;

///	// keep track of current flock size
///	int population;

	// which of the various proximity databases is currently in use
	int cyclePD;

};

//PedestrianPlugIn gPedestrianPlugIn;

// ----------------------------------------------------------------------------

}// anonymous namespace

#endif /* PLUGIN_PEDESTRIAN_H_ */
