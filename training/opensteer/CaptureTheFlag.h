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
// Capture the Flag   (a portion of the traditional game)
//
// The "Capture the Flag" sample steering problem, proposed by Marcin
// Chady of the Working Group on Steering of the IGDA's AI Interface
// Standards Committee (http://www.igda.org/Committees/ai.htm) in this
// message (http://sourceforge.net/forum/message.php?msg_id=1642243):
//
//     "An agent is trying to reach a physical location while trying
//     to stay clear of a group of enemies who are actively seeking
//     him. The environment is littered with obstacles, so collision
//     avoidance is also necessary."
//
// Note that the enemies do not make use of their knowledge of the 
// seeker's goal by "guarding" it.  
//
// XXX hmm, rename them "attacker" and "defender"?
//
// 08-12-02 cwr: created 
//
//
// ----------------------------------------------------------------------------

#include <iomanip>
#include <string>
#include <sstream>
#include "OpenSteer/SimpleVehicle.h"
//#include "OpenSteer/OpenSteerDemo.h"
#include "OpenSteer/Color.h"
#include <OpenSteer/PlugIn.h>

#include "common.h"

namespace ely
{

using namespace OpenSteer;

// ----------------------------------------------------------------------------
// short names for STL vectors (iterators) of SphereObstacle pointers
// (obsolete? replace with ObstacleGroup/ObstacleIterator ?)

typedef std::vector<SphereObstacle*> SOG;  // SphereObstacle group
typedef SOG::const_iterator SOI;           // SphereObstacle iterator

// ----------------------------------------------------------------------------
// This PlugIn uses two vehicle types: CtfSeeker and CtfEnemy.  They have a
// common base class: CtfBase which is a specialization of SimpleVehicle.

class CtfBase: public SimpleVehicle
{
public:

	// type for a flock: an STL vector of CtfBase pointers
	typedef std::vector<CtfBase*> groupType;

	// constructor
	CtfBase()
	{
		reset();
	}

	// reset state
	void reset(void);

	// draw this character/vehicle into the scene
	void draw(void);

	// annotate when actively avoiding obstacles
	void annotateAvoidObstacle(const float minDistanceToCollision);

	void drawHomeBase(void);

	void randomizeStartingPositionAndHeading(void);
	enum seekerState
	{
		running, tagged, atGoal
	};

	// for draw method
	Color bodyColor;

	// xxx store steer sub-state for anotation
	bool avoiding;

	// dynamic obstacle registry
	static void initializeObstacles(void);
	static void addOneObstacle(void);
	static void removeOneObstacle(void);
	float minDistanceToObstacle(const Vec3 point);
	static int obstacleCount;
	static const int maxObstacleCount;
	static SOG allObstacles;
};

class _CtfSeeker: public CtfBase
{
public:

	// constructor
	_CtfSeeker()
	{
		reset();
	}

	// reset state
	void reset(void);

	// per frame simulation update
	void update(const float currentTime, const float elapsedTime);

	// is there a clear path to the goal?
	bool clearPathToGoal(void);

	Vec3 steeringForSeeker(void);
	void updateState(const float currentTime);
	void draw(void);
	Vec3 steerToEvadeAllDefenders(void);
	Vec3 XXXsteerToEvadeAllDefenders(void);
	void adjustObstacleAvoidanceLookAhead(const bool clearPath);
	void clearPathAnnotation(const float threshold,
			const float behindcThreshold, const Vec3& goalDirection);

	seekerState state;
	bool evading; // xxx store steer sub-state for anotation
	float lastRunningTime; // for auto-reset
};

class _CtfEnemy: public CtfBase
{
public:

	// constructor
	_CtfEnemy()
	{
		reset();
	}

	// reset state
	void reset(void);

	// per frame simulation update
	void update(const float currentTime, const float elapsedTime);
};

typedef ActorMixin<_CtfSeeker> CtfSeeker;
typedef ActorMixin<_CtfEnemy> CtfEnemy;

// ----------------------------------------------------------------------------
// globals
// (perhaps these should be member variables of a Vehicle or PlugIn class)

extern const Vec3 gHomeBaseCenter;
extern const float gHomeBaseRadius;

extern const float gMinStartRadius;
extern const float gMaxStartRadius;

extern const float gBrakingRate;

extern const Color evadeColor; // annotation
extern const Color seekColor; // annotation
extern const Color clearPathColor; // annotation

extern const float gAvoidancePredictTimeMin;
extern const float gAvoidancePredictTimeMax;
extern float gAvoidancePredictTime;

extern bool enableAttackSeek; // for testing (perhaps retain for UI control?)
extern bool enableAttackEvade; // for testing (perhaps retain for UI control?)

extern _CtfSeeker* gSeeker;

// count the number of times the simulation has reset (e.g. for overnight runs)
extern int resetCount;

// ----------------------------------------------------------------------------
// state for OpenSteerDemo PlugIn
//
// XXX consider moving this inside CtfPlugIn
// XXX consider using STL (any advantage? consistency?)

extern _CtfSeeker* ctfSeeker;
extern const int ctfEnemyCount;
extern _CtfEnemy* ctfEnemies[];

#define testOneObstacleOverlap(radius, center)               \
    {                                                            \
        float d = Vec3::distance (c, center);                    \
        float clearance = d - (r + (radius));                    \
        if (minClearance > clearance) minClearance = clearance;  \
    }

// ----------------------------------------------------------------------------
// PlugIn for OpenSteerDemo

class CtfPlugIn: public PlugIn
{
public:

	const char* name(void)
	{
		return "Capture the Flag";
	}

	float selectionOrderSortKey(void)
	{
		return 0.01f;
	}

	virtual ~CtfPlugIn()
	{
	} // be more "nice" to avoid a compiler warning

	void open(void)
	{
		// create the seeker ("hero"/"attacker")
		ctfSeeker = new CtfSeeker;
		all.push_back(ctfSeeker);

		// create the specified number of enemies,
		// storing pointers to them in an array.
		for (int i = 0; i < ctfEnemyCount; i++)
		{
			ctfEnemies[i] = new CtfEnemy;
			all.push_back(ctfEnemies[i]);
		}

		// initialize camera
//		OpenSteerDemo::init2dCamera(*ctfSeeker);
//		OpenSteerDemo::camera.mode = Camera::cmFixedDistanceOffset;
//		OpenSteerDemo::camera.fixedTarget.set(15, 0, 0);
//		OpenSteerDemo::camera.fixedPosition.set(80, 60, 0);

		CtfBase::initializeObstacles();
	}

	void update(const float currentTime, const float elapsedTime)
	{
		// update the seeker
		ctfSeeker->update(currentTime, elapsedTime);

		// update each enemy
		for (int i = 0; i < ctfEnemyCount; i++)
		{
			ctfEnemies[i]->update(currentTime, elapsedTime);
		}
	}

	void redraw(const float currentTime, const float elapsedTime)
	{
		// selected vehicle (user can mouse click to select another)
//		AbstractVehicle& selected = *OpenSteerDemo::selectedVehicle;

		// vehicle nearest mouse (to be highlighted)
//		AbstractVehicle& nearMouse = *OpenSteerDemo::vehicleNearestToMouse();

		// update camera
//		OpenSteerDemo::updateCamera(currentTime, elapsedTime, selected);

		// draw "ground plane" centered between base and selected vehicle
//		const Vec3 goalOffset = gHomeBaseCenter
//				- OpenSteerDemo::camera.position();
//		const Vec3 goalDirection = goalOffset.normalize();
//		const Vec3 cameraForward = OpenSteerDemo::camera.xxxls().forward();
//		const float goalDot = cameraForward.dot(goalDirection);
//		const float blend = remapIntervalClip(goalDot, 1, 0, 0.5, 0);
//		const Vec3 gridCenter = interpolate(blend, selected.position(),
//				gHomeBaseCenter);
//		OpenSteerDemo::gridUtility(gridCenter);
		Vec3 gridCenter;
		if (gToggleDrawGrid)
		{
			///very slow
			if (selectedVehicle)
			{
				gridCenter = selectedVehicle->position();
			}
			gridUtility(gridCenter, 600, 20);
		}


		// draw the seeker, obstacles and home base
		ctfSeeker->draw();
		drawObstacles();
		drawHomeBase();

		// draw each enemy
		for (int i = 0; i < ctfEnemyCount; i++)
			ctfEnemies[i]->draw();

		// highlight vehicle nearest mouse
//		OpenSteerDemo::highlightVehicleUtility(nearMouse);
	}

	void close(void)
	{
		// delete seeker
		delete (ctfSeeker);
		ctfSeeker = NULL;

		// delete each enemy
		for (int i = 0; i < ctfEnemyCount; i++)
		{
			delete (ctfEnemies[i]);
			ctfEnemies[i] = NULL;
		}

		// clear the group of all vehicles
		all.clear();
	}

	void reset(void)
	{
		// count resets
		resetCount++;

		// reset the seeker ("hero"/"attacker") and enemies
		ctfSeeker->reset();
		for (int i = 0; i < ctfEnemyCount; i++)
			ctfEnemies[i]->reset();

		// reset camera position
//		OpenSteerDemo::position2dCamera(*ctfSeeker);

		// make camera jump immediately to new position
//		OpenSteerDemo::camera.doNotSmoothNextMove();
	}

	void handleFunctionKeys(int keyNumber)
	{
		switch (keyNumber)
		{
		case 1:
			CtfBase::addOneObstacle();
			break;
		case 2:
			CtfBase::removeOneObstacle();
			break;
		}
	}

	void printMiniHelpForFunctionKeys(void)
	{
		std::ostringstream message;
		message << "Function keys handled by ";
		message << '"' << name() << '"' << ':' << std::ends;
//		OpenSteerDemo::printMessage(message);
//		OpenSteerDemo::printMessage("  F1     add one obstacle.");
//		OpenSteerDemo::printMessage("  F2     remove one obstacle.");
//		OpenSteerDemo::printMessage("");
	}

	const AVGroup& allVehicles(void)
	{
		return (const AVGroup&) all;
	}

	void drawHomeBase(void)
	{
		const Vec3 up(0, 0.01f, 0);
		const Color atColor(0.3f, 0.3f, 0.5f);
		const Color noColor = gGray50;
		const bool reached = ctfSeeker->state == CtfSeeker::atGoal;
		const Color baseColor = (reached ? atColor : noColor);
		drawXZDisk(gHomeBaseRadius, gHomeBaseCenter, baseColor, 40);
		drawXZDisk(gHomeBaseRadius / 15, gHomeBaseCenter + up, gBlack, 20);
	}

	void drawObstacles(void)
	{
		const Color color(0.8f, 0.6f, 0.4f);
		const SOG& allSO = CtfBase::allObstacles;
		for (SOI so = allSO.begin(); so != allSO.end(); so++)
		{
			drawXZCircle((**so).radius, (**so).center, color, 40);
		}
	}

	// a group (STL vector) of all vehicles in the PlugIn
//	std::vector<CtfBase*> all;
	CtfBase::groupType all;
	typedef CtfBase::groupType::const_iterator iterator;
};

//CtfPlugIn gCtfPlugIn;

// ----------------------------------------------------------------------------

}// ely namespace
