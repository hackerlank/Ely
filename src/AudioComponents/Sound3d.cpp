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
#include "AudioComponents/Sound3dTemplate.h"

Sound3d::Sound3d()
{
	// TODO Auto-generated constructor stub
}

Sound3d::Sound3d(Sound3dTemplate* tmpl) :
		mMinDist(1.0), mMaxDist(1000000000.0)
{
	mTmpl = tmpl;
	mSounds.clear();
	mPosition = LPoint3(0.0, 0.0, 0.0);
}

Sound3d::~Sound3d()
{
	//check if game audio manager exists
	if (GameAudioManager::GetSingletonPtr())
	{
		GameAudioManager::GetSingletonPtr()->removeFromAudioUpdate(this);
	}
	//stops every playing sounds
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		if (iter->second->status() == AudioSound::PLAYING)
		{
			iter->second->stop();
		}
	}
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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//set sound files
	std::list<std::string>::iterator iter;
	std::list<std::string> soundFileList = mTmpl->parameterList(
			std::string("sound_files"));
	for (iter = soundFileList.begin(); iter != soundFileList.end(); ++iter)
	{
		PT(AudioSound) sound =
				GameAudioManager::GetSingletonPtr()->audioMgr()->get_sound(
						*iter, true);
		if (not sound.is_null())
		{
			mSounds[*iter] = sound;
		}
	}
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void Sound3d::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	// update sounds' position/velocity only for dynamic objects
	// and if sound table is not empty
	if (GameAudioManager::GetSingletonPtr() and (not mOwnerObject->isStatic())
			and (not mSounds.empty()))
	{
		GameAudioManager::GetSingletonPtr()->addToAudioUpdate(this);
	}
	//set the root of the scene
	mSceneRoot = mTmpl->windowFramework()->get_render();
	//register event callbacks if any
	registerEventCallbacks();
}

void Sound3d::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mOwnerObject->isStatic())
	{
		//set 3d attribute (in this case static)
		set3dStaticAttributes();
	}
}

bool Sound3d::addSound(const std::string& fileName)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//make mSounds modifications
	bool result = false;
	if (not mOwnerObject)
	{
		//no owner object:return false
		return result;
	}
	PT(AudioSound) sound =
			GameAudioManager::GetSingletonPtr()->audioMgr()->get_sound(fileName,
					true);
	if (sound)
	{
		mSounds[fileName] = sound;
		result = true;
	}
	// try to add this component to update only if object is dynamic
	// and if sound table is not empty
	if (GameAudioManager::GetSingletonPtr() and (not mOwnerObject->isStatic())
			and (not mSounds.empty()))
	{
		//addToAudioUpdate will safely add this component
		//to update only if it wasn't previously added
		GameAudioManager::GetSingletonPtr()->addToAudioUpdate(this);
	}
	//
	return result;
}

bool Sound3d::removeSound(const std::string& soundName)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//make mSounds modifications
	bool result = false;
	size_t removed = mSounds.erase(soundName);
	if (removed == 1)
	{
		result = true;
	}
	// try to remove this component to update if sound table is empty
	if (GameAudioManager::GetSingletonPtr() and (mSounds.empty()))
	{
		//removeFromAudioUpdate will safely remove this component
		//to update only if it was previously added
		GameAudioManager::GetSingletonPtr()->removeFromAudioUpdate(this);
	}
	//
	return result;
}

void Sound3d::setMinDistance(float dist)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mMinDist = dist;
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second->set_3d_min_distance(mMinDist);
	}
}

float Sound3d::getMinDistance()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mMinDist;
}

void Sound3d::setMaxDistance(float dist)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mMaxDist = dist;
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second->set_3d_max_distance(mMaxDist);
	}
}

float Sound3d::getMaxDistance()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mMaxDist;
}

void Sound3d::set3dStaticAttributes()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mPosition = mOwnerObject->getNodePath().get_pos(mSceneRoot);
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second->set_3d_attributes(mPosition.get_x(), mPosition.get_y(),
				mPosition.get_z(), 0.0, 0.0, 0.0);
	}
}

AudioSound* Sound3d::getSound(const std::string& name)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SoundTable::iterator iter = mSounds.find(name);
	if (iter == mSounds.end())
	{
		return NULL;
	}
	return iter->second.p();
}

AudioSound* Sound3d::getSound(int index)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SoundTable::iterator iter;
	unsigned int idx;
	for (idx = 0, iter = mSounds.begin(); idx < mSounds.size(); ++idx, ++iter)
	{
		if (idx == (unsigned int) index)
		{
			break;
		}
	}
	if (idx == mSounds.size())
	{
		return NULL;
	}
	return iter->second.p();
}

void Sound3d::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

	//get the new position
	//note on threading: this should be an atomic operation
	LPoint3 newPosition = mOwnerObject->getNodePath().get_pos(mSceneRoot);
	//get the velocity (mPosition holds the previous position)
	LVector3 deltaPos = (newPosition - mPosition);
	LVector3 velocity = deltaPos / dt;
	//update sounds' velocity and position
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		//note on threading: this should be an atomic operation
		iter->second->set_3d_attributes(newPosition.get_x(),
				newPosition.get_y(), newPosition.get_z(), velocity.get_x(),
				velocity.get_y(), velocity.get_z());
	}
	//update actual position
	mPosition = newPosition;
}

//TypedObject semantics: hardcoded
TypeHandle Sound3d::_type_handle;

