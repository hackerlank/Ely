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

#include <nodePath.h>
#include <referenceCount.h>
#include <pointerTo.h>
#include "Component.h"
#include "Utilitiy.h"

#include <map>
#include <string>

/**
 * \brief Object instance identifier type (by default the name
 * of the NodePath component).
 */
typedef std::string ObjectId;

/**
 * \brief The object is the basic entity that can exist in the game world.
 *
 * Because Panda3d is used as game engine, a object should
 * "compose" the different aspects (visualization, sound, ai,
 * physics, animation, etc...), already existing in Panda3d.
 * An object can have only one component of a given family.
 * Each object has a NodePath embedded.
 *
 * \note The Object Model is based on the article "Game Object
 * Component System" by Chris Stoy in "Game Programming Gems 6" book.
 */
class Object: public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	Object(const ObjectId& objectId);

	/**
	 * \brief Destructor.
	 */
	virtual ~Object();

	/**
	 * \brief Return the node path.
	 * @return The node path.
	 */
	NodePath& getNodePath();
	/**
	 * \brief Set the node path.
	 * @param nodePath The node path to be set.
	 */
	void setNodePath(NodePath& nodePath);

	/**
	 * \brief Get the component of that given family.
	 * @param familyID The family of the component.
	 * @return The component, or NULL if no component of that
	 * family exists.
	 */
	Component* getComponent(
			const ComponentFamilyId& familyID);
	/**
	 * \brief Set a new component into this object.
	 *
	 * It will set the component in the object to the passed component,
	 * and if a component of that family already existed it'll be
	 * replaced by this new component (and its ownership released by
	 * the object).
	 * @param newComponent The new component to set.
	 * @return PT(NULL) if there wasn't a component of that family, otherwise
	 * the previous component.
	 */
	PT(Component) setComponent(
			Component* newComponent);
	/**
	 * \brief Clears the table of all components of this object.
	 */
	void clearComponents();

	/**
	 * \brief Get the id of this object.
	 * @return The id of this object.
	 */
	const ObjectId& getObjectId() const;
	/**
	 * \brief Set the id of this object.
	 * @param objectId The id of this object.
	 */
	void setObjectId(ObjectId& objectId);

	/**
	 * \brief NodePath conversion function.
	 */
	operator NodePath();

private:
	///NodePath associated with this object.
	NodePath mNodePath;
	///Unique identifier for this object.
	ObjectId mObjectId;
	///Table of all components indexed by component family type.
	typedef std::map<const ComponentFamilyId, PT(Component)> ComponentTable;
	ComponentTable mComponents;

};

#endif /* OBJECT_H_ */
