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
 * \file /Ely/src/test/controlcomponents/Chaser_test.cpp
 *
 * \date 11/nov/2012 (09:45:01)
 * \author consultit
 */

#include "ControlSuiteFixture.h"

struct ChaserTestCaseFixture
{
	ChaserTestCaseFixture()
	{
	}
	~ChaserTestCaseFixture()
	{
	}
};

/// Control suite
BOOST_FIXTURE_TEST_SUITE(Control,ControlSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(ChaserInitializeTEST)
{
	BOOST_TEST_MESSAGE("TESTING ChaserTemplate");
	BOOST_TEST_MESSAGE("TESTING Chaser");
}

BOOST_AUTO_TEST_SUITE_END() // Control suite
