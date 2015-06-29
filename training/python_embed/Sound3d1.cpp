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
 * \author consultit
 */

#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAudioManager.h"
#include "Sound3d1.h"
#include "Sound3d1Py.h"

Sound3d1::Sound3d1(SMARTPTR(Sound3d1Template)tmpl)
{
	CHECK_EXISTENCE_DEBUG(ely::GameAudioManager::GetSingletonPtr(),
	"Sound3d::Sound3d: invalid GameAudioManager")

	mTmpl = tmpl;
	reset();
	//create the Sound3d1Py class instance inside Sound3d1 class instance
	createSound3d1PyInstance(mSound3d1Py, this);
}

Sound3d1::~Sound3d1()
{
	// delete the Sound3d1Py class instance inside Sound3d1 class instance
	deleteSound3d1PyInstance(mSound3d1Py);
}

ely::ComponentFamilyType Sound3d1::familyType() const
{
	return mTmpl->familyType();
}

ely::ComponentType Sound3d1::componentType() const
{
	return mTmpl->componentType();
}

bool Sound3d1::initialize()
{
	bool result = true;
	//get scene root
	mSceneRootId = ely::ObjectId(mTmpl->parameter(std::string("scene_root")));
	//sound files
	mSoundFileListParam = mTmpl->parameterList(std::string("sound_files"));
	//
	return result;
}

void Sound3d1::onAddToObjectSetup()
{
	//set sound files
	std::list<std::string>::iterator iter;
	for (iter = mSoundFileListParam.begin(); iter != mSoundFileListParam.end();
			++iter)
	{
		//any "sound_files" string is a "compound" one, i.e. could have the form:
		// "sound_name1@sound_file1:sound_name2@sound_file2:...:sound_nameN@sound_fileN"
		std::vector<std::string> nameFilePairs = ely::parseCompoundString(*iter,
				':');
		std::vector<std::string>::const_iterator iterPair;
		for (iterPair = nameFilePairs.begin(); iterPair != nameFilePairs.end();
				++iterPair)
		{
			//an empty sound_name@sound_file is ignored
			if (not iterPair->empty())
			{
				//get sound name and sound file name
				std::vector<std::string> nameFilePair =
						ely::parseCompoundString(*iterPair, '@');
				//check only if there is a pair
				if (nameFilePair.size() == 2)
				{
					//sound name == nameFilePair[0]
					//sound file name == nameFilePair[1]
					SMARTPTR(AudioSound)sound =
					ely::GameAudioManager::GetSingletonPtr()->audioMgr()->get_sound(
							nameFilePair[1], true).p();
					if (not sound.is_null())
					{
						//an empty ("") sound name is allowed
						mSounds[nameFilePair[0]] = sound;
					}
				}
			}
		}
	}

	//set the root of the scene
	SMARTPTR(ely::Object)sceneRoot =
	ely::ObjectTemplateManager::GetSingleton().getCreatedObject(
			mSceneRootId);
	if (sceneRoot)
	{
		mSceneRoot = sceneRoot->getNodePath();
	}
	else if (ely::ObjectTemplateManager::GetSingleton().getCreatedObject(
			ely::ObjectId("render")))
	{
		mSceneRoot =
				ely::ObjectTemplateManager::GetSingleton().getCreatedObject(
						ely::ObjectId("render"))->getNodePath();
	}
	else
	{
		mSceneRoot = NodePath();
	}

	//clear all no more needed "Param" variables
	mSoundFileListParam.clear();
}

void Sound3d1::onRemoveFromObjectCleanup()
{
	//stops every playing sounds
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		if (iter->second->status() == AudioSound::PLAYING)
		{
			iter->second->stop();
		}
	}
	//
	reset();
}

void Sound3d1::onAddToSceneSetup()
{
	if (mOwnerObject->isSteady() or mSounds.empty())
	{
		//set 3d attribute (in this case static)
		doSet3dStaticAttributes();
	}
	else
	{
		// update sounds' position/velocity only if sound table is not empty
		// and for dynamic objects
		ely::GameAudioManager::GetSingletonPtr()->addToAudioUpdate(this);
	}
}

void Sound3d1::onRemoveFromSceneCleanup()
{
	//remove from audio update
	ely::GameAudioManager::GetSingletonPtr()->removeFromAudioUpdate(this);
}

Sound3d1::Result Sound3d1::addSound(const std::string& soundName,
		const std::string& fileName)
{
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)
	}

	int result = Result::ERROR;
	//lock (guard) the mutex
	HOLD_REMUTEX(ely::GameAudioManager::GetSingletonPtr()->getMutex())
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//get the sound from fileName
		SMARTPTR(AudioSound)sound =
		ely::GameAudioManager::GetSingletonPtr()->audioMgr()->get_sound(fileName,
				true).p();
		if (sound)
		{
			//add sound with soundName
			mSounds[soundName] = sound;
			result = Result::OK;
			// try to add this component to updating (if not present)
			// only if object is dynamic
			if (not mOwnerObject->isSteady())
			{
				//add to audio update
				ely::GameAudioManager::GetSingletonPtr()->addToAudioUpdate(
						this);
			}
		}
	}
	//
	return result;
}

Sound3d1::Result Sound3d1::removeSound(const std::string& soundName)
{
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)
	}

	int result = Result::ERROR;
	//lock (guard) the mutex
	HOLD_REMUTEX(ely::GameAudioManager::GetSingletonPtr()->getMutex())
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//make mSounds modifications
		size_t removed = mSounds.erase(soundName);
		if (removed == 1)
		{
			// sound is removed
			result = Result::OK;
			// try to remove this component from update if sound table is empty
			if (mSounds.empty())
			{
				//remove from audio update
				ely::GameAudioManager::GetSingletonPtr()->removeFromAudioUpdate(
						this);
			}
		}
	}
	//
	return result;
}

void Sound3d1::setMinDistance(float dist)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	mMinDist = dist;
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second->set_3d_min_distance(mMinDist);
	}
}

void Sound3d1::setMaxDistance(float dist)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	mMaxDist = dist;
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second->set_3d_max_distance(mMaxDist);
	}
}

void Sound3d1::set3dStaticAttributes()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	//do actual set
	doSet3dStaticAttributes();
}

void Sound3d1::doSet3dStaticAttributes()
{
	mPosition = mOwnerObject->getNodePath().get_pos(mSceneRoot);
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second->set_3d_attributes(mPosition.get_x(), mPosition.get_y(),
				mPosition.get_z(), 0.0, 0.0, 0.0);
	}
}

SMARTPTR(AudioSound)Sound3d1::getSound(const std::string& soundName)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,NULL)

	SoundTable::iterator iter = mSounds.find(soundName);
	if (iter == mSounds.end())
	{
		return NULL;
	}
	return iter->second;
}

SMARTPTR(AudioSound)Sound3d1::getSound(int index)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,NULL)

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
	return iter->second;
}

void Sound3d1::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//get the new position
	//note on threading: this should be an atomic operation
	LPoint3f newPosition = mOwnerObject->getNodePath().get_pos(mSceneRoot);
	//get the velocity (mPosition holds the previous position)
	LVector3f deltaPos = (newPosition - mPosition);
	LVector3f velocity;
	dt > 0.0 ? velocity = deltaPos / dt : velocity = LVector3f::zero();
	//update sounds' velocity and position
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		//note on threading: this should be an atomic operation
		iter->second->set_3d_attributes(newPosition.get_x(),
				newPosition.get_y(), newPosition.get_z(), velocity.get_x(),
				velocity.get_y(), velocity.get_z());
	}
	//update current position
	mPosition = newPosition;
}

//TypedObject semantics: hardcoded
TypeHandle Sound3d1::_type_handle;

///Template

Sound3d1Template::Sound3d1Template(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"Sound3dTemplate::Sound3dTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"Sound3dTemplate::Sound3dTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(ely::GameAudioManager::GetSingletonPtr(),
			"Sound3dTemplate::Sound3dTemplate: invalid GameAudioManager")
	//
	setParametersDefaults();
}

Sound3d1Template::~Sound3d1Template()
{

}

ely::ComponentType Sound3d1Template::componentType() const
{
	return ely::ComponentType(Sound3d1::get_class_type().get_name());
}

ely::ComponentFamilyType Sound3d1Template::familyType() const
{
	return ely::ComponentFamilyType("Audio");
}

SMARTPTR(ely::Component)Sound3d1Template::makeComponent(const ely::ComponentId& compId)
{
	SMARTPTR(Sound3d1) newSound3d = new Sound3d1(this);
	newSound3d->setComponentId(compId);
	if (not newSound3d->initialize())
	{
		return NULL;
	}
	return newSound3d.p();
}

void Sound3d1Template::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ely::ParameterNameValue("scene_root", "render"));
}

//TypedObject semantics: hardcoded
TypeHandle Sound3d1Template::_type_handle;
