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
 * \author consultit
 */

#include "ObjectModelSuiteFixture.h"

struct ObjectTestCaseFixture
{
	ObjectTestCaseFixture()
	{
	}
	~ObjectTestCaseFixture()
	{
	}
};

/// ObjectModel suite
BOOST_FIXTURE_TEST_SUITE(ObjectModel, ObjectModelSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(ObjectConstructorTEST)
{
	mObjectTmpl = new ObjectTemplate(ObjectType("Object_test"),ObjectTemplateManager::GetSingletonPtr(),mPanda,mWin);
	mObject = new Object(ObjectId("TestObject"), mObjectTmpl);
	BOOST_CHECK(mObject->objectId()==ObjectId("TestObject"));
	BOOST_CHECK(mObject->numComponents() == 0);
	BOOST_CHECK(mObject->getNodePath().is_empty());
}

BOOST_AUTO_TEST_CASE(ObjectComponentsTEST)
{
	mObjectTmpl = new ObjectTemplate(ObjectType("Object_test"),ObjectTemplateManager::GetSingletonPtr(),mPanda,mWin);
	mObject = new Object(ObjectId("TestObject"), mObjectTmpl);
	mModelTmpl = new ModelTemplate(mPanda,mWin);
	ParameterTable paramTable;
	paramTable.insert(std::pair<std::string,std::string>("model_file","panda"));
	mModelTmpl->setParameters(paramTable);
	mModel =
	DCAST(Model, mModelTmpl->makeComponent(ComponentId("TestModel")));
	mObject->doAddComponent(mModel.p());
	BOOST_CHECK(mObject->getComponent(ComponentFamilyType("Scene"))==mModel);
	BOOST_CHECK(mObject->numComponents() == 1);
	mObject->doAddComponent(mModel.p());
	BOOST_CHECK(mObject->numComponents() == 1);
	mObject->clearComponents();
	BOOST_CHECK(mObject->numComponents() == 0);
}

BOOST_AUTO_TEST_SUITE_END() // ObjectModel suite
