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
 * \file /Ely/src/AudioComponents/Sound3dTemplate.cpp
 *
 * \date 20/giu/2012 (12:48:00)
 * \author marco
 */

#include "AudioComponents/Sound3dTemplate.h"

Sound3dTemplate::Sound3dTemplate(AudioManager* audioMgr)
{
	if (not audioMgr)
	{
		throw GameException(
				"Sound3dTemplate::Sound3dTemplate: invalid AudioManager");

	}
	mAudioMgr = audioMgr;
	resetParameters();
}

Sound3dTemplate::~Sound3dTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType Sound3dTemplate::componentType() const
{
	return ComponentType("Sound3d");
}

const ComponentFamilyType Sound3dTemplate::familyType() const
{
	return ComponentFamilyType("Audio");
}

Component* Sound3dTemplate::makeComponent(ComponentId& compId)
{
	Sound3d* newSound3d = new Sound3d(this);
	newSound3d->componentId() = compId;
	if (not newSound3d->initialize())
	{
		return NULL;
	}
	return newSound3d;
}

void Sound3dTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTable::iterator iter;
	pair<ParameterTable::iterator, ParameterTable::iterator> iterRange;
	//set sounds filenames
	iterRange = parameterTable.equal_range("sound_file");
	for (iter = iterRange.first; iter != iterRange.second; ++iter)
	{
		mSoundFiles.push_back(iter->second);
	}
}

void Sound3dTemplate::resetParameters()
{
	//set component parameters default values
	mSoundFiles.clear();
}

std::list<std::string>& Sound3dTemplate::soundFiles()
{
	return mSoundFiles;
}

AudioManager*& Sound3dTemplate::audioManager()
{
	return mAudioMgr;
}

//TypedObject semantics: hardcoded
TypeHandle Sound3dTemplate::_type_handle;
