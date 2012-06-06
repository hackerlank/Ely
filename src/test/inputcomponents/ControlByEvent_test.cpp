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
 * \file /Ely/src/test/inputcomponents/ControlByEvent_test.cpp
 *
 * \date 31/mag/2012 (16:23:02)
 * \author marco
 */

#include <boost/test/unit_test.hpp>
#include "InputSuiteFixture.h"

struct ControlByEventTestCaseFixture
{
	ControlByEventTestCaseFixture()
	{
	}

	~ControlByEventTestCaseFixture()
	{
	}
};

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Input,InputSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(ControlByEventTEST)
{
	mControl =
	DCAST(ControlByEvent, mControlTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mControl != NULL);
	BOOST_TEST_MESSAGE( "Checking ControlByEventTemplate" );
	BOOST_CHECK(mControl->componentType() == ComponentId("ControlByEvent"));
	BOOST_CHECK(mControl->familyType() == ComponentFamilyType("Input"));
	BOOST_TEST_MESSAGE( "Checking ControlByEventHandlers" );
	std::vector<std::string>::iterator iter;
	for (iter = mEvents.begin(); iter != mEvents.end(); ++iter)
	{
		BOOST_CHECK(mPanda->get_event_handler().has_hook(*iter));
	}
	delete mControl;
	BOOST_TEST_MESSAGE( "Checking ControlByEventDestruction" );
	for (iter = mEvents.begin(); iter != mEvents.end(); ++iter)
	{
		BOOST_CHECK(not mPanda->get_event_handler().has_hook(*iter));
	}
	mControl = NULL;
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
