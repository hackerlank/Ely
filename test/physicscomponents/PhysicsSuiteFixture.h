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
 * \author consultit
 */

#ifndef PHYSICSSUITEFIXTURE_H_
#define PHYSICSSUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include "PhysicsComponents/RigidBody.h"
#include "PhysicsComponents/RigidBodyTemplate.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GamePhysicsManager.h"
#include <pandaFramework.h>
#include <nodePath.h>
#include <geomNode.h>
#include <genericAsyncTask.h>

using namespace ely;

struct PhysicsSuiteFixture
{
	PhysicsSuiteFixture() :
			mRigid(NULL), mCompId("RigidBody_Test"), mRigidTmpl(NULL), mObjectTmpl(
					NULL), physicsMgr(NULL)
	{
		int argc = 0;
		char** argv = NULL;
		mPanda = new PandaFramework();
		mPanda->open_framework(argc, argv);
		mWin = mPanda->open_window();
		physicsMgr = new GamePhysicsManager();
		RigidBodyTemplate::init_type();
		RigidBody::init_type();
		CharacterControllerTemplate::init_type();
		CharacterController::init_type();
		ObjectTemplate::init_type();
		Object::init_type();
	}
	~PhysicsSuiteFixture()
	{
		//delete audio manager
		if (physicsMgr)
		{
			delete physicsMgr;
		}
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
		if (mChar)
		{
			delete mChar;
		}
		if (mCharTmpl)
		{
			delete mCharTmpl;
		}
		mPanda->close_framework();
		delete mPanda;
	}
	SMARTPTR(RigidBody) mRigid;
	SMARTPTR(CharacterController) mChar;
	ComponentId mCompId;
	SMARTPTR(RigidBodyTemplate) mRigidTmpl;
	SMARTPTR(CharacterControllerTemplate) mCharTmpl;
	SMARTPTR(ObjectTemplate) mObjectTmpl;
	GamePhysicsManager* physicsMgr;
	PandaFramework* mPanda;
	WindowFramework* mWin;
};

#endif /* PHYSICSSUITEFIXTURE_H_ */
