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
 * \file /Ely/src/test/support/FirstPersonCamera_test.cpp
 *
 * \date 07/mag/2012 (18:15:16)
 * \author marco
 */

#include "SupportSuiteFixture.h"

struct FirstPersonCameraTestCaseFixture
{
	FirstPersonCameraTestCaseFixture()
	{
	}
	~FirstPersonCameraTestCaseFixture()
	{
	}
};

/// Support suite
BOOST_FIXTURE_TEST_SUITE(Support, SupportSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(FirstPersonCameraTEST)
{
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // Support suite


