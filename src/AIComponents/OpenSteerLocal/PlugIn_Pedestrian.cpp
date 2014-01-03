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
 * \file /Ely/training/opensteer/Pedestrian.cpp
 *
 * \date 24/nov/2013 (09:44:39)
 * \author consultit
 */
/**
 * \file /Ely/src/AIComponents/OpenSteerLocal/PlugIn_Pedestrian.cpp
 *
 * \date 28/dic/2013 (11:48:53)
 * \author consultit
 */

#include "AIComponents/OpenSteerLocal/PlugIn_Pedestrian.h"

namespace ely
{

//AVGroup Pedestrian::neighbors;

// How many pedestrians to create when the plugin starts first?
int const gPedestrianStartCount = 20;
// creates a path for the PlugIn
PolylineSegmentedPathwaySingleRadius* gTestPath = NULL;
//SphereObstacle gObstacle1;
//SphereObstacle gObstacle2;
ObstacleGroup gObstacles;
Vec3 gEndpoint0;
Vec3 gEndpoint1;
bool gUseDirectedPathFollowing = true;
// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
//RectangleObstacle gObstacle3(7, 7);
// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid

// this was added for debugging tool, but I might as well leave it in
bool gWanderSwitch = true;

PolylineSegmentedPathwaySingleRadius* getTestPath(void)
{
//	if (gTestPath == NULL)
//	{
//		const float pathRadius = 2;
//
//		const PolylineSegmentedPathwaySingleRadius::size_type pathPointCount = 7;
//		const float size = 30;
//		const float top = 2 * size;
//		const float gap = 1.2f * size;
//		const float out = 2 * size;
//		const float h = 0.5;
//		const Vec3 pathPoints[pathPointCount] =
//		{ Vec3(h + gap - out, 0, h + top - out),  // 0 a
//		Vec3(h + gap, 0, h + top),      // 1 b
//		Vec3(h + gap + (top / 2), 0, h + top / 2),    // 2 c
//		Vec3(h + gap, 0, h),          // 3 d
//		Vec3(h, 0, h),          // 4 e
//		Vec3(h, 0, h + top),      // 5 f
//		Vec3(h + gap, 0, h + top / 2) };   // 6 g
//
//		gObstacle1.center = interpolate(0.2f, pathPoints[0], pathPoints[1]);
//		gObstacle2.center = interpolate(0.5f, pathPoints[2], pathPoints[3]);
//		gObstacle1.radius = 3;
//		gObstacle2.radius = 5;
//		gObstacles.push_back(&gObstacle1);
//		gObstacles.push_back(&gObstacle2);
//		// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
//
//		gObstacles.push_back(&gObstacle3);
//
//		//         // rotated to be perpendicular with path
//		//         gObstacle3.setForward (1, 0, 0);
//		//         gObstacle3.setSide (0, 0, 1);
//		//         gObstacle3.setPosition (20, 0, h);
//
//		//         // moved up to test off-center
//		//         gObstacle3.setForward (1, 0, 0);
//		//         gObstacle3.setSide (0, 0, 1);
//		//         gObstacle3.setPosition (20, 3, h);
//
//		//         // rotated 90 degrees around path to test other local axis
//		//         gObstacle3.setForward (1, 0, 0);
//		//         gObstacle3.setSide (0, -1, 0);
//		//         gObstacle3.setUp (0, 0, -1);
//		//         gObstacle3.setPosition (20, 0, h);
//
//		// tilted 45 degrees
//		gObstacle3.setForward(Vec3(1, 1, 0).normalize());
//		gObstacle3.setSide(0, 0, 1);
//		gObstacle3.setUp(Vec3(-1, 1, 0).normalize());
//		gObstacle3.setPosition(20, 0, h);
//
//		//         gObstacle3.setSeenFrom (Obstacle::outside);
//		//         gObstacle3.setSeenFrom (Obstacle::inside);
//		gObstacle3.setSeenFrom(Obstacle::both);
//
//		// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
//
//		gEndpoint0 = pathPoints[0];
//		gEndpoint1 = pathPoints[pathPointCount - 1];
//
//		gTestPath = new PolylineSegmentedPathwaySingleRadius(pathPointCount,
//				pathPoints, pathRadius, false);
//	}
	return gTestPath;
}

} // ely namespace
