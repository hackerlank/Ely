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
 * \file /Ely/include/ObjectModel/Object.h
 *
 * \date 07/mag/2012 (18:10:37)
 * \author marco
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <map>
#include <string>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <nodePath.h>
#include <typedWritableReferenceCount.h>
#include <pointerTo.h>

#include "ObjectTemplate.h"
#include "Component.h"
#include "Utilities/Tools.h"

/**
 * \brief Object instance identifier type (by default the name
 * of the NodePath component).
 */
typedef std::string ObjectId;

/**
 * \brief The object is the basic entity that can exist in the game world.
 *
 * Because Panda3d is used as game engine, a object should
 * "compose" the different aspects (graphics, sound, ai,
 * physics, animation, etc...), already existing in Panda3d.
 * An object can have only one component of a given family.
 * Each object has a NodePath embedded.
 *
 * XML Param(s):
 * \li \c "enabled"  |single|required|"true"
 *
 * \note The Object Model is based on the article "Game Object
 * Component System" by Chris Stoy in "Game Programming Gems 6" book.
 */
class Object: public TypedWritableReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	Object(const ObjectId& objectId, ObjectTemplate* tmpl);

	/**
	 * \brief Destructor.
	 */
	virtual ~Object();

	/**
	 * \brief Gets the component of that given family.
	 * @param familyID The family of the component.
	 * @return The component, or NULL if no component of that
	 * family exists.
	 */
	Component* getComponent(const ComponentFamilyType& familyID);

	/**
	 * \brief Sets a new component into this object.
	 *
	 * It will add the component in the object to the passed component,
	 * and if a component of that family already existed it'll be
	 * replaced by this new component (and its ownership released by
	 * the object).
	 * @param newComponent The new component to add.
	 * @return PT(NULL) if there wasn't a component of that family, otherwise
	 * the previous component.
	 */
	PT(Component) addComponent(Component* newComponent);

	/**
	 * \brief Clears the table of all components of this object.
	 */
	void clearComponents();

	/**
	 * \brief Returns the number of components.
	 * @return The number of components.
	 */
	unsigned int numComponents();

	/**
	 * \brief On addition to scene setup.
	 *
	 * Gives an object the ability to perform the
	 * addition to scene setup.
	 */
	void sceneSetup();

	/**
	 * \brief Gets a reference to the id of this object.
	 * @return The id of this object.
	 */
	ObjectId& objectId();

	/**
	 * \brief Gets a reference to the node path of this object.
	 * @return The node path of this object.
	 */
	NodePath& nodePath();
	/**
	 * \brief NodePath conversion function.
	 */
	operator NodePath();

	/**
	 * \brief Gets a reference to the object template.
	 * @return The a reference to the object template.
	 */
	ObjectTemplate* objectTmpl();

	/**
	 * \brief Gets a reference to the static flag.
	 *
	 * Thi flag represents if this object doesn't move in the world.
	 * Various components can set or get this value to implement
	 * some optimization. By default false (i.e. object is dynamic).
	 */
	bool& isStatic();

private:
	///The template used to construct this component.
	ObjectTemplate* mTmpl;
	///The NodePath associated to this object.
	NodePath mNodePath;
	///Unique identifier for this object.
	ObjectId mObjectId;
	///@{
	///Table of all components indexed by component family type.
	typedef std::map<const ComponentFamilyType, PT(Component)> ComponentTable;
	ComponentTable mComponents;
	///@}
	///Static flag: if this object doesn't move in the world.
	///Various components can set or get this value to implement
	///some optimization.
	bool mIsStatic;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedObject::init_type();
		register_type(_type_handle, "Object", TypedObject::get_class_type());
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

#endif /* OBJECT_H_ */
