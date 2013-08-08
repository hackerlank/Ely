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
 * \file /Ely/src/AudioComponents/Listener.cpp
 *
 * \date 27/giu/2012 (16:40:27)
 * \author consultit
 */

#include "AudioComponents/Listener.h"
#include "AudioComponents/ListenerTemplate.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAudioManager.h"
#include <throw_event.h>

namespace ely
{

Listener::Listener()
{
	// TODO Auto-generated constructor stub
}

Listener::Listener(SMARTPTR(ListenerTemplate)tmpl)
{
	CHECKEXISTENCE(GameAudioManager::GetSingletonPtr(),
			"Listener::Listener: invalid GameAudioManager")
	mTmpl = tmpl;
	mPosition = LPoint3(0.0, 0.0, 0.0);
}

Listener::~Listener()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//check if game audio manager exists
	if (GameAudioManager::GetSingletonPtr())
	{
		//remove from audio update
		throw_event(std::string("GameAudioManager::handleUpdateRequest"),
				EventParameter(this),
				EventParameter(GameAudioManager::REMOVEFROMUPDATE));
	}
}

ComponentFamilyType Listener::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Listener::componentType() const
{
	return mTmpl->componentType();
}

bool Listener::initialize()
{
	bool result = true;
	//
	return result;
}

void Listener::onAddToObjectSetup()
{
	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//set the root of the scene
	SMARTPTR(Object) sceneRoot =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				"render");
	if (sceneRoot)
	{
		mSceneRoot = sceneRoot->getNodePath();
	}

	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void Listener::onAddToSceneSetup()
{
	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	if (mOwnerObject->isSteady())
	{
		//set 3d attribute (in this case static)
		set3dStaticAttributes();
	}
	else
	{
		// update listener position/velocity only for dynamic objects
		throw_event(std::string("GameAudioManager::handleUpdateRequest"),
				EventParameter(this),
				EventParameter(GameAudioManager::ADDTOUPDATE));
	}
}

void Listener::set3dStaticAttributes()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	NodePath ownerNodePath = mOwnerObject->getNodePath();
	mPosition = ownerNodePath.get_pos(mSceneRoot);
	LVector3 forward = ownerNodePath.get_relative_vector(mSceneRoot,
			LVector3::forward());
	LVector3 up = ownerNodePath.get_relative_vector(mSceneRoot, LVector3::up());
	GameAudioManager::GetSingletonPtr()->audioMgr()->audio_3d_set_listener_attributes(
			mPosition.get_x(), mPosition.get_y(), mPosition.get_z(), 0.0, 0.0,
			0.0, forward.get_x(), forward.get_y(), forward.get_z(), up.get_x(),
			up.get_y(), up.get_z());
}

void Listener::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	LPoint3 newPosition;
	LVector3 forward, up, deltaPos, velocity;
	NodePath ownerNodePath = mOwnerObject->getNodePath();

	//get the new position
#ifdef ELY_THREAD
	//note on threading: this should be an atomic operation
	CSMARTPTR(TransformState)ownerTransform = ownerNodePath.get_transform(
			mSceneRoot).p();
	newPosition = ownerTransform->get_pos();
	forward = LVector3::forward() * ownerTransform->get_mat();
	up = LVector3::up() * ownerTransform->get_mat();
#else
	newPosition = ownerNodePath.get_pos(mSceneRoot);
	forward = mSceneRoot.get_relative_vector(ownerNodePath,
			LVector3::forward());
	up = mSceneRoot.get_relative_vector(ownerNodePath, LVector3::up());
#endif

	//get the velocity (mPosition holds the previous position)
	deltaPos = (newPosition - mPosition);
	dt > 0.0 ? velocity = deltaPos / dt : velocity = LVector3::zero();
	//update listener velocity and position
	//note on threading: this should be an atomic operation
	GameAudioManager::GetSingletonPtr()->audioMgr()->audio_3d_set_listener_attributes(
			newPosition.get_x(), newPosition.get_y(), newPosition.get_z(),
			velocity.get_x(), velocity.get_y(), velocity.get_z(),
			forward.get_x(), forward.get_y(), forward.get_z(), up.get_x(),
			up.get_y(), up.get_z());
	//update current position
	mPosition = newPosition;
}

//TypedObject semantics: hardcoded
TypeHandle Listener::_type_handle;

} // namespace ely
