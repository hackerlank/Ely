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
 * \file /Ely/src/test/ely/Ely_test.cpp
 *
 * \date 07/mag/2012 (17:37:09)
 * \author marco
 */

//#include "utilities/math/Math.h"
//#include <boost/test/unit_test.hpp>
//#include "../UtilitiesSuiteFixture.h"
//#include "MathSuiteFixture.h"
//
//const ely::Real MathSuiteFixture::tolerance = std::numeric_limits<ely::Real>::epsilon() * 100;
//
//#include <cmath>
//
//struct MathTestCaseFixture
//{
//	MathTestCaseFixture() :
//			mPI(Ogre::Math::PI), mPI_4(Ogre::Math::PI / 4.0)
//	{
//		// TODO
//	}
//
//	~MathTestCaseFixture()
//	{
//		// TODO
//	}
//
//	ely::Real mPI, mPI_4;
//};
//
//// Utilities suite
//BOOST_FIXTURE_TEST_SUITE(utilities, UtilitiesSuiteFixture)
//// Math suite
//BOOST_FIXTURE_TEST_SUITE(math, MathSuiteFixture)
//
//// Test cases
//BOOST_FIXTURE_TEST_CASE(mathfunctions, MathTestCaseFixture)
//{
//	BOOST_CHECK(ely::Math::areEqual(mPI, M_PI));
//	BOOST_CHECK_CLOSE(ely::Math::fastSqrt(M_PI), std::sqrt(M_PI), 0.05);
//	BOOST_CHECK_CLOSE(ely::Math::invSqrt(mPI), 1.0 / std::sqrt(M_PI), 0.05);
//	//
//	BOOST_CHECK_CLOSE(ely::Math::round(1.25), 1.0, MathSuiteFixture::tolerance);
//	BOOST_CHECK_CLOSE(ely::Math::round(-0.75), -1.0, MathSuiteFixture::tolerance);
//	BOOST_CHECK_CLOSE(ely::Math::round(1.2345678912, 4), 1.2345, 1e-2);
//	BOOST_CHECK_CLOSE(ely::Math::round(1.2345678912, 8), 1.23456789, 1e-6);
//}
//
//BOOST_AUTO_TEST_SUITE_END() // Math suite
//
//BOOST_AUTO_TEST_SUITE_END()// Utilities suite
