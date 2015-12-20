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
 * \date 2012-06-27 
 * \author consultit
 */

#include "AudioComponents/Listener.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAudioManager.h"

namespace ely
{

Listener::Listener(SMARTPTR(ListenerTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameAudioManager::GetSingletonPtr(),
			"Listener::Listener: invalid GameAudioManager")

	mTmpl = tmpl;
	reset();
}

Listener::~Listener()
{
}

bool Listener::initialize()
{
	bool result = true;
	//get scene root
	mSceneRootId = ObjectId(mTmpl->parameter(std::string("scene_root")));
	//
	return result;
}

void Listener::onAddToObjectSetup()
{
	//set the root of the scene
	SMARTPTR(Object)sceneRoot =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mSceneRootId);
	if (sceneRoot)
	{
		mSceneRoot = sceneRoot->getNodePath();
	}
	else if (ObjectTemplateManager::GetSingleton().getCreatedObject(ObjectId("render")))
	{
		mSceneRoot =
				ObjectTemplateManager::GetSingleton().getCreatedObject(
						ObjectId("render"))->getNodePath();
	}
	else
	{
		mSceneRoot = NodePath();
	}
}

void Listener::onRemoveFromObjectCleanup()
{
	//
	reset();
}

void Listener::onAddToSceneSetup()
{
	if (mOwnerObject->isSteady())
	{
		//set 3d attribute (in this case static)
		doSet3dStaticAttributes();
	}
	else
	{
		// update listener position/velocity only for dynamic objects
		GameAudioManager::GetSingletonPtr()->addToAudioUpdate(this);
	}
}

void Listener::onRemoveFromSceneCleanup()
{
	//remove from audio update
	GameAudioManager::GetSingletonPtr()->removeFromAudioUpdate(this);
}

void Listener::set3dStaticAttributes()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	//do actual set
	doSet3dStaticAttributes();
}

void Listener::doSet3dStaticAttributes()
{
	NodePath ownerNodePath = mOwnerObject->getNodePath();
	mPosition = ownerNodePath.get_pos(mSceneRoot);
	LVector3f forward = ownerNodePath.get_relative_vector(mSceneRoot,
			LVector3f::forward());
	LVector3f up = ownerNodePath.get_relative_vector(mSceneRoot, LVector3f::up());
	GameAudioManager::GetSingletonPtr()->audioMgr()->audio_3d_set_listener_attributes(
			mPosition.get_x(), mPosition.get_y(), mPosition.get_z(), 0.0, 0.0,
			0.0, forward.get_x(), forward.get_y(), forward.get_z(), up.get_x(),
			up.get_y(), up.get_z());
}

void Listener::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	LPoint3f newPosition;
	LVector3f forward, up, deltaPos, velocity;
	NodePath ownerNodePath = mOwnerObject->getNodePath();

	//get the new position
#ifdef ELY_THREAD
	//note on threading: this should be an atomic operation
	CSMARTPTR(TransformState)ownerTransform = ownerNodePath.get_transform(
			mSceneRoot).p();
	newPosition = ownerTransform->get_pos();
	forward = LVector3f::forward() * ownerTransform->get_mat();
	up = LVector3f::up() * ownerTransform->get_mat();
#else
	newPosition = ownerNodePath.get_pos(mSceneRoot);
	forward = mSceneRoot.get_relative_vector(ownerNodePath,
			LVector3f::forward());
	up = mSceneRoot.get_relative_vector(ownerNodePath, LVector3f::up());
#endif

	//get the velocity (mPosition holds the previous position)
	deltaPos = (newPosition - mPosition);
	dt > 0.0 ? velocity = deltaPos / dt : velocity = LVector3f::zero();
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

///Template

ListenerTemplate::ListenerTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"ListenerTemplate::ListenerTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"ListenerTemplate::ListenerTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameAudioManager::GetSingletonPtr(),
			"ListenerTemplate::ListenerTemplate: invalid GameAudioManager")
	//
	setParametersDefaults();
}

ListenerTemplate::~ListenerTemplate()
{
	
}

ComponentType ListenerTemplate::componentType() const
{
	return ComponentType(Listener::get_class_type().get_name());
}

ComponentFamilyType ListenerTemplate::componentFamilyType() const
{
	return ComponentFamilyType("Audio");
}

SMARTPTR(Component)ListenerTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Listener) newListener = new Listener(this);
	newListener->setComponentId(compId);
	if (not newListener->initialize())
	{
		return NULL;
	}
	return newListener.p();
}

void ListenerTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("scene_root", "render"));
}

//TypedObject semantics: hardcoded
TypeHandle ListenerTemplate::_type_handle;

} // namespace ely
