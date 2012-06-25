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
 * \file /Ely/src/test/graphicscomponents/Model_test.cpp
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#include "GraphicsComponents/Model.h"
#include <boost/test/unit_test.hpp>
#include "GraphicsSuiteFixture.h"

#include "GraphicsComponents/ModelTemplate.h"
#include <pandaFramework.h>

struct ModelTestCaseFixture
{
	ModelTestCaseFixture() :
			mModel(NULL), mCompId("Model_Test"), mModelTmpl(NULL)
	{
	}

	~ModelTestCaseFixture()
	{
	}
	PT(Model) mModel;
	ComponentId mCompId;
	PT(ModelTemplate) mModelTmpl;
};

PandaFramework* pandaModel;
WindowFramework* winModel;

/// Graphics suite
BOOST_FIXTURE_TEST_SUITE(Graphics, GraphicsSuiteFixture)

//startup common to all test cases
BOOST_AUTO_TEST_CASE(startupModel)
{
	BOOST_TEST_MESSAGE( "startup" );
	int argc = 0;
	char** argv = NULL;
	pandaModel = new PandaFramework();
	pandaModel->open_framework(argc, argv);
	winModel = pandaModel->open_window();
	ModelTemplate::init_type();
	Model::init_type();
}

/// Test cases
BOOST_FIXTURE_TEST_CASE(ModelTemplateTEST, ModelTestCaseFixture)
{
	mModelTmpl = new ModelTemplate(pandaModel, pandaModel->open_window());
	mModelTmpl->resetParameters();
	mModelTmpl->parameter("model_file") = Filename("panda");
	mModel =
	DCAST(Model, mModelTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mModel != NULL);
	BOOST_CHECK(mModel->componentType() == ComponentId("Model"));
	BOOST_CHECK(mModel->familyType() == ComponentFamilyType("Graphics"));
}

//cleanup common to all test cases
BOOST_AUTO_TEST_CASE(cleanupModel)
{
	BOOST_TEST_MESSAGE( "cleanup" );
	pandaModel->close_framework();
	delete pandaModel;
}

BOOST_AUTO_TEST_SUITE_END() // Graphics suite

