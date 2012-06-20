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

#include "AudioComponents/Sound3d.h"
#include <boost/test/unit_test.hpp>
#include "AudioSuiteFixture.h"

#include "AudioComponents/Sound3dTemplate.h"
#include <nodePath.h>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <pointerTo.h>
#include <audioManager.h>
#include <string>

struct Sound3dTestCaseFixture
{
	Sound3dTestCaseFixture()
	{
		// TODO
	}

	~Sound3dTestCaseFixture()
	{
		// TODO
	}
};

PandaFramework* panda;
WindowFramework* window;
PT(AudioManager) audio_manager;
std::string audio_file = "/usr/share/panda3d/models/audio/sfx/GUI_rollover.wav";
NodePath listener_target, render, camera, object;

/// Input suite
BOOST_FIXTURE_TEST_SUITE(Sound3d,AudioSuiteFixture)

BOOST_AUTO_TEST_CASE(startup)
{
	BOOST_TEST_MESSAGE( "startup" );
	panda = new PandaFramework();
	window = panda->open_window();
	render = window->get_render();
	camera = window->get_camera_group();
	object = render.attach_new_node("object");
	audio_manager = AudioManager::create_AudioManager();
}

/// Test cases
BOOST_FIXTURE_TEST_CASE(Audio3DManagerTEST,Sound3dTestCaseFixture)
{
//	PT(Audio3DManager) audio3dMgr= new Audio3DManager(audio_manager,camera);
//	BOOST_REQUIRE(not audio3dMgr.is_null());
//	PT(AudioSound) audioSound = audio3dMgr->loadSfx(audio_file);
//	BOOST_REQUIRE(not audioSound.is_null());
//	audio3dMgr->attachSoundToObject(audioSound,object);
//	BOOST_CHECK(audio3dMgr->getSoundsOnObject(object).size() == 1);
//	audio3dMgr->detachSound(audioSound);
//	BOOST_CHECK(audio3dMgr->getSoundsOnObject(object).empty());
}

BOOST_AUTO_TEST_CASE(cleanup)
{
	BOOST_TEST_MESSAGE( "cleanup" );
	panda->close_framework();
	delete panda;
}
BOOST_AUTO_TEST_SUITE_END() // Input suite
