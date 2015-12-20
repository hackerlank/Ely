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
 * \file /Ely/src/test/physicscontrolcomponents/CharacterController_test.cpp
 *
 * \date 2012-10-30 
 * \author consultit
 */

#include "PhysicsControlSuiteFixture.h"

struct CharacterControllerTestCaseFixture
{
	CharacterControllerTestCaseFixture()
	{
	}

	~CharacterControllerTestCaseFixture()
	{
	}
};

/// PhysicsControl suite
BOOST_FIXTURE_TEST_SUITE(PhysicsControl, PhysicsControlSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(CharacterControllerTEST)
{
	BOOST_TEST_MESSAGE("TESTING CharacterControllerTemplate");
	mCharTmpl = new CharacterControllerTemplate(mPanda,mWin);
	BOOST_REQUIRE(mCharTmpl != NULL);
	mCharTmpl->setParametersDefaults();
	BOOST_TEST_MESSAGE("TESTING CharacterController");
	mChar =
	DCAST(CharacterController, mRigidTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mChar != NULL);
	BOOST_CHECK(mChar->componentType() == ComponentId("CharacterController"));
	BOOST_CHECK(mChar->componentFamilyType() == ComponentFamilyType("PhysicsControl"));
}

BOOST_AUTO_TEST_SUITE_END() // PhysicsControl suite
