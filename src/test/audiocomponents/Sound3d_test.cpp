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

#include <boost/test/unit_test.hpp>
#include "AudioSuiteFixture.h"

struct Sound3dTestCaseFixture
{
	Sound3dTestCaseFixture()
	{
//		mCompId = "Sound3d_Test";
	}

	~Sound3dTestCaseFixture()
	{
	}
//	PT(Sound3dTemplate) mSound3dTmpl;
//	PT(Sound3d) mSound3d;
//	ComponentId mCompId;
};

//PandaFramework* panda;
//PT(AudioManager) audioMgr;
//std::string audioFile = "/usr/share/panda3d/models/audio/sfx/GUI_rollover.wav";

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Audio, AudioSuiteFixture)

//startup common to all test cases
BOOST_AUTO_TEST_CASE(startup)
{
//	BOOST_TEST_MESSAGE( "startup" );
//	panda = new PandaFramework();
//	audioMgr = AudioManager::create_AudioManager();
//	Sound3dTemplate::init_type();
//	Sound3d::init_type();
}

/// Test cases
BOOST_FIXTURE_TEST_CASE(Sound3dTESTS,Sound3dTestCaseFixture)
{
//	BOOST_TEST_MESSAGE( "Sound3dTemplateTEST" );
//	mSound3dTmpl = new Sound3dTemplate(panda,panda->open_window(),audioMgr);
//	BOOST_REQUIRE(mSound3dTmpl != NULL);
//	mSound3dTmpl->resetParameters();
//	BOOST_CHECK(mSound3dTmpl->soundFiles().size() == 0);
//	mSound3dTmpl->soundFiles().push_back(audioFile);
//	BOOST_CHECK(mSound3dTmpl->soundFiles().size() == 1);
//	BOOST_TEST_MESSAGE( "Sound3dTEST" );
//	mSound3d =
//	DCAST(Sound3d, mSound3dTmpl->makeComponent(mCompId));
//	BOOST_REQUIRE(mSound3d != NULL);
//	BOOST_CHECK(mSound3d->sounds().size() == 1);
//	BOOST_CHECK(mSound3d->componentType() == ComponentId("Sound3d"));
//	BOOST_CHECK(mSound3d->familyType() == ComponentFamilyType("Audio"));
//	mSound3d->removeSound(audioFile);
//	BOOST_CHECK(mSound3d->sounds().size() == 0);
//	mSound3d->addSound(audioFile);
//	BOOST_CHECK(mSound3d->sounds().size() == 1);
}

BOOST_AUTO_TEST_CASE(cleanup)
{
//	BOOST_TEST_MESSAGE( "cleanup" );
//	panda->close_framework();
//	delete panda;
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
