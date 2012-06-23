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
		mTmpl(tmpl), mMinDist(1.0), mMaxDist(1000000000.0)
{
	mSounds.clear();
	mUpdateData = NULL;
	mUpdateTask = NULL;
}

Sound3d::~Sound3d()
{
	if (mUpdateTask)
	{
		mTmpl->pandaFramework()->get_task_mgr().remove(mUpdateTask);
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
	bool result = true;
	//set sound files
	std::list<std::string>::iterator it;
	for (it = mTmpl->soundFiles().begin(); it != mTmpl->soundFiles().end();
			++it)
	{
		addSound(*it);
	}
	//
	return result;
}

void Sound3d::onAddToObjectSetup()
{
	// update sounds' position/velocity only for dynamic objects
	if (not mOwnerObject->isStatic())
	{
		//create the task for updating the active sounds
		mUpdateData = new TaskInterface<Sound3d>::TaskData(this,
				&Sound3d::update);
		mUpdateTask = new GenericAsyncTask("Sound3d::update",
				&TaskInterface<Sound3d>::taskFunction,
				reinterpret_cast<void*>(mUpdateData.p()));
		//Add the task for updating the controlled object
		mTmpl->pandaFramework()->get_task_mgr().add(mUpdateTask);
	}
	//set the root of the scene
	mSceneRoot = mTmpl->windowFramework()->get_render();
	//set 3d attribute (in this case static)
	set3dStaticAttributes();
}

bool Sound3d::addSound(const std::string& fileName)
{
	bool result = false;
	PT(AudioSound) sound = mTmpl->audioManager()->get_sound(fileName, true);
	if (sound)
	{
		sounds()[fileName] = sound;
		result = true;
	}
	return result;
}

bool Sound3d::removeSound(const std::string& soundName)
{
	bool result = false;
	size_t removed = sounds().erase(soundName);
	if (removed == 1)
	{
		result = true;
	}
	return result;
}

void Sound3d::setMinDistance(float dist)
{
	mMinDist = dist;
	SoundTable::iterator iter;
	for (iter = sounds().begin(); iter != sounds().end(); ++iter)
	{
		iter->second->set_3d_min_distance(mMinDist);
	}
}

float Sound3d::getMinDistance()
{
	return mMinDist;
}

void Sound3d::setMaxDistance(float dist)
{
	mMaxDist = dist;
	SoundTable::iterator iter;
	for (iter = sounds().begin(); iter != sounds().end(); ++iter)
	{
		iter->second->set_3d_max_distance(mMaxDist);
	}
}

float Sound3d::getMaxDistance()
{
	return mMaxDist;
}

void Sound3d::set3dStaticAttributes()
{
	LPoint3 position = mOwnerObject->nodePath().get_pos(mSceneRoot);
	SoundTable::iterator iter;
	for (iter = sounds().begin(); iter != sounds().end(); ++iter)
	{
		iter->second->set_3d_attributes(position.get_x(), position.get_y(),
				position.get_z(), 0.0, 0.0, 0.0);
	}
}

Sound3d::SoundTable& Sound3d::sounds()
{
	return mSounds;
}

AsyncTask::DoneStatus Sound3d::update(GenericAsyncTask* task)
{
	float dt = task->get_dt();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//get the new position
	LPoint3 newPosition = mOwnerObject->nodePath().get_pos(mSceneRoot);
	//get the velocity (mPosition holds the previous position)
	LVector3 velocity = (newPosition - mPosition) / dt;
	//update sounds' velocity and position
	SoundTable::iterator iter;
	for (iter = sounds().begin(); iter != sounds().end(); ++iter)
	{
		iter->second->set_3d_attributes(newPosition.get_x(), newPosition.get_y(),
				newPosition.get_z(), velocity.get_x(), velocity.get_y(),
				velocity.get_z());
	}
	//update actual position
	mPosition = newPosition;
	//
	return AsyncTask::DS_cont;
}

//TypedObject semantics: hardcoded
TypeHandle Sound3d::_type_handle;

