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
 * \file /Ely/src/test/physicscomponents/RigidBody_test.cpp
 *
 * \date 07/lug/2012 (15:58:35)
 * \author consultit
 */

#include "PhysicsSuiteFixture.h"

struct RigidBodyTestCaseFixture
{
	RigidBodyTestCaseFixture()
	{
	}

	~RigidBodyTestCaseFixture()
	{
	}
};

/// Physics suite
BOOST_FIXTURE_TEST_SUITE(Physics, PhysicsSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(RigidBodyTEST)
{
	BOOST_TEST_MESSAGE("TESTING RigidBodyTemplate");
	mRigidTmpl = new RigidBodyTemplate(mPanda,mWin);
	BOOST_REQUIRE(mRigidTmpl != NULL);
	mRigidTmpl->setParametersDefaults();
	BOOST_CHECK(mRigidTmpl->parameter("body_type") == std::string("dynamic"));
	BOOST_CHECK(mRigidTmpl->parameter("body_mass") == std::string("1.0"));
	BOOST_CHECK(mRigidTmpl->parameter("shape_type") == std::string("sphere"));
	BOOST_CHECK(mRigidTmpl->parameter("collide_mask") == std::string("all_on"));
	BOOST_CHECK(mRigidTmpl->parameter("ccd_motion_threshold") == std::string(""));
	BOOST_CHECK(mRigidTmpl->parameter("ccd_swept_sphere_radius") == std::string(""));
	BOOST_TEST_MESSAGE("TESTING RigidBody");
	mRigid =
	DCAST(RigidBody, mRigidTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mRigid != NULL);
	BOOST_CHECK(mRigid->componentType() == ComponentId("RigidBody"));
	BOOST_CHECK(mRigid->familyType() == ComponentFamilyType("Physics"));
	//add mRigid to an object
	GeomNode* testGeom = new GeomNode("testGeom");
	NodePath testNP(testGeom);
	Object testObj("testObj",mObjectTmpl);
	testObj.setNodePath(testNP);
	mRigid->setOwnerObject(&testObj);
	testObj.addComponent(mRigid.p());
	mRigid->onAddToSceneSetup();
	BOOST_CHECK(DCAST(BulletRigidBodyNode, testObj.getNodePath().node()));
}

BOOST_AUTO_TEST_SUITE_END() // Physics suite
