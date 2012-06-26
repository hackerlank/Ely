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
BOOST_AUTO_TEST_CASE(ControlByEventInitializeTEST)
{
	mObjectTmpl = new ObjectTemplate(ObjectType("Object_test"),ObjectTemplateManager::GetSingletonPtr(),mPanda,mWin);
	mControlTmpl = new ControlByEventTemplate(mPanda, mWin);
	mControl =
	DCAST(ControlByEvent, mControlTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mControl != NULL);
	BOOST_CHECK(mControl->componentType() == ComponentId("ControlByEvent"));
	BOOST_CHECK(mControl->familyType() == ComponentFamilyType("Input"));
	GeomNode* testGeom = new GeomNode("testGeom");
	NodePath testNP(testGeom);
	Object testObj("testObj",mObjectTmpl);
	testObj.nodePath() = testNP;
	testObj.nodePath().set_pos(VAL,VAL,VAL);
	testObj.nodePath().set_hpr(DEG,DEG,DEG);
	mControl->ownerObject() = &testObj;
	mControl->onAddToObjectSetup();
	std::vector<Event>::iterator iter;
	for (iter = mEvents.begin(); iter != mEvents.end(); ++iter)
	{
		//check if has event-handler
		BOOST_REQUIRE(mPanda->get_event_handler().has_hook(iter->get_name()));
	}
	GenericAsyncTask* task = DCAST(GenericAsyncTask,mPanda->get_task_mgr().find_task("ControlByEvent::update"));
	BOOST_REQUIRE(task != NULL);
	unsigned int i;
	//send movement events: 3 at a time
	for (i = 0; i < mEvents.size(); i+=3)
	{
		mPanda->get_event_handler().dispatch_event(&mEvents[i]); //e.g. w
		mControl->update(task);
		mPanda->get_event_handler().dispatch_event(&mEvents[i+2]);//e.g. w-up
		mPanda->get_event_handler().dispatch_event(&mEvents[i+1]);//e.g. shift-w
		mControl->update(task);
		mPanda->get_event_handler().dispatch_event(&mEvents[i+2]);//e.g. w-up
	}
	//testObject should stay at (nearly) initial position/orientation
	BOOST_CHECK_CLOSE( testObj.nodePath().get_x(), VAL, 1.0);
	BOOST_CHECK_CLOSE( testObj.nodePath().get_y(), VAL, 1.0);
	BOOST_CHECK_CLOSE( testObj.nodePath().get_z(), VAL, 1.0);
	BOOST_CHECK_CLOSE( testObj.nodePath().get_h(), DEG, 1.0);
	BOOST_CHECK_CLOSE( testObj.nodePath().get_p(), DEG, 1.0);
	BOOST_CHECK_CLOSE( testObj.nodePath().get_r(), DEG, 1.0);
	//send speed events
	testObj.nodePath().set_pos(VAL,VAL,VAL);
	Event speedFast("shift");
	mPanda->get_event_handler().dispatch_event(&speedFast);
	mPanda->get_event_handler().dispatch_event(&mEvents[1]);//shift-w
	mControl->update(task);
	mPanda->get_event_handler().dispatch_event(&mEvents[2]);//w-up
	mPanda->get_event_handler().dispatch_event(&mEvents[7]);//shift-s
	mControl->update(task);
	mPanda->get_event_handler().dispatch_event(&mEvents[2]);//w-up
	//testObject should stay at (nearly) initial position/orientation
	BOOST_CHECK_CLOSE( testObj.nodePath().get_x(), VAL, 1.0);
	BOOST_CHECK_CLOSE( testObj.nodePath().get_y(), VAL, 1.0);
	BOOST_CHECK_CLOSE( testObj.nodePath().get_z(), VAL, 1.0);
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
