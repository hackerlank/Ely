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
 * \file /Ely/src/test/inputcomponents/InputSuiteFixture.h
 *
 * \date 31/mag/2012 (16:24:12)
 * \author marco
 */

#ifndef INPUTSUITEFIXTURE_H_
#define INPUTSUITEFIXTURE_H_

#include "InputComponents/ControlByEvent.h"
#include "InputComponents/ControlByEventTemplate.h"
#include <pandaFramework.h>
#include <vector>
#include <string>
#include <iostream>

struct InputSuiteFixture
{
	InputSuiteFixture() :
		mControl(NULL), mCompId("ControlByEvent_Test")
	{
		BOOST_TEST_MESSAGE( "setup InputSuiteFixture" );
		mPanda = new PandaFramework();
		mControlTmpl = new ControlByEventTemplate(mPanda);
		ControlByEventTemplate::init_type();
		ControlByEvent::init_type();
		mEvents.push_back("w");
		mEvents.push_back("shift-w");
		mEvents.push_back("w-up");
		mEvents.push_back("a");
		mEvents.push_back("shift-a");
		mEvents.push_back("a-up");
		mEvents.push_back("s");
		mEvents.push_back("shift-s");
		mEvents.push_back("s-up");
		mEvents.push_back("d");
		mEvents.push_back("shift-d");
		mEvents.push_back("d-up");
		mEvents.push_back("q");
		mEvents.push_back("shift-q");
		mEvents.push_back("q-up");
		mEvents.push_back("e");
		mEvents.push_back("shift-e");
		mEvents.push_back("e-up");
		mEvents.push_back("r");
		mEvents.push_back("shift-r");
		mEvents.push_back("r-up");
		mEvents.push_back("f");
		mEvents.push_back("shift-f");
		mEvents.push_back("f-up");
	}
	~InputSuiteFixture()
	{
		if (mControl)
		{
			delete mControl;
		}
		delete mControlTmpl;
		mPanda->close_framework();
		delete mPanda;
		BOOST_TEST_MESSAGE( "teardown InputSuiteFixture" );
	}
	PandaFramework* mPanda;
	ControlByEventTemplate* mControlTmpl;
	ControlByEvent* mControl;
	ComponentId mCompId;
	std::vector<std::string> mEvents;
};

#endif /* INPUTSUITEFIXTURE_H_ */
