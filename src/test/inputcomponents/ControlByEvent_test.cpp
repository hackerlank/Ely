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

#include "InputComponents/ControlByEvent.h"
#include <boost/test/unit_test.hpp>
#include "InputSuiteFixture.h"

#include "InputComponents/ControlByEventTemplate.h"
#include <pandaFramework.h>
#include <event.h>

struct ControlByEventTestCaseFixture
{
	ControlByEventTestCaseFixture() :
			mControl(NULL), mCompId("ControlByEvent_Test")
	{
		mPanda = new PandaFramework();
		mControlTmpl = new ControlByEventTemplate(mPanda);
		ControlByEventTemplate::init_type();
		ControlByEvent::init_type();
	}

	~ControlByEventTestCaseFixture()
	{
		if (mControl)
		{
			delete mControl;
		}
		delete mControlTmpl;
		mPanda->close_framework();
		delete mPanda;
	}
	PandaFramework* mPanda;
	ControlByEventTemplate* mControlTmpl;
	ControlByEvent* mControl;
	ComponentId mCompId;
};

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Input, InputSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(ControlByEventTemplateTEST, ControlByEventTestCaseFixture)
{
	mControl =
	DCAST(ControlByEvent, mControlTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mControl != NULL);
	BOOST_CHECK(mControl->componentType() == ComponentId("ControlByEvent"));
	BOOST_CHECK(mControl->familyType() == ComponentFamilyType("Input"));
}

BOOST_FIXTURE_TEST_CASE(ControlByEventTEST, ControlByEventTestCaseFixture)
{
	BOOST_REQUIRE(mControl != NULL);
	BOOST_CHECK(mPanda->get_event_handler().has_hook("w"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-w"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("w-up"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("a"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-a"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("a-up"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("s"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-s"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("s-up"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("d"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-d"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("d-up"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("q"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-q"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("q-up"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("e"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-e"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("e-up"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("r"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-r"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("r-up"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("f"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("shift-f"));
	BOOST_CHECK(mPanda->get_event_handler().has_hook("f-up"));
}

BOOST_FIXTURE_TEST_CASE(ControlByEventDestructionTEST, ControlByEventTestCaseFixture)
{
	BOOST_REQUIRE(mControl != NULL);
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
