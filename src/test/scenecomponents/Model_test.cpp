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
 * \file /Ely/src/test/scenesomponents/Model_test.cpp
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#include "SceneSuiteFixture.h"

struct ModelTestCaseFixture
{
	ModelTestCaseFixture()
	{
	}
	~ModelTestCaseFixture()
	{
	}
};

/// Scene suite
BOOST_FIXTURE_TEST_SUITE(Scene, SceneSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(ModelTemplateTEST)
{
	mModelTmpl = new ModelTemplate(mPanda, mPanda->open_window());
	mModelTmpl->setParametersDefaults();
	mModelTmpl->parameter("model_file") = Filename("panda");
	mModel =
	DCAST(Model, mModelTmpl->makeComponent(mModelId));
	BOOST_REQUIRE(mModel != NULL);
	BOOST_CHECK(mModel->componentType() == ComponentId("Model"));
	BOOST_CHECK(mModel->familyType() == ComponentFamilyType("Graphics"));
}

BOOST_AUTO_TEST_SUITE_END() // Scene suite

