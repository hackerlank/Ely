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

struct ObjectTestCaseFixture: public ObjectModelSuiteFixture
{
	ObjectTestCaseFixture()
	{
//		mModelTmpl = new ModelTemplate(mPandaFrmwk, mWindowFrmwk);
//		mModel = new Model(mModelTmpl);
//		mObject = new Object(ObjectId("Test1Object"));
	}

	~ObjectTestCaseFixture()
	{
//		delete mObject;
//		delete mModel;
//		delete mModelTmpl;
	}
//	Model* mModel;
//	ModelTemplate* mModelTmpl;
//	Object* mObject;
};

/// ObjectModel suite
BOOST_FIXTURE_TEST_SUITE(ObjectModel, ObjectModelSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(ObjectConstructor, ObjectTestCaseFixture)
{
//	BOOST_CHECK(mObject->objectId()==ObjectId("TestObject"));
//	BOOST_CHECK(mObject->nodePath().is_empty());
}

BOOST_FIXTURE_TEST_CASE(ObjectComponents, ObjectTestCaseFixture)
{
//	mObject->addComponent(mModel);
//	BOOST_CHECK(mObject->);
}

BOOST_AUTO_TEST_SUITE_END() // ObjectModel suite
