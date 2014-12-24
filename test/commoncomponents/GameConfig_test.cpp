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
 * \file /Ely/src/test/commoncomponents/GameConfig_test.cpp
 *
 * \date 28/set/2014 (11:10:41)
 * \author consultit
 */

#include "CommonSuiteFixture.h"

struct GameConfigTestCaseFixture
{
	GameConfigTestCaseFixture()
	{
	}

	~GameConfigTestCaseFixture()
	{
	}
};

/// Common suite
BOOST_FIXTURE_TEST_SUITE(Common, CommonSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(TEST)
{
	BOOST_TEST_MESSAGE("TESTING");
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // Common suite


