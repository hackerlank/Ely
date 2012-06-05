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
			mModel(NULL), mCompId("Model_Test")
	{
		int argc = 0;
		char** argv = NULL;
		mPanda = new PandaFramework();
		mPanda->open_framework(argc, argv);
		mModelTmpl = new ModelTemplate(mPanda, mPanda->open_window());
		ModelTemplate::init_type();
		Model::init_type();
	}

	~ModelTestCaseFixture()
	{
		if (mModel)
		{
			delete mModel;
		}
		delete mModelTmpl;
		mPanda->close_framework();
		delete mPanda;
	}
	PandaFramework* mPanda;
	ModelTemplate* mModelTmpl;
	Model* mModel;
	ComponentId mCompId;
};

/// Graphics suite
BOOST_FIXTURE_TEST_SUITE(Graphics, GraphicsSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(ModelTemplateTEST, ModelTestCaseFixture)
{
	mModelTmpl->reset();
	mModelTmpl->modelFile() = Filename("panda");
	mModel =
	DCAST(Model, mModelTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mModel != NULL);
	BOOST_CHECK(mModel->componentType() == ComponentId("Model"));
	BOOST_CHECK(mModel->familyType() == ComponentFamilyType("Graphics"));
}

BOOST_AUTO_TEST_SUITE_END() // Graphics suite

