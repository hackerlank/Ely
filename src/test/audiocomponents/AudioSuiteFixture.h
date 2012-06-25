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
 * \file /Ely/src/test/audiocomponents/AudioSuiteFixture.h
 *
 * \date 20/giu/2012 (12:42:25)
 * \author marco
 */

#ifndef AUDIOSUITEFIXTURE_H_
#define AUDIOSUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include "AudioComponents/Sound3d.h"
#include "AudioComponents/Sound3dTemplate.h"
#include <pointerTo.h>
#include <audioManager.h>
#include <audioSound.h>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <string>

struct AudioSuiteFixture
{
	AudioSuiteFixture() :
		mSound3dTmpl(NULL), mSound3d(NULL), audioMgr(NULL), mCompId(
				"Sound3d_Test")
	{
		audioFile = "/usr/share/panda3d/models/audio/sfx/GUI_rollover.wav";
		int argc = 0;
		char** argv = NULL;
		mPanda = new PandaFramework();
		mPanda->open_framework(argc, argv);
		mWin = mPanda->open_window();
		audioMgr = AudioManager::create_AudioManager();
		Sound3dTemplate::init_type();
		Sound3d::init_type();
	}

	~AudioSuiteFixture()
	{
		mPanda->close_framework();
				delete mPanda;
	}
	PandaFramework* mPanda;
	WindowFramework* mWin;
	std::string audioFile;
	PT(Sound3dTemplate) mSound3dTmpl;
	PT(Sound3d) mSound3d;
	PT(AudioManager) audioMgr;
	ComponentId mCompId;
};

#endif /* AUDIOSUITEFIXTURE_H_ */
