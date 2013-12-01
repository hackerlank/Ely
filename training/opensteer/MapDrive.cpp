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
// Driving through map-based obstacles    (OpenSteerDemo PlugIn)
//
// This demonstration is inspired by the DARPA Grand Challenge cross country
// race for autonomous vehicles (http://www.darpa.mil/grandchallenge/).  A
// route is defined as a series of (GPS) waypoints and a width associated
// with each segment between waypoints.  This demo assumes sensors on-board
// the vehicle create a binary map classifying the surrounding terrain into
// drivable and not drivable.  The vehicle tries to follow the route while
// avoiding obstacles and maximizing speed.  When the vehicle finds itself
// in danger of collision, it "gives up" (turns yellow) and slows to a stop.
// If it collides with an obstacle it turns red.  In both cases the
// simulation is restarted.  (This plug-in includes two non-path-following
// demos of map-based obstacle avoidance.  Use F1 to select among them.)
//
// 06-01-05 bknafla: exchanged GCRoute with PolylineSegmentedPathwaySegmentRadii
// 08-16-04 cwr: merge back into OpenSteer code base
// 10-15-03 cwr: created 
//
//
// ----------------------------------------------------------------------------

#include "MapDrive.h"

namespace ely
{

/**
 * Maps @a point to @a pathway and extracts the radius at the mapping point.
 */
float mapPointToRadius(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point)
{
	PointToRadiusMapping mapping;
	OpenSteer::mapPointToPathAlike(pathway, point, mapping);
	return mapping.radius;
}

/**
 * Maps @a point to @a pathway and extracts the tangent at the mapping
 * point.
 */
OpenSteer::Vec3 mapPointToTangent(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point)
{
	PointToTangentMapping mapping;
	OpenSteer::mapPointToPathAlike(pathway, point, mapping);
	return mapping.tangent;
}

/**
 * Returns @c true if @a point is inside @a pathway segment @a segmentIndex.
 *
 * On point on the boundary isn't inside the pathway.
 */
bool isInsidePathSegment(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::PolylineSegmentedPathwaySegmentRadii::size_type segmentIndex,
		OpenSteer::Vec3 const& point)
{
	assert(pathway.isValid() && "pathway isn't valid.");
	assert(
			segmentIndex < pathway.segmentCount()
					&& "segmentIndex out of range.");

	float const segmentDistance = pathway.mapPointToSegmentDistance(
			segmentIndex, point);
	OpenSteer::Vec3 const pointOnSegmentCenterLine =
			pathway.mapSegmentDistanceToPoint(segmentIndex, segmentDistance);
	float const segmentRadiusAtPoint = pathway.mapSegmentDistanceToRadius(
			segmentIndex, segmentDistance);

	float const distancePointToPointOnSegmentCenterLine = (point
			- pointOnSegmentCenterLine).length();

	return distancePointToPointOnSegmentCenterLine < segmentRadiusAtPoint;
}

/**
 * Maps the @a point to @a pathway and extracts the tangent at the mapping
 * point or of the next path segment as indicated by @a direction if the
 * mapping point is near a path defining point (waypoint).
 *
 * @param pathway Pathway to inspect.
 * @param point Point to map to @a pathway.
 * @param direction Follow the path in path direction (@c 1) or in reverse
 *                  direction ( @c -1 ).
 */
OpenSteer::Vec3 mapPointAndDirectionToTangent(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point, int direction)
{
	assert(
			((1 == direction) || (-1 == direction))
					&& "direction must be 1 or -1.");
	typedef OpenSteer::PolylineSegmentedPathwaySegmentRadii::size_type size_type;

	PointToSegmentIndexMapping mapping;
	OpenSteer::mapPointToPathAlike(pathway, point, mapping);
	size_type segmentIndex = mapping.segmentIndex;
	size_type nextSegmentIndex = segmentIndex;
	if (0 < direction)
	{
		nextSegmentIndex = OpenSteer::nextSegment(pathway, segmentIndex);
	}
	else
	{
		nextSegmentIndex = OpenSteer::previousSegment(pathway, segmentIndex);
	}

	if (isInsidePathSegment(pathway, nextSegmentIndex, point))
	{
		segmentIndex = nextSegmentIndex;
	}

	// To save calculations to gather the tangent in a sound way the fact is
	// used that a polyline segmented pathway has the same tangent for a
	// whole segment.
	return pathway.mapSegmentDistanceToTangent(segmentIndex, 0.0f)
			* static_cast<float>(direction);

	/*
	 const int segmentIndex = indexOfNearestSegment (point);
	 const int nextIndex = segmentIndex + pathFollowDirection;
	 const bool insideNextSegment = isInsidePathSegment (point, nextIndex);
	 const int i = (segmentIndex +
	 (insideNextSegment ? pathFollowDirection : 0));
	 return normals [i] * (float)pathFollowDirection;
	 */
}

/**
 * Returns @c true if @a point is near a waypoint of @a pathway.
 *
 * It is near if its distance to a waypoint of the path is lesser than the
 * radius of one of the segments that the waypoint belongs to.
 *
 * On point on the boundary isn't inside the pathway.
 */
bool isNearWaypoint(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point)
{
	assert(pathway.isValid() && "pathway must be valid.");

	typedef OpenSteer::PolylineSegmentedPathwaySegmentRadii::size_type size_type;

	size_type pointIndex = 0;

	// Test first waypoint.
	OpenSteer::Vec3 pointPathwayPointVector = point - pathway.point(pointIndex);
	float pointPathwayPointDistance = pointPathwayPointVector.dot(
			pointPathwayPointVector);
	if (pointPathwayPointDistance
			< OpenSteer::square(pathway.segmentRadius(pointIndex)))
	{
		return true;
	}

	// Test other waypoints.
	size_type const maxInnerPointIndex = pathway.pointCount() - 2;
	for (pointIndex = 1; pointIndex <= maxInnerPointIndex; ++pointIndex)
	{
		pointPathwayPointVector = point - pathway.point(pointIndex);
		pointPathwayPointDistance = pointPathwayPointVector.dot(
				pointPathwayPointVector);
		if ((pointPathwayPointDistance
				< OpenSteer::square(pathway.segmentRadius(pointIndex)))
				|| (pointPathwayPointDistance
						< OpenSteer::square(
								pathway.segmentRadius(pointIndex - 1))))
		{
			return true;
		}
	}

	// Test last waypoint.
	pointPathwayPointVector = point - pathway.point(pointIndex);
	pointPathwayPointDistance = pointPathwayPointVector.dot(
			pointPathwayPointVector);
	if (pointPathwayPointDistance
			< OpenSteer::square(pathway.segmentRadius(pointIndex - 1)))
	{
		return true;
	}

	return false;

	/*
	 // loop over all waypoints
	 for (int i = 1; i < pointCount; i++)
	 {
	 // return true if near enough to this waypoint
	 const float r = maxXXX (radii[i], radii[i+1]);
	 const float d = (point - points[i]).length ();
	 if (d < r) return true;
	 }
	 return false;
	 */
}

/**
 * Maps @a point to @a pathway and returns the mapping point on the pathway
 * boundary and how far outside @a point is from the mapping point.
 */
OpenSteer::Vec3 mapPointToPointOnCenterLineAndOutside(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point, float& outside)
{
	PointToPointOnCenterLineAndOutsideMapping mapping;
	OpenSteer::mapPointToPathAlike(pathway, point, mapping);
	outside = mapping.distancePointToPathBoundary;
	return mapping.pointOnPathCenterLine;
}

/**
 * Maps @a point to @a pathway and returns how far outside @a point is from
 * the mapping point on the path boundary.
 */
float mapPointToOutside(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point)
{
	PointToOutsideMapping mapping;
	OpenSteer::mapPointToPathAlike(pathway, point, mapping);
	return mapping.distancePointToPathBoundary;
}

/**
 * Returns @c true if @a point is inside @a pathway, @c false otherwise.
 * A point on the boundary isn't inside the pathway.
 */
bool isInsidePathway(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point)
{
	return 0.0f > mapPointToOutside(pathway, point);
}

OpenSteer::PolylineSegmentedPathwaySegmentRadii::size_type mapPointToSegmentIndex(
		OpenSteer::PolylineSegmentedPathwaySegmentRadii const& pathway,
		OpenSteer::Vec3 const& point)
{
	PointToSegmentIndexMapping mapping;
	OpenSteer::mapPointToPathAlike(pathway, point, mapping);
	return mapping.segmentIndex;
}

// define map size (and compute its half diagonal)
float MapDriver::worldSize = 200;
float MapDriver::worldDiag = sqrtXXX(square(worldSize) / 2);

// 0 = obstacle avoidance and speed control
// 1 = wander, obstacle avoidance and speed control
// 2 = path following, obstacle avoidance and speed control
// int MapDriver::demoSelect = 0;
int MapDriver::demoSelect = 2;

float MapDriver::savedNearestWR = 0;
float MapDriver::savedNearestR = 0;
float MapDriver::savedNearestL = 0;
float MapDriver::savedNearestWL = 0;

} // anonymous namespace
