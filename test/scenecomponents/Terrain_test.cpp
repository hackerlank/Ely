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
 * \file /Ely/src/test/scenecomponents/Terrain_test.cpp
 *
 * \date 2012-08-15 
 * \author consultit
 */

#include "SceneSuiteFixture.h"

struct TerrainTestCaseFixture
{
	TerrainTestCaseFixture()
	{
	}
	~TerrainTestCaseFixture()
	{
	}
};

/// Scene suite
BOOST_FIXTURE_TEST_SUITE(Scene, SceneSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(TEST)
{
	BOOST_TEST_MESSAGE("TESTING");
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // Scene suite
