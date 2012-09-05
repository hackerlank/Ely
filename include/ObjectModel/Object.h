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
#include <reMutex.h>
#include <reMutexHolder.h>
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
 * Each object has a NodePath embedded.\n
 * Object can be initialized after it is added to the scene, by
 * an initialization function, whose name is <OBJECTID>_initialization
 * loaded at runtime from a dynamic linked library (referenced by the
 * macro INITIALIZATIONS_SO).
 *
 * XML Param(s):
 * - "parent"  				|single|no default
 * - "is_static"  			|single|"false"
 * - "pos_x"  				|single|"0.0"
 * - "pos_y"  				|single|"0.0"
 * - "pos_z"  				|single|"0.0"
 * - "rot_h"  				|single|"0.0"
 * - "rot_p"  				|single|"0.0"
 * - "rot_r"  				|single|"0.0"
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
	Object(const ObjectId& objectId, SMARTPTR(ObjectTemplate)tmpl);

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
	SMARTPTR(Component) getComponent(const ComponentFamilyType& familyID);

	/**
	 * \brief Sets a new component into this object.
	 *
	 * It will add the component in the object to the passed component,
	 * and if a component of that family already existed it'll be
	 * replaced by this new component (and its ownership released by
	 * the object).
	 * @param newComponent The new component to add.
	 * @return SMARTPTR(NULL) if there wasn't a component of that family, otherwise
	 * the previous component.
	 */
	SMARTPTR(Component) addComponent(SMARTPTR(Component) newComponent);

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
	const ObjectId& objectId() const;

	/**
	 * \brief Gets/sets the node path of this object.
	 * @return The node path of this object.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \brief NodePath conversion function.
	 */
	operator NodePath();

	/**
	 * \brief Gets a reference to the object template.
	 * @return The a reference to the object template.
	 */
	SMARTPTR(ObjectTemplate) const objectTmpl() const;

	/**
	 * \brief Gets/sets a reference to the static flag.
	 *
	 * This flag represents if this object doesn't move in the world.
	 * Various components can set or get this value to implement
	 * some optimization. By default false (i.e. object is dynamic).
	 */
	///@{
	bool isStatic();
	void setStatic(bool value);
	///@}

	/**
	 * \brief Initialization function type.
	 */
	typedef void (*PINITILIZATION)(Object*);

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex
	 */
	ReMutex& getMutex();

private:
	///The template used to construct this component.
	SMARTPTR(ObjectTemplate) const mTmpl;
	///The NodePath associated to this object.
	NodePath mNodePath;
	///Unique identifier for this object.
	ObjectId mObjectId;
	///@{
	///Table of all components indexed by component family type.
	typedef std::map<const ComponentFamilyType, SMARTPTR(Component)> ComponentTable;
	ComponentTable mComponents;
	///@}
	///Static flag: if this object doesn't move in the world.
	///Various components can set or get this value to implement
	///some optimization.
	bool mIsStatic;

	///Handle of the library of initialization functions.
	LIB_HANDLE mInitializationLib;
	///Helper flag.
	bool mInitializationsLoaded;

	/**
	 * \name Helper functions to load/unload initialization functions.
	 */
	///@{
	void loadInitializationFunctions();
	void unloadInitializationFunctions();
	///@}

	///The (reentrant) mutex associated with this object.
	ReMutex mMutex;

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
