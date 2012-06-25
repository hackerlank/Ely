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
 * \file /Ely/src/test/objectmodel/ObjectTemplateManager_test.cpp
 *
 * \date 13/mag/2012 (10:16:20)
 * \author marco
 */

#include "ObjectModelSuiteFixture.h"

struct ObjectTemplateManagerTestCaseFixture
{
	ObjectTemplateManagerTestCaseFixture()
	{
	}
	~ObjectTemplateManagerTestCaseFixture()
	{
	}
};

/// ObjectModel suite
BOOST_FIXTURE_TEST_SUITE(ObjectModel, ObjectModelSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(ObjectTemplateManagerTEST)
{
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // ObjectModel suite

