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

#include "ObjectModelSuiteFixture.h"

struct ObjectTestCaseFixture
{
	ObjectTestCaseFixture() :
			mObject(NULL), mObjectTmpl(NULL), mModel(NULL)
	{
	}

	~ObjectTestCaseFixture()
	{
	}
	PT(Object) mObject;
	PT(ObjectTemplate) mObjectTmpl;
	PT(Model) mModel;
};

PandaFramework* pandaObject;
WindowFramework* mWin;

/// ObjectModel suite
BOOST_FIXTURE_TEST_SUITE(ObjectModel, ObjectModelSuiteFixture)

//startup common to all test cases
BOOST_AUTO_TEST_CASE(startupObject)
{
	BOOST_TEST_MESSAGE( "startup" );
	pandaObject = new PandaFramework();
	mWin = pandaObject->open_window();
	Object::init_type();
	ObjectTemplate::init_type();
	Model::init_type();
}
/// Test cases
BOOST_FIXTURE_TEST_CASE(ObjectTemplateMethods, ObjectTestCaseFixture)
{
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(ObjectConstructorTEST, ObjectTestCaseFixture)
{
	ObjectTemplateManager mObjectTmplMgr;
	mObjectTmpl = new ObjectTemplate(ObjectType("Object_test"),ObjectTemplateManager::GetSingletonPtr(),pandaObject,mWin);
	mObject = new Object(ObjectId("TestObject"), mObjectTmpl);
	BOOST_CHECK(mObject->objectId()==ObjectId("TestObject"));
	BOOST_CHECK(mObject->numComponents() == 0);
	BOOST_CHECK(mObject->nodePath().is_empty());
}

BOOST_FIXTURE_TEST_CASE(ObjectComponentsTEST, ObjectTestCaseFixture)
{
	ObjectTemplateManager mObjectTmplMgr;
	mObjectTmpl = new ObjectTemplate(ObjectType("Object_test"),ObjectTemplateManager::GetSingletonPtr(),pandaObject,mWin);
	mObject = new Object(ObjectId("TestObject"), mObjectTmpl);
	mModel = new Model();
	mObject->addComponent(mModel);
	BOOST_CHECK(mObject->getComponent(ComponentFamilyType("Graphics"))==mModel);
	BOOST_CHECK(mObject->numComponents() == 1);
	mObject->addComponent(mModel);
	BOOST_CHECK(mObject->numComponents() == 1);
	mObject->clearComponents();
	BOOST_CHECK(mObject->numComponents() == 0);
}

//cleanup common to all test cases
BOOST_AUTO_TEST_CASE(cleanupObject)
{
	BOOST_TEST_MESSAGE( "cleanup" );
	pandaObject->close_framework();
	delete pandaObject;
}

BOOST_AUTO_TEST_SUITE_END() // ObjectModel suite
