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
 * \file /Ely/src/test/inputcomponents/ControlByEventTemplate_test.cpp
 *
 * \date 31/mag/2012 (16:25:58)
 * \author marco
 */

#include "InputComponents/ControlByEventTemplate.h"
#include <boost/test/unit_test.hpp>
#include "InputSuiteFixture.h"

#include "InputComponents/ControlByEvent.h"

struct ControlByEventTemplateTestCaseFixture
{
	ControlByEventTemplateTestCaseFixture() :
			mControlTmpl(new ControlByEventTemplate()), mControl(NULL), mCompId(
					"ControlByEvent")
	{
	}

	~ControlByEventTemplateTestCaseFixture()
	{
		delete mControlTmpl;
		if (mControl)
		{
			delete mControl;
		}
	}

	ControlByEventTemplate* mControlTmpl;
	ControlByEvent* mControl;
	ComponentId mCompId;
};

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Input, InputSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(ControlByEventTemplateMethods, ControlByEventTemplateTestCaseFixture)
{

	mControl =
	DCAST(ControlByEvent, mControlTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mControl != NULL);
	BOOST_CHECK(mControl->componentType() == ComponentId("ControlByEvent"));
	BOOST_CHECK(mControl->familyType() == ComponentFamilyType("Input"));
}

BOOST_AUTO_TEST_SUITE_END() // Input suite

