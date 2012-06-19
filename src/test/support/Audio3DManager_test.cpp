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
 * \file /Ely/src/test/support/Audio3DManager_test.cpp
 *
 * \date 19/giu/2012 (09:02:09)
 * \author marco
 */

#include "Support/Audio3DManager.h"
#include <boost/test/unit_test.hpp>
#include "SupportSuiteFixture.h"

struct Audio3DManagerTestCaseFixture
{
	Audio3DManagerTestCaseFixture()
	{
		// TODO
	}

	~Audio3DManagerTestCaseFixture()
	{
		// TODO
	}
};

PandaFramework* panda;
PT(AudioManager) audio_manager;
std::string audio_file = "/usr/share/panda3d/models/audio/sfx/GUI_rollover.wav";
NodePath listener_target, render;

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Support,SupportSuiteFixture)

BOOST_AUTO_TEST_CASE(startup)
{
	BOOST_TEST_MESSAGE( "startup" );
	panda = new PandaFramework();
	render = panda->open_window()->get_render();
	audio_manager = AudioManager::create_AudioManager();
}
/// Test cases
BOOST_FIXTURE_TEST_CASE(Audio3DManagerTEST,Audio3DManagerTestCaseFixture)
{
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // Input suite
