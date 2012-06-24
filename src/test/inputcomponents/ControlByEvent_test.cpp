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

//PandaFramework* mPanda;
//ControlByEventTemplate* mControlTmpl;
//ControlByEvent* mControl;
//ComponentId mCompId;
//std::vector<Event> mEvents;
//const float VAL = 1.0e+12;
//const float DEG = 89.999;

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Input,InputSuiteFixture)

BOOST_AUTO_TEST_CASE(startup)
{
//	mControl = NULL;
//	mCompId = "ControlByEvent_Test";
//	BOOST_TEST_MESSAGE( "startup" );
//	mPanda = new PandaFramework();
//	mControlTmpl = new ControlByEventTemplate(mPanda, mPanda->open_window());
//	ControlByEventTemplate::init_type();
//	ControlByEvent::init_type();
//	mEvents.push_back(Event("w"));
//	mEvents.push_back(Event("shift-w"));
//	mEvents.push_back(Event("w-up"));
//	mEvents.push_back(Event("a"));
//	mEvents.push_back(Event("shift-a"));
//	mEvents.push_back(Event("a-up"));
//	mEvents.push_back(Event("s"));
//	mEvents.push_back(Event("shift-s"));
//	mEvents.push_back(Event("s-up"));
//	mEvents.push_back(Event("d"));
//	mEvents.push_back(Event("shift-d"));
//	mEvents.push_back(Event("d-up"));
//	mEvents.push_back(Event("q"));
//	mEvents.push_back(Event("shift-q"));
//	mEvents.push_back(Event("q-up"));
//	mEvents.push_back(Event("e"));
//	mEvents.push_back(Event("shift-e"));
//	mEvents.push_back(Event("e-up"));
//	mEvents.push_back(Event("r"));
//	mEvents.push_back(Event("shift-r"));
//	mEvents.push_back(Event("r-up"));
//	mEvents.push_back(Event("f"));
//	mEvents.push_back(Event("shift-f"));
//	mEvents.push_back(Event("f-up"));
}
/// Test cases
BOOST_FIXTURE_TEST_CASE(ControlByEventInitializeTEST,ControlByEventTestCaseFixture)
{
//	mControl =
//	DCAST(ControlByEvent, mControlTmpl->makeComponent(mCompId));
//	BOOST_REQUIRE(mControl != NULL);
//	BOOST_CHECK(mControl->componentType() == ComponentId("ControlByEvent"));
//	BOOST_CHECK(mControl->familyType() == ComponentFamilyType("Input"));
//	std::vector<Event>::iterator iter;
//	for (iter = mEvents.begin(); iter != mEvents.end(); ++iter)
//	{
//		//check if has event-handler
//		BOOST_REQUIRE(mPanda->get_event_handler().has_hook(iter->get_name()));
//	}
//	GeomNode* testGeom = new GeomNode("testGeom");
//	NodePath testNP(testGeom);
//	Object testObj("testObj");
//	testObj.nodePath() = testNP;
//	testObj.nodePath().set_pos(VAL,VAL,VAL);
//	testObj.nodePath().set_hpr(DEG,DEG,DEG);
//	mControl->ownerObject() = &testObj;
//	mControl->onAddSetup();
//	GenericAsyncTask* task = DCAST(GenericAsyncTask,mPanda->get_task_mgr().find_task("ControlByEvent::update"));
//	BOOST_REQUIRE(task != NULL);
//	unsigned int i;
//	//send movement events: 3 at a time
//	for (i = 0; i < mEvents.size(); i+=3)
//	{
//		mPanda->get_event_handler().dispatch_event(&mEvents[i]); //e.g. w
//		mControl->update(task);
//		mPanda->get_event_handler().dispatch_event(&mEvents[i+2]);//e.g. w-up
//		mPanda->get_event_handler().dispatch_event(&mEvents[i+1]);//e.g. shift-w
//		mControl->update(task);
//		mPanda->get_event_handler().dispatch_event(&mEvents[i+2]);//e.g. w-up
//	}
//	//testObject should stay at (nearly) initial position/orientation
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_x(), VAL, 1.0);
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_y(), VAL, 1.0);
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_z(), VAL, 1.0);
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_h(), DEG, 1.0);
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_p(), DEG, 1.0);
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_r(), DEG, 1.0);
//	//send speed events
//	testObj.nodePath().set_pos(VAL,VAL,VAL);
//	Event speedFast("shift");
//	mPanda->get_event_handler().dispatch_event(&speedFast);
//	mPanda->get_event_handler().dispatch_event(&mEvents[1]);//shift-w
//	mControl->update(task);
//	mPanda->get_event_handler().dispatch_event(&mEvents[2]);//w-up
//	mPanda->get_event_handler().dispatch_event(&mEvents[7]);//shift-s
//	mControl->update(task);
//	mPanda->get_event_handler().dispatch_event(&mEvents[2]);//w-up
//	//testObject should stay at (nearly) initial position/orientation
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_x(), VAL, 1.0);
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_y(), VAL, 1.0);
//	BOOST_CHECK_CLOSE( testObj.nodePath().get_z(), VAL, 1.0);
}

BOOST_AUTO_TEST_CASE(cleanup)
{
//	BOOST_TEST_MESSAGE( "cleanup" );
//	if (mControl)
//	{
//		delete mControl;
//	}
//	delete mControlTmpl;
//	mPanda->close_framework();
//	delete mPanda;
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
