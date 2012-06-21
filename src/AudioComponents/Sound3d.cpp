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
 * \file /Ely/src/AudioComponents/Sound3d.cpp
 *
 * \date 20/giu/2012 (12:41:32)
 * \author marco
 */

#include "AudioComponents/Sound3d.h"

Sound3d::Sound3d()
{
	// TODO Auto-generated constructor stub
}

Sound3d::Sound3d(Sound3dTemplate* tmpl) :
		mTmpl(tmpl)
{
}

Sound3d::~Sound3d()
{
	// TODO Auto-generated destructor stub
}

const ComponentFamilyType Sound3d::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Sound3d::componentType() const
{
	return mTmpl->componentType();
}

bool Sound3d::initialize()
{
	bool result = true;
	//set AudioManager
	mAudioMgr = mTmpl->audioManager();
	std::list<std::string>::iterator it;
	for (it = mTmpl->soundFiles().begin(); it != mTmpl->soundFiles().end(); ++it)
	{
		addSound(*it);
	}
	//
	return result;
}

void Sound3d::onAddSetup()
{
}

bool Sound3d::addSound(const std::string& fileName)
{
}

bool Sound3d::removeSound(const std::string& soundName)
{
}

void Sound3d::setSoundMinDistance(AudioSound* sound, float dist)
{
}

float Sound3d::getSoundMinDistance(AudioSound* sound)
{
}

void Sound3d::setSoundMaxDistance(AudioSound* sound, float dist)
{
}

float Sound3d::getSoundMaxDistance(AudioSound* sound)
{
}

void Sound3d::setSoundVelocity(AudioSound* sound, const LVector3& velocity)
{
}

void Sound3d::setSoundVelocityAuto(AudioSound* sound)
{
}

LVector3 Sound3d::getSoundVelocity(AudioSound* sound)
{
}

void Sound3d::attachSound(AudioSound* sound)
{
}

void Sound3d::detachSound(AudioSound* sound)
{
}

SoundTable& Sound3d::sounds()
{
}

AsyncTask::DoneStatus Sound3d::update(GenericAsyncTask* task)
{
}

//TypedObject semantics: hardcoded
TypeHandle Sound3d::_type_handle;

