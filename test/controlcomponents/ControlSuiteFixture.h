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
 * \file /Ely/src/test/controlcomponents/ControlSuiteFixture.h
 *
 * \date 2012-05-31 
 * \author consultit
 */

#ifndef INPUTSUITEFIXTURE_H_
#define INPUTSUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include "ControlComponents/Driver.h"
#include "ControlComponents/Chaser.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include <pandaFramework.h>
#include <nodePath.h>
#include <geomNode.h>
#include <genericAsyncTask.h>
#include <event.h>
#include <vector>

using namespace ely;

struct ControlSuiteFixture
{
	ControlSuiteFixture() :
			mControl(NULL), mCompId("Driver_Test"), mControlTmpl(NULL), mObjectTmpl(
					NULL), VAL(1.0e+12), DEG(89.999)
	{
		int argc = 0;
		char** argv = NULL;
		mPanda = new PandaFramework();
		mPanda->open_framework(argc, argv);
		mWin = mPanda->open_window();
		DriverTemplate::init_type();
		Driver::init_type();
		ObjectTemplate::init_type();
		Object::init_type();
		mEvents.push_back(Event("w"));
		mEvents.push_back(Event("shift-w"));
		mEvents.push_back(Event("w-up"));
		mEvents.push_back(Event("a"));
		mEvents.push_back(Event("shift-a"));
		mEvents.push_back(Event("a-up"));
		mEvents.push_back(Event("s"));
		mEvents.push_back(Event("shift-s"));
		mEvents.push_back(Event("s-up"));
		mEvents.push_back(Event("d"));
		mEvents.push_back(Event("shift-d"));
		mEvents.push_back(Event("d-up"));
		mEvents.push_back(Event("q"));
		mEvents.push_back(Event("shift-q"));
		mEvents.push_back(Event("q-up"));
		mEvents.push_back(Event("e"));
		mEvents.push_back(Event("shift-e"));
		mEvents.push_back(Event("e-up"));
		mEvents.push_back(Event("r"));
		mEvents.push_back(Event("shift-r"));
		mEvents.push_back(Event("r-up"));
		mEvents.push_back(Event("f"));
		mEvents.push_back(Event("shift-f"));
		mEvents.push_back(Event("f-up"));
	}
	~ControlSuiteFixture()
	{
		//delete always objects/components before their templates
		if (mObjectTmpl)
		{
			delete mObjectTmpl;
		}
		if (mControl)
		{
			delete mControl;
		}
		if (mControlTmpl)
		{
			delete mControlTmpl;
		}
		mPanda->close_framework();
		delete mPanda;
	}
	SMARTPTR(Driver) mControl;
	ComponentId mCompId;
	SMARTPTR(DriverTemplate) mControlTmpl;
	SMARTPTR(ObjectTemplate) mObjectTmpl;
	PandaFramework* mPanda;
	WindowFramework* mWin;
	std::vector<Event> mEvents;
	const float VAL;
	const float DEG;
	ObjectTemplateManager mObjectTmplMgr;
};

#endif /* INPUTSUITEFIXTURE_H_ */
