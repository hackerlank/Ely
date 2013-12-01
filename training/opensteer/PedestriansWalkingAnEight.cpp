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

#include "PedestriansWalkingAnEight.h"

namespace ely
{
// How many pedestrians to create when the plugin starts first?
int const gPedestrianStartCountEight = 1; // 100
// creates a path for the PlugIn
PolylineSegmentedPathwaySingleRadius* getTestPathEight(void);
PolylineSegmentedPathwaySingleRadius* gTestPathEight = NULL;
ObstacleGroup gObstaclesEight;
Vec3 gEndpoint0Eight;
Vec3 gEndpoint1Eight;
bool gUseDirectedPathFollowingEight = true;

// this was added for debugging tool, but I might as well leave it in
bool gWanderSwitchEight = true;

AVGroup PedestrianWalkingAnEight::neighbors;

/**
 * Creates a path of the form of an eight. Data provided by Nick Porcino.
 */
PolylineSegmentedPathwaySingleRadius* getTestPathEight(void)
{
	if (gTestPathEight == NULL)
	{
		const float pathRadius = 2;

		const PolylineSegmentedPathwaySingleRadius::size_type pathPointCount =
				16;
		// const float size = 30;
		const Vec3 pathPoints[pathPointCount] =
		{ Vec3(-12.678730011f, 0.0144290002063f, 0.523285984993f), Vec3(
				-10.447640419f, 0.0149269998074f, -3.44138407707f), Vec3(
				-5.88988399506f, 0.0128290001303f, -4.1717581749f), Vec3(
				0.941263973713f, 0.00330199999735f, 0.350513994694f), Vec3(
				5.83484792709f, -0.00407700007781f, 6.56243610382f), Vec3(
				11.0144147873f, -0.0111180003732f, 10.175157547f), Vec3(
				15.9621419907f, -0.0129949999973f, 8.82364273071f), Vec3(
				18.697883606f, -0.0102310003713f, 2.42084693909f), Vec3(
				16.0552558899f, -0.00506500015035f, -3.57153511047f), Vec3(
				10.5450153351f, 0.00284500000998f, -9.92683887482f), Vec3(
				5.88374519348f, 0.00683500012383f, -8.51393127441f), Vec3(
				3.17790007591f, 0.00419700006023f, -2.33129906654f), Vec3(
				1.94371795654f, 0.00101799995173f, 2.78656601906f), Vec3(
				-1.04967498779f, 0.000867999973707f, 5.57114219666f), Vec3(
				-7.58111476898f, 0.00634300010279f, 6.13661909103f), Vec3(
				-12.4111375809f, 0.0108559997752f, 3.5670940876f) };

		// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid

		gEndpoint0Eight = pathPoints[0];
		gEndpoint1Eight = pathPoints[pathPointCount - 1];

		gTestPathEight = new PolylineSegmentedPathwaySingleRadius(pathPointCount,
				pathPoints, pathRadius, false);
	}
	return gTestPathEight;
}

}// ely namespace
