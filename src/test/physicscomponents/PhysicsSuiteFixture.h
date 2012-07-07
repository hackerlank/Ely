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
 * \file /Ely/src/test/physicscomponents/PhysicsSuiteFixture.h
 *
 * \date 07/lug/2012 (17:14:03)
 * \author marco
 */

#ifndef PHYSICSSUITEFIXTURE_H_
#define PHYSICSSUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include "PhysicsComponents/RigidBody.h"
#include "PhysicsComponents/RigidBodyTemplate.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include <pandaFramework.h>
#include <nodePath.h>
#include <geomNode.h>
#include <genericAsyncTask.h>

struct PhysicsSuiteFixture
{
	PhysicsSuiteFixture() :
			mRigid(NULL), mCompId("RigidBody_Test"), mRigidTmpl(NULL), mObjectTmpl(
					NULL)
	{
		int argc = 0;
		char** argv = NULL;
		mPanda = new PandaFramework();
		mPanda->open_framework(argc, argv);
		mWin = mPanda->open_window();
		RigidBodyTemplate::init_type();
		RigidBody::init_type();
		ObjectTemplate::init_type();
		Object::init_type();
	}
	~PhysicsSuiteFixture()
	{
		//delete always objects/components before their templates
		if (mObjectTmpl)
		{
			delete mObjectTmpl;
		}
		if (mRigid)
		{
			delete mRigid;
		}
		if (mRigidTmpl)
		{
			delete mRigidTmpl;
		}
		mPanda->close_framework();
		delete mPanda;
	}
	RigidBody* mRigid;
	ComponentId mCompId;
	RigidBodyTemplate* mRigidTmpl;
	ObjectTemplate* mObjectTmpl;
	PandaFramework* mPanda;
	WindowFramework* mWin;
	ObjectTemplateManager mObjectTmplMgr;
};

#endif /* PHYSICSSUITEFIXTURE_H_ */
