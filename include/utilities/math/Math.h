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
 * \file /Ely/include/utilities/math/Math.h
 *
 * \date Nov 2, 2011
 * \author marco
 */

#ifndef MATH_H_
#define MATH_H_

#include "utilities/UtilitiesPrerequisites.h"

#include <limits>
#include <cmath>

namespace ely
{

/**
 * \brief ely::Math
 */
class Math
{
public:

	/// Square of 1/2
	static const Real SQRT_05;
	/// Used by OgeAngle.h
	static const Real DEGREE2RADIAN;
	static const Real RADIAN2DEGREE;

	/** Compare two reals within a tolerance
	 */
	static bool areEqual(Real a, Real b,
			Real tolerance = std::numeric_limits<Real>::epsilon());

	/**
	 * Fast Square Root (aka sqrt())
	 * See invSqrt for references
	 */
	static inline Real fastSqrt(Real x)
	{
		return ((Real) 1.0f) / invSqrt(x);
	}
	/**
	 * Fast Inverse Square Root (aka 1.0/sqrt(r))
	 * @link http://www.mceniry.net/papers/Fast%20Inverse%20Square%20Root.pdf
	 * @link http://www.ogre3d.org/phpBB2/viewtopic.php?t=37343
	 */
	static Real invSqrt(Real x);

	/// @see round(Real, unsigned int)
	static inline Real round(Real value)
	{
		return (value >= 0.0) ? (Real) std::floor(value + 0.5) :
				(Real) std::ceil(value - 0.5);
	}

	/**
	 * Round a real value to the specified number of digits, default 0(whole number)
	 * @note Use round(Real) if you are not interested in rounding digits
	 */
	static Real round(Real value, unsigned int digits);
};

} /* namespace ely */
#endif /* MATH_H_ */
