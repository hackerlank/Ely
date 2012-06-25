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

#include "Ely.h"
#include <boost/test/unit_test.hpp>
#include "ElySuiteFixture.h"

struct ElyTestCaseFixture
{
	ElyTestCaseFixture()
	{
		// TODO
	}

	~ElyTestCaseFixture()
	{
		// TODO
	}
};

/// Ely suite
BOOST_FIXTURE_TEST_SUITE(Ely, ElySuiteFixture)

//startup common to all test cases
BOOST_AUTO_TEST_CASE(startupEly)
{
	BOOST_TEST_MESSAGE( "startup" );
}

/// Test cases
BOOST_FIXTURE_TEST_CASE(ElyTestCaseTEST, ElyTestCaseFixture)
{
	BOOST_CHECK(true);
}

//cleanup common to all test cases
BOOST_AUTO_TEST_CASE(cleanupEly)
{
	BOOST_TEST_MESSAGE( "cleanup" );
}

BOOST_AUTO_TEST_SUITE_END() // Ely suite
