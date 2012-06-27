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
 * \file /Ely/include/AudioComponents/Listener.h
 *
 * \date 27/giu/2012 (16:40:27)
 * \author marco
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include <audioManager.h>
#include <audioSound.h>
#include <pointerTo.h>
#include <lvector3.h>
#include <lpoint3.h>
#include <nodePath.h>
#include <genericAsyncTask.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

class ListenerTemplate;

/**
 * \brief Component manipulating the listener for 3d sounds.
 */
class Listener: public Component
{
public:
	Listener();
	Listener(ListenerTemplate* tmpl);
	virtual ~Listener();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \brief Set position/velocity for static objects.
	 *
	 * The position is the that of the owner object wrt scene root
	 * (i.e. render). Velocity is zero.
	 */
	void set3dStaticAttributes();

	/**
	 * \brief Updates position and velocity of this components.
	 *
	 * Will be called automatically in a task.
	 * @param task The task.
	 * @return The "done" status.
	 */
	virtual AsyncTask::DoneStatus update(GenericAsyncTask* task);

private:
	///The template used to construct this component.
	ListenerTemplate* mTmpl;
	///The root of the scene (e.g. render)
	NodePath mSceneRoot;
	///Sounds' characteristics.
	LPoint3 mPosition;

	///@{
	///A task data for update.
	PT(TaskInterface<Listener>::TaskData) mUpdateData;
	PT(AsyncTask) mUpdateTask;
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Listener", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

#endif /* LISTENER_H_ */
