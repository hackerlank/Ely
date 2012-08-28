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
 * \file /Ely/src/test/scenecomponents/SceneSuiteFixture.h
 *
 * \date 15/mag/2012 (15:33:43)
 * \author marco
 */

#ifndef SCENESUITEFIXTURE_H_
#define SCENESUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include <pandaFramework.h>
#include <windowFramework.h>
#include "SceneComponents/InstanceOf.h"
#include "SceneComponents/InstanceOfTemplate.h"
#include "SceneComponents/ModelTemplate.h"
#include "SceneComponents/Model.h"
#include "SceneComponents/Terrain.h"
#include "SceneComponents/TerrainTemplate.h"
#include "Utilities/Tools.h"

struct SceneSuiteFixture
{
	SceneSuiteFixture() :
			mInstanceOf(NULL), mInstanceOfId("InstanceOf_Test"), mModel(NULL), mModelId(
					"Model_Test"), mModelTmpl(NULL)
	{
		InstanceOf::init_type();
		InstanceOfTemplate::init_type();
		int argc = 0;
		char** argv = NULL;
		mPanda = new PandaFramework();
		mPanda->open_framework(argc, argv);
		mWin = mPanda->open_window();
		mInstanceOfTmpl = new InstanceOfTemplate(mPanda, mWin);
		ModelTemplate::init_type();
		Model::init_type();
	}

	~SceneSuiteFixture()
	{
		//delete components before their templates
		if (mInstanceOf)
		{
			delete mInstanceOf;
		}
		if (mInstanceOfTmpl)
		{
			delete mInstanceOfTmpl;
		}
		if (mModel)
		{
			delete mModel;
		}
		if (mModelTmpl)
		{
			delete mModelTmpl;
		}
		mPanda->close_framework();
		delete mPanda;
	}
	SMARTPTR(InstanceOf) mInstanceOf;
	ComponentId mInstanceOfId;
	SMARTPTR(InstanceOfTemplate) mInstanceOfTmpl;
	SMARTPTR(Model) mModel;
	ComponentId mModelId;
	SMARTPTR(ModelTemplate) mModelTmpl;
	PandaFramework* mPanda;
	WindowFramework* mWin;
};

#endif /* SCENESUITEFIXTURE_H_ */
