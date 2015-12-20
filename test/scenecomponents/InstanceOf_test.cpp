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
 * \file /Ely/src/test/scenecomponents/InstanceOf_test.cpp
 *
 * \date 2012-05-20 
 * \author consultit
 */

#include "SceneSuiteFixture.h"

struct InstanceOfTestCaseFixture
{
	InstanceOfTestCaseFixture()
	{
	}
	~InstanceOfTestCaseFixture()
	{
	}
};

/// Scene suite
BOOST_FIXTURE_TEST_SUITE(Scene, SceneSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(InstanceOfTemplateTEST)
{
	mInstanceOf =
	DCAST(InstanceOf, mInstanceOfTmpl->makeComponent(mInstanceOfId));
	BOOST_REQUIRE(mInstanceOf != NULL);
	BOOST_CHECK(mInstanceOf->componentType() == ComponentId("InstanceOf"));
	BOOST_CHECK(mInstanceOf->componentFamilyType() == ComponentFamilyType("Graphics"));
}

BOOST_AUTO_TEST_SUITE_END() // Scene suite

