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
 * \author marco
 */

#include "AudioComponents/Listener.h"
#include "AudioComponents/ListenerTemplate.h"

Listener::Listener()
{
	// TODO Auto-generated constructor stub

}

Listener::Listener(ListenerTemplate* tmpl) :
		mTmpl(tmpl)
{
	mPosition = LPoint3(0.0, 0.0, 0.0);
}

Listener::~Listener()
{
	// check if game audio manager exists
	if (mTmpl->gameAudioMgr())
	{
		mTmpl->gameAudioMgr()->removeFromAudioUpdate(this);
	}
}

const ComponentFamilyType Listener::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Listener::componentType() const
{
	return mTmpl->componentType();
}

bool Listener::initialize()
{
	bool result = true;
	return result;
}

void Listener::onAddToObjectSetup()
{
	// update listener position/velocity only for dynamic objects
	if (mTmpl->gameAudioMgr() and (not mOwnerObject->isStatic()))
	{
		mTmpl->gameAudioMgr()->addToAudioUpdate(this);
	}
	//set the root of the scene
	mSceneRoot = mTmpl->windowFramework()->get_render();
}

void Listener::onAddToSceneSetup()
{
	if (mTmpl->gameAudioMgr() and mOwnerObject->isStatic())
	{
		//set 3d attribute (in this case static)
		set3dStaticAttributes();
	}
}

void Listener::set3dStaticAttributes()
{
	mPosition = mOwnerObject->nodePath().get_pos(mSceneRoot);
	LVector3 forward = mOwnerObject->nodePath().get_relative_vector(mSceneRoot,
			LVector3::forward());
	LVector3 up = mOwnerObject->nodePath().get_relative_vector(mSceneRoot,
			LVector3::up());
	mTmpl->gameAudioMgr()->audioMgr()->audio_3d_set_listener_attributes(
			mPosition.get_x(), mPosition.get_y(), mPosition.get_z(), 0.0, 0.0,
			0.0, forward.get_x(), forward.get_y(), forward.get_z(), up.get_x(),
			up.get_y(), up.get_z());
}

void Listener::update(void* data)
{
	float dt = *(reinterpret_cast<float*>(data));

	//get the new position
	LPoint3 newPosition = mOwnerObject->nodePath().get_pos(mSceneRoot);
	LVector3 forward = mOwnerObject->nodePath().get_relative_vector(mSceneRoot,
			LVector3::forward());
	LVector3 up = mOwnerObject->nodePath().get_relative_vector(mSceneRoot,
			LVector3::up());
	//get the velocity (mPosition holds the previous position)
	LVector3 velocity = (newPosition - mPosition) / dt;
	//update listener velocity and position
	mTmpl->gameAudioMgr()->audioMgr()->audio_3d_set_listener_attributes(
			newPosition.get_x(), newPosition.get_y(), newPosition.get_z(),
			velocity.get_x(), velocity.get_y(), velocity.get_z(),
			forward.get_x(), forward.get_y(), forward.get_z(), up.get_x(),
			up.get_y(), up.get_z());
	//update actual position
	mPosition = newPosition;
}

//TypedObject semantics: hardcoded
TypeHandle Listener::_type_handle;

