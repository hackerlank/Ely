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
 * \file /Ely/src/utilities/math/Math.cpp
 *
 * \date Nov 2, 2011
 * \author marco
 */

#include "utilities/math/Math.h"

namespace ely
{
const Real Math::SQRT_05 = static_cast<Real>(0.70710678118654752440084436210485);
const Real Math::DEGREE2RADIAN = Ogre::Math::PI / Real(180.0);
const Real Math::RADIAN2DEGREE = Real(180.0) / Ogre::Math::PI;

//-------------------------------------------------------------------------
bool Math::areEqual(Real a, Real b, Real tolerance)
{
	if (fabs(b - a) <= tolerance)
		return true;
	else
		return false;
}
//-------------------------------------------------------------------------
Real Math::invSqrt(Real x)
{
#ifdef ELY_DOUBLE
//	union
//	{	double f; unsigned long long ul;}y;
//	y.f = x;
//	y.ul = ( 0xBFCDD6A18F6A6F54 - y.ul ) >> 1;
//	y.f = ((double) 0.5f) * y.f * ( ((double) 3.0f) - x * y.f * y.f );
//	return y.f;

	// from "FAST INVERSE SQUARE ROOT" - CHRIS LOMONT
	double xhalf = 0.5 * x;
	long long int i = *(long long int*) &x; // get bits for floating value
	i = 0x5fe6ec85e7de30da - (i >> 1); // gives initial guess y0
	x = *(double*) &i; // convert bits back to double
	x = x * (1.5 - xhalf * x * x); // Newton step, repeating increases accuracy
	return x;

#else
//	union
//	{
//		float f;
//		unsigned long ul;
//	} y;
//	y.f = x;
//	y.ul = (0xBE6EB50C - y.ul) >> 1;
//	y.f = 0.5f * y.f * (3.0f - x * y.f * y.f);
//	return y.f;

	// from "FAST INVERSE SQUARE ROOT" - CHRIS LOMONT
	float xhalf = 0.5f * x;
	int i = *(int*) &x; // get bits for floating value
	i = 0x5f375a86 - (i >> 1); // gives initial guess y0
	x = *(float*) &i; // convert bits back to float
	x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
	return x;

#endif
}
//----------------------------------------------------------------------
Real Math::round(Real value, unsigned int digits)
{
	Real f = 1.0;

	while (digits--)
		f = f * (Real) 10.0;

	value *= f;

	if (value >= 0.0)
		value = (Real) std::floor(value + 0.5);
	else
		value = (Real) std::ceil(value - 0.5);

	value /= f;
	return value;
}
//----------------------------------------------------------------------

} /* namespace ely */
