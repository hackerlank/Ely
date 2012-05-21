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
 * \file /Ely/src/test/graphicscomponents/ModelTemplate_test.cpp
 *
 * \date 16/mag/2012 (16:26:41)
 * \author marco
 */

#include "../../../include/GraphicsComponents/ModelTemplate.h"
#include <boost/test/unit_test.hpp>
#include "GraphicsSuiteFixture.h"

struct ModelTemplateCaseFixture
{
	ModelTemplateCaseFixture()
	{
		// TODO
	}

	~ModelTemplateCaseFixture()
	{
		// TODO
	}
};

/// Graphics suite
BOOST_FIXTURE_TEST_SUITE(Graphics, GraphicsSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(ModelTemplate, ModelTemplateCaseFixture)
{
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // Graphics suite

