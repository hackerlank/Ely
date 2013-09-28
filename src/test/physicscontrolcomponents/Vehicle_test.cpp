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
 * \file /Ely/src/test/physicscontrolcomponents/Vehicle_test.cpp
 *
 * \date 15/set/2013 (10:40:11)
 * \author consultit
 */

#include "PhysicsControlSuiteFixture.h"

struct VehicleControllerTestCaseFixture
{
	VehicleControllerTestCaseFixture()
	{
	}

	~VehicleControllerTestCaseFixture()
	{
	}
};

/// PhysicsControl suite
BOOST_FIXTURE_TEST_SUITE(PhysicsControl, PhysicsControlSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(TEST)
{
	BOOST_TEST_MESSAGE("TESTING");
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // PhysicsControl suite
