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
 * \author consultit
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include <lvector3.h>
#include <lpoint3.h>
#include <nodePath.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

namespace ely
{
class ListenerTemplate;

/**
 * \brief Component manipulating the listener for 3d sounds.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *scene_root* 			|single| *render* | -
 *
 * \note parts inside [] are optional.\n
 */
class Listener: public Component
{
protected:
	friend class ListenerTemplate;

	Listener(SMARTPTR(ListenerTemplate)tmpl);
	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	virtual ~Listener();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Set position/velocity for static objects.
	 *
	 * The position is the that of the owner object wrt scene root
	 * (i.e. render). Velocity is zero.
	 */
	void set3dStaticAttributes();

	/**
	 * \brief Updates position and velocity of this component.
	 *
	 * Will be called automatically by a audio manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

private:
	///The root of the scene (e.g. render)
	NodePath mSceneRoot;
	ObjectId mSceneRootId;
	///Sounds' characteristics.
	LPoint3f mPosition;

	/**
	 * \brief Actually sets position/velocity for static objects.
	 */
	void doSet3dStaticAttributes();

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

///inline definitions

inline void Listener::reset()
{
	//
	mSceneRoot = NodePath();
	mSceneRootId = ObjectId();
	mPosition = LPoint3f::zero();
}

///Template

class ListenerTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	ListenerTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ListenerTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType familyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "ListenerTemplate",
				ComponentTemplate::get_class_type());
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
}  // namespace ely

#endif /* LISTENER_H_ */
