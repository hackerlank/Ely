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
// Draw
//
// This is a first stab at a graphics module for OpenSteerDemo.  It is intended
// to encapsulate all functionality related to 3d graphics as well as windows
// and graphics input devices such as the mouse.
//
// However this is purely an OpenGL-based implementation.  No special effort
// has been made to keep the "OpenGL way" from leaking through.  Attempting to
// port this to another graphics substrate may run into modularity problems.
//
// In any case, all calls to the underlying graphics substrate should be made
// from this module only.
//
// 10-04-04 bk:  put everything into the OpenSteer namespace
// 06-25-02 cwr: created 
//
//
// ----------------------------------------------------------------------------

#include "OpenSteer/Draw.h"

#include <iomanip>
#include <sstream>

#include "OpenSteer/Vec3.h"

// To include OpenSteer::round.
#include "OpenSteer/Utilities.h"

void OpenSteer::glVertexVec3(const Vec3& v)
{
}

// ----------------------------------------------------------------------------
// warn when draw functions are called during OpenSteerDemo's update phase

void OpenSteer::warnIfInUpdatePhase2(const char* name)
{
}

void OpenSteer::drawLine(const Vec3& startPoint, const Vec3& endPoint,
		const Color& color)
{
}

// ----------------------------------------------------------------------------
// draw a line with alpha blending

void OpenSteer::drawLineAlpha(const Vec3& startPoint, const Vec3& endPoint,
		const Color& color, const float alpha)
{
}

void OpenSteer::drawTriangle(const Vec3& a, const Vec3& b, const Vec3& c,
		const Color& color)
{
}

void OpenSteer::drawQuadrangle(const Vec3& a, const Vec3& b, const Vec3& c,
		const Vec3& d, const Color& color)
{
}

// ------------------------------------------------------------------------
// draws a "wide line segment": a rectangle of the given width and color
// whose mid-line connects two given endpoints

void OpenSteer::drawXZWideLine(const Vec3& startPoint, const Vec3& endPoint,
		const Color& color, float width)
{
}

// ------------------------------------------------------------------------
// General purpose circle/disk drawing routine.  Draws circles or disks (as
// specified by "filled" argument) and handles both special case 2d circles
// on the XZ plane or arbitrary circles in 3d space (as specified by "in3d"
// argument)

void OpenSteer::drawCircleOrDisk(const float radius, const Vec3& axis,
		const Vec3& center, const Color& color, const int segments,
		const bool filled, const bool in3d)
{
}

// ------------------------------------------------------------------------

void OpenSteer::draw3dCircleOrDisk(const float radius, const Vec3& center,
		const Vec3& axis, const Color& color, const int segments,
		const bool filled)
{
}

// ------------------------------------------------------------------------
// drawing utility used by both drawXZCircle and drawXZDisk

void OpenSteer::drawXZCircleOrDisk(const float radius, const Vec3& center,
		const Color& color, const int segments, const bool filled)
{
}

// ------------------------------------------------------------------------
// draw a circular arc on the XZ plane, from a start point, around a center,
// for a given arc length, in a given number of segments and color.  The
// sign of arcLength determines the direction in which the arc is drawn.
//
// XXX maybe this should alow allow "in3d" cricles about an given axis
// XXX maybe there should be a "filled" version of this
// XXX maybe this should be merged in with drawCircleOrDisk

void OpenSteer::drawXZArc(const Vec3& start, const Vec3& center,
		const float arcLength, const int segments, const Color& color)
{
}

// ------------------------------------------------------------------------
// a simple 2d vehicle on the XZ plane

void OpenSteer::drawBasic2dCircularVehicle(const AbstractVehicle& vehicle,
		const Color& color)
{
}

// ------------------------------------------------------------------------
// a simple 3d vehicle

void OpenSteer::drawBasic3dSphericalVehicle(const AbstractVehicle& vehicle,
		const Color& color)
{
}

// drawBasic3dSphericalVehicle with a supplied draw routine
// provided so non-OpenGL based apps can draw a boid

void OpenSteer::drawBasic3dSphericalVehicle(drawTriangleRoutine draw,
		const AbstractVehicle& vehicle, const Color& color)
{
}

// ------------------------------------------------------------------------
// draw a (filled-in, polygon-based) square checkerboard grid on the XZ
// (horizontal) plane.
//
// ("size" is the length of a side of the overall grid, "subsquares" is the
// number of subsquares along each edge (for example a standard checkboard
// has eight), "center" is the 3d position of the center of the grid,
// color1 and color2 are used for alternating subsquares.)

void OpenSteer::drawXZCheckerboardGrid(const float size, const int subsquares,
		const Vec3& center, const Color& color1, const Color& color2)
{
}

// ------------------------------------------------------------------------
// draw a square grid of lines on the XZ (horizontal) plane.
//
// ("size" is the length of a side of the overall grid, "subsquares" is the
// number of subsquares along each edge (for example a standard checkboard
// has eight), "center" is the 3d position of the center of the grid, lines
// are drawn in the specified "color".)

void OpenSteer::drawXZLineGrid(const float size, const int subsquares,
		const Vec3& center, const Color& color)
{
}

// ------------------------------------------------------------------------
// draw the three axes of a LocalSpace: three lines parallel to the
// basis vectors of the space, centered at its origin, of lengths
// given by the coordinates of "size".

void OpenSteer::drawAxes(const AbstractLocalSpace& ls, const Vec3& size,
		const Color& color)
{
}

// ------------------------------------------------------------------------
// draw the edges of a box with a given position, orientation, size
// and color.  The box edges are aligned with the axes of the given
// LocalSpace, and it is centered at the origin of that LocalSpace.
// "size" is the main diagonal of the box.
//
// use gGlobalSpace to draw a box aligned with global space

void OpenSteer::drawBoxOutline(const AbstractLocalSpace& localSpace,
		const Vec3& size, const Color& color)
{
}

// ------------------------------------------------------------------------
// Define scene's camera (viewing transformation) in terms of the camera's
// position, the point to look at (an "aim point" in the scene which will
// end up at the center of the camera's view), and an "up" vector defining
// the camera's "roll" around the "view axis" between cameraPosition and
// pointToLookAt (the image of the up vector will be vertical in the
// camera's view).

void OpenSteer::drawCameraLookAt(const Vec3& cameraPosition,
		const Vec3& pointToLookAt, const Vec3& up)
{
}

void OpenSteer::draw2dLine(const Vec3& startPoint, const Vec3& endPoint,
		const Color& color, float w, float h)
{
}

// ------------------------------------------------------------------------
// draw a reticle at the center of the window.  Currently it is small
// crosshair with a gap at the center, drawn in white with black borders

void OpenSteer::drawReticle(float w, float h)
{
}

// ------------------------------------------------------------------------
// check for errors during redraw, report any and then exit

void OpenSteer::checkForDrawError(const char * locationDescription)
{
}

// ----------------------------------------------------------------------------
// return a normalized direction vector pointing from the camera towards a
// given point on the screen: the ray that would be traced for that pixel

OpenSteer::Vec3 OpenSteer::directionFromCameraToScreenPosition(int x, int y,
		int h)
{
	return Vec3();
}

void OpenSteer::deferredDrawLine(const Vec3& startPoint, const Vec3& endPoint,
		const Color& color)
{
}

void OpenSteer::drawAllDeferredLines(void)
{
}

void OpenSteer::deferredDrawCircleOrDisk(const float radius, const Vec3& axis,
		const Vec3& center, const Color& color, const int segments,
		const bool filled, const bool in3d)
{
}

void OpenSteer::drawAllDeferredCirclesOrDisks(void)
{
}

void OpenSteer::draw2dTextAt3dLocation(const char& text, const Vec3& location,
		const Color& color, float w, float h)
{
}

void OpenSteer::draw2dTextAt3dLocation(const std::ostringstream& text,
		const Vec3& location, const Color& color, float w, float h)
{
}

void OpenSteer::draw2dTextAt2dLocation(const char& text, const Vec3 location,
		const Color& color, float w, float h)
{
}

void OpenSteer::draw2dTextAt2dLocation(const std::ostringstream& text,
		const Vec3 location, const Color& color, float w, float h)
{
}
