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
 * \file /Ely/src/test/audiocomponents/Sound3d_test.cpp
 *
 * \date 20/giu/2012 (12:41:32)
 * \author marco
 */

#include "AudioSuiteFixture.h"

struct Sound3dTestCaseFixture
{
	Sound3dTestCaseFixture()
	{
	}

	~Sound3dTestCaseFixture()
	{
	}
};

static float posExpected[] =
{ 0.0, 1.0 / 60.0, 0.0, 1.0 / 30.0, 1.0 / 30.0, 0.0 };
static float velExpected[] =
{ 0.0, 1.0, -1.0, 2.0, 0.0, -2.0 };

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Audio, AudioSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(Sound3dTEST)
{
	BOOST_TEST_MESSAGE("TESTING Sound3dTemplate");
	mSound3dTmpl = new Sound3dTemplate(mPanda,mWin);
	BOOST_REQUIRE(mSound3dTmpl != NULL);
	mSound3dTmpl->setParametersDefaults();
	BOOST_CHECK(mSound3dTmpl->parameterList("sound_files").size() == 0);
	mSound3dTmpl->parameterList("sound_files").push_back(audioFile);
	BOOST_CHECK(mSound3dTmpl->parameterList("sound_files").size() == 1);
	BOOST_TEST_MESSAGE("TESTING Sound3d");
	mSound3d =
	DCAST(Sound3d, mSound3dTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mSound3d != NULL);
	BOOST_CHECK(mSound3d->sounds().size() == 1);
	BOOST_CHECK(mSound3d->componentType() == ComponentId("Sound3d"));
	BOOST_CHECK(mSound3d->familyType() == ComponentFamilyType("Audio"));
	mSound3d->removeSound(audioFile);
	BOOST_CHECK(mSound3d->sounds().size() == 0);
	mSound3d->addSound(audioFile);
	BOOST_CHECK(mSound3d->sounds().size() == 1);
	//add mSound3d to an object so will be automatically destroyed
	Object testObj("testObj",mObjectTmpl);
	mSound3d->ownerObject() = &testObj;
	testObj.addComponent(mSound3d);
	mSound3d->onAddToObjectSetup();
}

BOOST_AUTO_TEST_CASE(Sound3dUpdateTEST)
{
	mSound3dTmpl = new Sound3dTemplate(mPanda,mWin);
	BOOST_REQUIRE(mSound3dTmpl != NULL);
	mSound3dTmpl->setParametersDefaults();
	mSound3dTmpl->parameterList("sound_files").push_back(audioFile);
	mSound3d =
	DCAST(Sound3d, mSound3dTmpl->makeComponent(mCompId));
	BOOST_REQUIRE(mSound3d != NULL);
	//add mSound3d to an object
	GeomNode* testGeom = new GeomNode("testGeom");
	NodePath testNP(testGeom);
	Object testObj("testObj",mObjectTmpl);
	testObj.setNodePath(testNP);
	mSound3d->ownerObject() = &testObj;
	testObj.addComponent(mSound3d);
	BOOST_CHECK(testObj.getComponent("Audio")!=NULL);
	mSound3d->onAddToObjectSetup();
	GenericAsyncTask* task = DCAST(GenericAsyncTask,mPanda->get_task_mgr().find_task("GameAudioManager::update"));
	BOOST_REQUIRE(task != NULL);
	float posx,posy,posz,velx,vely,velz;
	//call update: sound pos & vel = 0.0
	for (unsigned int i = 0; i < (sizeof(posExpected)/sizeof(float)); ++i)
	{
		//move object nodepath (in posExpected)
		testObj.getNodePath().set_pos(posExpected[i],posExpected[i],posExpected[i]);
		//call update (dt = 0.016666667)
		GameAudioManager::GetSingleton().update(task);
		//check results
		mSound3d->sounds()[audioFile]->get_3d_attributes(&posx,&posy,&posz,&velx,&vely,&velz);
		BOOST_CHECK_CLOSE( posx, posExpected[i], 1.0);
		BOOST_CHECK_CLOSE( posy, posExpected[i], 1.0);
		BOOST_CHECK_CLOSE( posz, posExpected[i], 1.0);
		BOOST_CHECK_CLOSE( velx, velExpected[i], 1.0);
		BOOST_CHECK_CLOSE( vely, velExpected[i], 1.0);
		BOOST_CHECK_CLOSE( velz, velExpected[i], 1.0);
	}
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
