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
 * \file /Ely/src/test/objectmodel/Object_test.cpp
 *
 * \date 07/mag/2012 (18:10:37)
 * \author marco
 */

#include "ObjectModel/Object.h"
#include <boost/test/unit_test.hpp>
#include "ObjectModelSuiteFixture.h"

#include "ObjectModel/ObjectTemplate.h"
#include "GraphicsComponents/Model.h"

struct ObjectTestCaseFixture
{
	ObjectTestCaseFixture()
	{
//		mObject = new Object(ObjectId("TestObject"));
//		mModel = new Model();
	}

	~ObjectTestCaseFixture()
	{
//		//delete mModel;: owned by mObject
//		delete mObject;
	}
//	Object* mObject;
//	Model* mModel;
};

/// ObjectModel suite
BOOST_FIXTURE_TEST_SUITE(ObjectModel, ObjectModelSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(ObjectTemplateMethods, ObjectTestCaseFixture)
{
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(ObjectConstructorTEST, ObjectTestCaseFixture)
{
//	BOOST_CHECK(mObject->objectId()==ObjectId("TestObject"));
//	BOOST_CHECK(mObject->numComponents() == 0);
//	BOOST_CHECK(mObject->nodePath().is_empty());
}

BOOST_FIXTURE_TEST_CASE(ObjectComponentsTEST, ObjectTestCaseFixture)
{
//	mObject->addComponent(mModel);
//	BOOST_CHECK(mObject->getComponent(ComponentFamilyType("Graphics"))==mModel);
//	BOOST_CHECK(mObject->numComponents() == 1);
//	mObject->addComponent(mModel);
//	BOOST_CHECK(mObject->numComponents() == 1);
//	mObject->clearComponents();
//	BOOST_CHECK(mObject->numComponents() == 0);
}

BOOST_AUTO_TEST_SUITE_END() // ObjectModel suite
