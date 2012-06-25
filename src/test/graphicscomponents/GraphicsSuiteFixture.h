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
 * \file /Ely/src/test/graphicscomponents/GraphicsSuiteFixture.h
 *
 * \date 15/mag/2012 (15:33:43)
 * \author marco
 */

#ifndef GRAPHICSSUITEFIXTURE_H_
#define GRAPHICSSUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include <pandaFramework.h>
#include <windowFramework.h>
#include "GraphicsComponents/InstanceOf.h"
#include "GraphicsComponents/InstanceOfTemplate.h"
#include "GraphicsComponents/ModelTemplate.h"
#include "GraphicsComponents/Model.h"
#include "Utilities/Tools.h"

struct GraphicsSuiteFixture
{
	GraphicsSuiteFixture() :
			mInstanceOf(NULL), mInstanceOfId("InstanceOf_Test"), mInstanceOfTmpl(
					new InstanceOfTemplate()), mModel(NULL), mModelId(
					"Model_Test"), mModelTmpl(NULL)
	{
		InstanceOf::init_type();
		InstanceOfTemplate::init_type();
		int argc = 0;
		char** argv = NULL;
		mPanda = new PandaFramework();
		mPanda->open_framework(argc, argv);
		mWin = mPanda->open_window();
		ModelTemplate::init_type();
		Model::init_type();
	}

	~GraphicsSuiteFixture()
	{
		mPanda->close_framework();
		delete mPanda;
	}
	PT(InstanceOf) mInstanceOf;
	ComponentId mInstanceOfId;
	PT(InstanceOfTemplate) mInstanceOfTmpl;
	PT(Model) mModel;
	ComponentId mModelId;
	PT(ModelTemplate) mModelTmpl;
	PandaFramework* mPanda;
	WindowFramework* mWin;
};

#endif /* GRAPHICSSUITEFIXTURE_H_ */
