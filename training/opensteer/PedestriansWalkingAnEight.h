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
// Pedestrian following path tests with one pedestrian following a pathway of
// the shape of an eight.
//
// 06-20-05 bk: created
//
//
// ----------------------------------------------------------------------------

#include <iomanip>
#include <sstream>
#include "OpenSteer/PolylineSegmentedPathwaySingleRadius.h"
#include "OpenSteer/SimpleVehicle.h"
//#include "OpenSteer/OpenSteerDemo.h"
#include "OpenSteer/Proximity.h"
#include "OpenSteer/Color.h"
#include <OpenSteer/PlugIn.h>

#include "common.h"

#include "DrawMeshDrawer.h"
extern ely::DrawMeshDrawer *gDrawer3d, *gDrawerGrid3d, *gDrawer2d;

namespace ely
{

using namespace OpenSteer;

// ----------------------------------------------------------------------------

typedef AbstractProximityDatabase<AbstractVehicle*> ProximityDatabase;
typedef AbstractTokenForProximityDatabase<AbstractVehicle*> ProximityToken;

// ----------------------------------------------------------------------------

// How many pedestrians to create when the plugin starts first?
extern int const gPedestrianStartCountEight; // 100
// creates a path for the PlugIn
PolylineSegmentedPathwaySingleRadius* getTestPathEight(void);
extern PolylineSegmentedPathwaySingleRadius* gTestPathEight;
extern ObstacleGroup gObstaclesEight;
extern Vec3 gEndpoint0Eight;
extern Vec3 gEndpoint1Eight;
extern bool gUseDirectedPathFollowingEight;

// this was added for debugging tool, but I might as well leave it in
extern bool gWanderSwitchEight;

// ----------------------------------------------------------------------------

class _PedestrianWalkingAnEight: public SimpleVehicle
{
public:

	// type for a group of _PedestrianWalkingAnEights
	typedef std::vector<_PedestrianWalkingAnEight*> groupType;

	// constructor
	_PedestrianWalkingAnEight(ProximityDatabase& pd)
	{
		// allocate a token for this boid in the proximity database
		proximityToken = NULL;
		newPD(pd);

		// reset PedestrianWalkingAnEight state
		reset();
	}

	// destructor
	virtual ~_PedestrianWalkingAnEight()
	{
		// delete this boid's token in the proximity database
		delete proximityToken;
	}

	// reset all instance state
	void reset(void)
	{
		// reset the vehicle
		SimpleVehicle::reset();

		// max speed and max steering force (maneuverability)
		setMaxSpeed(2.0);
		setMaxForce(8.0);

		// initially stopped
		setSpeed(0);

		// size of bounding sphere, for obstacle avoidance, etc.
		setRadius(0.5); // width = 0.7, add 0.3 margin, take half

		// set the path for this PedestrianWalkingAnEight to follow
		path = getTestPathEight();

		// set initial position
		// (random point on path + random horizontal offset)
		const float d = path->length() * frandom01();
		const float r = path->radius();
		const Vec3 randomOffset = randomVectorOnUnitRadiusXZDisk() * r;
		setPosition(path->mapPathDistanceToPoint(d) + randomOffset);

		// randomize 2D heading
		randomizeHeadingOnXZPlane();

		// pick a random direction for path following (upstream or downstream)
		pathDirection = (frandom01() > 0.5) ? -1 : +1;

		// trail parameters: 3 seconds with 60 points along the trail
		setTrailParameters(3, 60);

		// notify proximity database that our position has changed
		proximityToken->updateForNewPosition(position());
	}

	// per frame simulation update
	void update(const float currentTime, const float elapsedTime)
	{
		// apply steering force to our momentum
		applySteeringForce(determineCombinedSteering(elapsedTime), elapsedTime);

		// reverse direction when we reach an endpoint
		if (gUseDirectedPathFollowingEight)
		{
			const Color darkRed(0.7f, 0, 0);
			float const pathRadius = path->radius();

			if (Vec3::distance(position(), gEndpoint0Eight) < pathRadius)
			{
				pathDirection = +1;
				annotationXZCircle(pathRadius, gEndpoint0Eight, darkRed, 20);
			}
			if (Vec3::distance(position(), gEndpoint1Eight) < pathRadius)
			{
				pathDirection = -1;
				annotationXZCircle(pathRadius, gEndpoint1Eight, darkRed, 20);
			}
		}

		// annotation
		annotationVelocityAcceleration(5, 0);
		recordTrailVertex(currentTime, position());

		// notify proximity database that our position has changed
		proximityToken->updateForNewPosition(position());
	}

	// compute combined steering force: move forward, avoid obstacles
	// or neighbors if needed, otherwise follow the path and wander
	Vec3 determineCombinedSteering(const float elapsedTime)
	{
		// move forward
		Vec3 steeringForce = forward();

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
								   //             obstacleAvoidance = steerToAvoidObstacles (oTime, gObstaclesEight);
								   //             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle1);
								   //             obstacleAvoidance = steerToAvoidObstacle (oTime, gObstacle3);
			obstacleAvoidance = steerToAvoidObstacles(oTime, gObstaclesEight);
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
			const float maxRadius = caLeadTime * maxSpeed() * 2;
			neighbors.clear();
			proximityToken->findNeighbors(position(), maxRadius, neighbors);

			if (leakThrough < frandom01())
				collisionAvoidance = steerToAvoidNeighbors(caLeadTime,
						neighbors) * 10;

			// if collision avoidance is needed, do it
			if (collisionAvoidance != Vec3::zero)
			{
				steeringForce += collisionAvoidance;
			}
			else
			{
				// add in wander component (according to user switch)
				if (gWanderSwitchEight)
					steeringForce += steerForWander(elapsedTime);

				// do (interactively) selected type of path following
				const float pfLeadTime = 3;
				const Vec3 pathFollow = (
						gUseDirectedPathFollowingEight ?
								steerToFollowPath(pathDirection, pfLeadTime,
										*path) :
								steerToStayOnPath(pfLeadTime, *path));

				// add in to steeringForce
				steeringForce += pathFollow * 0.5;
			}
		}

		// return steering constrained to global XZ "ground" plane
		return steeringForce.setYtoZero();
	}

	// draw this PedestrianWalkingAnEight into scene
	void draw(void)
	{
		drawBasic2dCircularVehicle(*this, gGray50);
		drawTrail();
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
		annotationLine(position(), future, yellow);

		// draw line from our position to our steering target on the path
		annotationLine(position(), target, yellowOrange);

		// draw a two-toned line between the future test point and its
		// projection onto the path, the change from dark to light color
		// indicates the boundary of the tube.
		const Vec3 boundaryOffset = (onPath - future).normalize() * outside;
		const Vec3 onPathBoundary = future + boundaryOffset;
		annotationLine(onPath, onPathBoundary, darkOrange);
		annotationLine(onPathBoundary, future, lightOrange);
	}

	// called when steerToAvoidCloseNeighbors decides steering is required
	// (parameter names commented out to prevent compiler warning from "-W")
	void annotateAvoidCloseNeighbor(const AbstractVehicle& other,
			const float /*additionalDistance*/)
	{
		// draw the word "Ouch!" above colliding vehicles
		const float headOn = forward().dot(other.forward()) < 0;
		const Color green(0.4f, 0.8f, 0.1f);
		const Color red(1, 0.1f, 0);
		const Color color = headOn ? red : green;
		const char* string = headOn ? "OUCH!" : "pardon me";
		const Vec3 location = position() + Vec3(0, 0.5f, 0);
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

		annotationLine(position(), ourFuture, green);
		annotationLine(threat.position(), threatFuture, green);
		annotationLine(ourFuture, threatFuture, gRed);
		annotationXZCircle(radius(), ourFuture, green, 12);
		annotationXZCircle(radius(), threatFuture, green, 12);
	}

	// xxx perhaps this should be a call to a general purpose annotation for
	// xxx "local xxx axis aligned box in XZ plane" -- same code in in
	// xxx CaptureTheFlag.cpp
	void annotateAvoidObstacle(const float minDistanceToCollision)
	{
		const Vec3 boxSide = side() * radius();
		const Vec3 boxFront = forward() * minDistanceToCollision;
		const Vec3 FR = position() + boxFront - boxSide;
		const Vec3 FL = position() + boxFront + boxSide;
		const Vec3 BR = position() - boxSide;
		const Vec3 BL = position() + boxSide;
		const Color white(1, 1, 1);
		annotationLine(FR, FL, white);
		annotationLine(FL, BL, white);
		annotationLine(BL, BR, white);
		annotationLine(BR, FR, white);
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
	static AVGroup neighbors;

	// path to be followed by this PedestrianWalkingAnEight
	// XXX Ideally this should be a generic Pathway, but we use the
	// XXX getTotalPathLength and radius methods (currently defined only
	// XXX on PolylinePathway) to set random initial positions.  Could
	// XXX there be a "random position inside path" method on Pathway?
	PolylineSegmentedPathwaySingleRadius* path;

	// direction for path following (upstream or downstream)
	int pathDirection;
};

//AVGroup _PedestrianWalkingAnEight::neighbors;

typedef ActorCP1Mixin<_PedestrianWalkingAnEight, ProximityDatabase> PedestrianWalkingAnEight;

// ----------------------------------------------------------------------------
// OpenSteerDemo PlugIn

class PedestriansWalkingAnEightPlugIn: public PlugIn
{
public:

	const char* name(void)
	{
		return "Pedestrians Walking an Eight";
	}

	float selectionOrderSortKey(void)
	{
		return 98.0f;
	}

	virtual ~PedestriansWalkingAnEightPlugIn()
	{
	}                              // be more "nice" to avoid a compiler warning

	void open(void)
	{
		// make the database used to accelerate proximity queries
		cyclePD = -1;
		pd = NULL;
		nextPD();

		// create the specified number of Pedestrians
		population = 0;
		for (int i = 0; i < gPedestrianStartCountEight; i++)
			addPedestrianToCrowd();

		// initialize camera and selectedVehicle
//		PedestrianWalkingAnEight& firstPedestrian = **crowd.begin();
//		OpenSteerDemo::init3dCamera(firstPedestrian);
//		OpenSteerDemo::camera.mode = Camera::cmFixedDistanceOffset;
//		OpenSteerDemo::camera.fixedTarget.set(15, 0, 30);
//		OpenSteerDemo::camera.fixedPosition.set(15, 70, -70);
	}

	void update(const float currentTime, const float elapsedTime)
	{
		// update each PedestrianWalkingAnEight
		for (iterator i = crowd.begin(); i != crowd.end(); i++)
		{
			(**i).update(currentTime, elapsedTime);
		}
	}

	void redraw(const float currentTime, const float elapsedTime)
	{
		// selected PedestrianWalkingAnEight (user can mouse click to select another)
//		AbstractVehicle& selected = *OpenSteerDemo::selectedVehicle;

		// PedestrianWalkingAnEight nearest mouse (to be highlighted)
//		AbstractVehicle& nearMouse = *OpenSteerDemo::vehicleNearestToMouse();

		// update camera
//		OpenSteerDemo::updateCamera(currentTime, elapsedTime, selected);

		// draw "ground plane"
//		if (OpenSteerDemo::selectedVehicle)
//			gridCenter = selected.position();
//		OpenSteerDemo::gridUtility(gridCenter);
		if (gToggleDrawGrid)
		{
			///very slow
			if (selectedVehicle)
			{
				gridCenter = selectedVehicle->position();
			}
			gridUtility(gridCenter, 600, 20);
		}

		// draw and annotate each PedestrianWalkingAnEight
		for (iterator i = crowd.begin(); i != crowd.end(); i++)
			(**i).draw();

		// draw the path they follow and obstacles they avoid
		gDrawer3d->setTwoSided(true);
		drawPathAndObstacles();
		gDrawer3d->setTwoSided(false);

		// highlight PedestrianWalkingAnEight nearest mouse
//		OpenSteerDemo::highlightVehicleUtility(nearMouse);

		// textual annotation (at the vehicle's screen position)
//		serialNumberAnnotationUtility(selected, nearMouse);

		// textual annotation for selected PedestrianWalkingAnEight
//		if (OpenSteerDemo::selectedVehicle && OpenSteer::annotationIsOn())
		if (selectedVehicle && enableAnnotation)
		{
			const Color color(0.8f, 0.8f, 1.0f);
			const Vec3 textOffset(0, 0.25f, 0);
			const Vec3 textPosition = selectedVehicle->position() + textOffset;
//			const Vec3 camPosition = OpenSteerDemo::camera.position();
//			const float camDistance = Vec3::distance(selected.position(),
//					camPosition);
			const char* spacer = "      ";
			std::ostringstream annote;
			annote << std::setprecision(2);
			annote << std::setiosflags(std::ios::fixed);
			annote << spacer << "1: speed: " << selectedVehicle->speed()
					<< std::endl;
			annote << std::setprecision(1);
//			annote << spacer << "2: cam dist: " << camDistance << std::endl;
			annote << spacer << "3: no third thing" << std::ends;
//			draw2dTextAt3dLocation(annote, textPosition, color,
//					drawGetWindowWidth(), drawGetWindowHeight());
			draw2dTextAt3dLocation(annote, textPosition, color, 0.0, 0.0);
		}

		// display status in the upper left corner of the window
		std::ostringstream status;
		status << "[F1/F2] Crowd size: " << population;
		status << "\n[F3] PD type: ";
		switch (cyclePD)
		{
		case 0:
			status << "LQ bin lattice";
			break;
		case 1:
			status << "brute force";
			break;
		}
		status << "\n[F4] ";
		if (gUseDirectedPathFollowingEight)
			status << "Directed path following.";
		else
			status << "Stay on the path.";
		status << "\n[F5] Wander: ";
		if (gWanderSwitchEight)
			status << "yes";
		else
			status << "no";
		status << std::endl;
//		const float h = drawGetWindowHeight();
//		const Vec3 screenLocation(10, h - 50, 0);
		const Vec3 screenLocation(-1.0, 0.9, 0);
//		draw2dTextAt2dLocation(status, screenLocation, gGray80,
//				drawGetWindowWidth(), drawGetWindowHeight());
		draw2dTextAt2dLocation(status, screenLocation, gGray80, 0.0, 0.0);
	}

	void serialNumberAnnotationUtility(const AbstractVehicle& selected,
			const AbstractVehicle& nearMouse)
	{
		// display a PedestrianWalkingAnEight's serial number as a text label near its
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
					sn << "#"
							<< ((PedestrianWalkingAnEight*) vehicle)->serialNumber
							<< std::ends;
					const Color textColor(0.8f, 1, 0.8f);
					const Vec3 textOffset(0, 0.25f, 0);
					const Vec3 textPos = vehicle->position() + textOffset;
//					draw2dTextAt3dLocation(sn, textPos, textColor,
//							drawGetWindowWidth(), drawGetWindowHeight());
					draw2dTextAt3dLocation(sn, textPos, textColor, 0.0, 0.0);
				}
			}
		}
	}

	void drawPathAndObstacles(void)
	{
		typedef PolylineSegmentedPathwaySingleRadius::size_type size_type;

		// draw a line along each segment of path
		const PolylineSegmentedPathwaySingleRadius& path = *getTestPathEight();
		for (size_type i = 1; i < path.pointCount(); ++i)
		{
			drawLine(path.point(i), path.point(i - 1), gRed);
		}

	}

	void close(void)
	{
		// delete all Pedestrians
		while (population > 0)
			removePedestrianFromCrowd();
	}

	void reset(void)
	{
		// reset each PedestrianWalkingAnEight
		for (iterator i = crowd.begin(); i != crowd.end(); i++)
			(**i).reset();

//		// reset camera position
//		OpenSteerDemo::position2dCamera(*OpenSteerDemo::selectedVehicle);

//		// make camera jump immediately to new position
//		OpenSteerDemo::camera.doNotSmoothNextMove();
	}

	void handleFunctionKeys(int keyNumber)
	{
		switch (keyNumber)
		{
		case 1:
			addPedestrianToCrowd();
			break;
		case 2:
			removePedestrianFromCrowd();
			break;
		case 3:
			nextPD();
			break;
		case 4:
			gUseDirectedPathFollowingEight = !gUseDirectedPathFollowingEight;
			break;
		case 5:
			gWanderSwitchEight = !gWanderSwitchEight;
			break;
		}
	}

	void printMiniHelpForFunctionKeys(void)
	{
		std::ostringstream message;
		message << "Function keys handled by ";
		message << '"' << name() << '"' << ':' << std::ends;
//		OpenSteerDemo::printMessage(message);
//		OpenSteerDemo::printMessage(message);
//		OpenSteerDemo::printMessage("  F1     add a PedestrianWalkingAnEight to the crowd.");
//		OpenSteerDemo::printMessage("  F2     remove a PedestrianWalkingAnEight from crowd.");
//		OpenSteerDemo::printMessage("  F3     use next proximity database.");
//		OpenSteerDemo::printMessage("  F4     toggle directed path follow.");
//		OpenSteerDemo::printMessage("  F5     toggle wander component on/off.");
//		OpenSteerDemo::printMessage("");
	}

	void addPedestrianToCrowd(void)
	{
		population++;
		PedestrianWalkingAnEight* pedestrian = new PedestrianWalkingAnEight(
				*pd);
		crowd.push_back(pedestrian);
		if (population == 1)
		{
//			OpenSteerDemo::selectedVehicle = pedestrian;
			selectedVehicle = pedestrian;
		}
	}

	void removePedestrianFromCrowd(void)
	{
		if (population > 0)
		{
			// save pointer to last pedestrian, then remove it from the crowd
			const PedestrianWalkingAnEight* pedestrian =
					dynamic_cast<PedestrianWalkingAnEight*>(crowd.back());
			crowd.pop_back();
			population--;

			// if it is OpenSteerDemo's selected vehicle, unselect it
//			if (pedestrian == OpenSteerDemo::selectedVehicle)
//				OpenSteerDemo::selectedVehicle = NULL;
			if (pedestrian == selectedVehicle)
			{
				selectedVehicle = NULL;
			}

			// delete the Pedestrian
			delete pedestrian;
		}
	}

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
	PedestrianWalkingAnEight::groupType crowd;
	typedef PedestrianWalkingAnEight::groupType::const_iterator iterator;

	Vec3 gridCenter;

	// pointer to database used to accelerate proximity queries
	ProximityDatabase* pd;

	// keep track of current flock size
	int population;

	// which of the various proximity databases is currently in use
	int cyclePD;
};

//PedestriansWalkingAnEightPlugIn gPedestriansWalkingAnEightPlugIn;

// ----------------------------------------------------------------------------

}// ely namespace
