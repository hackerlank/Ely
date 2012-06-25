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
 * \file /Ely/src/test/objectmodel/ComponentTemplateManager_test.cpp
 *
 * \date 11/mag/2012 (17:48:53)
 * \author marco
 */

#include "ObjectModel/ComponentTemplateManager.h"
#include <boost/test/unit_test.hpp>
#include "ObjectModelSuiteFixture.h"

struct ComponentTemplateManagerTestCaseFixture
{
	ComponentTemplateManagerTestCaseFixture()
	{
		// TODO
	}

	~ComponentTemplateManagerTestCaseFixture()
	{
		// TODO
	}
};

/// ObjectModel suite
BOOST_FIXTURE_TEST_SUITE(ObjectModel, ObjectModelSuiteFixture)

//startup common to all test cases
BOOST_AUTO_TEST_CASE(startupComponentTemplateManager)
{
	BOOST_TEST_MESSAGE( "startup" );
}

/// Test cases
BOOST_FIXTURE_TEST_CASE(ComponentTemplateManagerTEST, ComponentTemplateManagerTestCaseFixture)
{
	BOOST_CHECK(true);
}

//cleanup common to all test cases
BOOST_AUTO_TEST_CASE(cleanupComponentTemplateManager)
{
	BOOST_TEST_MESSAGE( "cleanup" );
}

BOOST_AUTO_TEST_SUITE_END() // ObjectModel suite
