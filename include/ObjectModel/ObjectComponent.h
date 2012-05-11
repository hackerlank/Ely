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
 * \file /Ely/include/ObjectComponent.h
 *
 * \date 09/mag/2012 (16:09:17)
 * \author marco
 */

#ifndef OBJECTCOMPONENT_H_
#define OBJECTCOMPONENT_H_

#include <referenceCount.h>
#include <pointerTo.h>

#include <iostream>

/**
 * \brief A unique identifier for GameObjectComponents.
 */
typedef std::string ObjectComponentId;

class Object;

/**
 * \brief Base abstract class to provide a common interface for
 * all game object components.
 *
 * Components are organized into class hierarchies called families.
 * Each component belongs to a family and is derived from a base
 * family component.
 */
class ObjectComponent: public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	ObjectComponent();
	/**
	 * \brief Destructor (pure virtual).
	 */
	virtual ~ObjectComponent() = 0;

	/**
	 * \brief Get the id of this component.
	 * @return The id of this component.
	 */
	virtual const ObjectComponentId& componentID() const = 0;
	/**
	 * \brief Get the family id of this component.
	 * @return The family id of this component.
	 */
	virtual const ObjectComponentId& familyID() const = 0;

	/**
	 * \brief Updates the state of the component.
	 */
	virtual void update();

	/**
	 * \brief Get the owner game object.
	 * \return The owner game object.
	 */
	Object* getOwnerGameObject();
	/**
	 * \brief Set the owner game object.
	 * @param owner The owner game object.
	 */
	void setOwnerGameObject(Object* owner);

private:
	/// The game object this component is a member of
	Object* mOwnerGameObject;
};

#endif /* OBJECTCOMPONENT_H_ */
