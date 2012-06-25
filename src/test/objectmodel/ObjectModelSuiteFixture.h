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
 * \file /Ely/src/test/objectmodel/ObjectModelSuiteFixture.h
 *
 * \date 11/mag/2012 (18:05:46)
 * \author marco
 */

#ifndef OBJECTMODELSUITEFIXTURE_H_
#define OBJECTMODELSUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include "ObjectModel/ComponentTemplateManager.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "ObjectModel/ObjectTemplate.h"
#include "ObjectModel/Object.h"
#include "GraphicsComponents/Model.h"

struct ObjectModelSuiteFixture
{
	ObjectModelSuiteFixture() :
			mObject(NULL), mObjectTmpl(NULL), mModel(NULL)
	{
		mPanda = new PandaFramework();
		mWin = mPanda->open_window();
		Object::init_type();
		ObjectTemplate::init_type();
		Model::init_type();
	}
	~ObjectModelSuiteFixture()
	{
		mPanda->close_framework();
		delete mPanda;
	}
	PT(Object) mObject;
	PT(ObjectTemplate) mObjectTmpl;
	PT(Model) mModel;
	PandaFramework* mPanda;
	WindowFramework* mWin;
	ObjectTemplateManager mObjectTmplMgr;
};

#endif /* OBJECTMODELSUITEFIXTURE_H_ */
