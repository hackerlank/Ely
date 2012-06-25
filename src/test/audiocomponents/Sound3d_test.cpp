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

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Audio, AudioSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(Sound3dTEST)
{
	BOOST_TEST_MESSAGE("TESTING Sound3dTemplate");
	mSound3dTmpl = new Sound3dTemplate(mPanda,mWin,audioMgr);
	BOOST_REQUIRE(mSound3dTmpl != NULL);
	mSound3dTmpl->resetParameters();
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
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
