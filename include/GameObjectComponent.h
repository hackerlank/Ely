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
 * \file /Ely/include/GameObjectComponent.h
 *
 * \date 09/mag/2012 (16:09:17)
 * \author marco
 */

#ifndef GAMEOBJECTCOMPONENT_H_
#define GAMEOBJECTCOMPONENT_H_

#include <referenceCount.h>
#include <pointerTo.h>

#include <iostream>

/**
 * \brief A unique identifier for GameObjectComponents.
 */
typedef std::string GameObjectComponentId;

class GameObject;

/**
 * \brief Base abstract class to provide a common interface for
 * all game object components.
 *
 * Components are organized into class hierarchies called families.
 * Each component belongs to a family and is derived from a base
 * family component.
 */
class GameObjectComponent: public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	GameObjectComponent();
	/**
	 * \brief Destructor (pure virtual).
	 */
	virtual ~GameObjectComponent() = 0;

	/**
	 * \brief Get the id of this component.
	 * @return The id of this component.
	 */
	virtual const GameObjectComponentId& componentID() const = 0;
	/**
	 * \brief Get the family id of this component.
	 * @return The family id of this component.
	 */
	virtual const GameObjectComponentId& familyID() const = 0;

	/**
	 * \brief Updates the state of the component.
	 */
	virtual void update();

	/**
	 * \brief Get the owner game object.
	 * \return The owner game object.
	 */
	GameObject* getOwnerGameObject();
	/**
	 * \brief Set the owner game object.
	 * @param owner The owner game object.
	 */
	void setOwnerGameObject(GameObject* owner);

private:
	/// The game object this component is a member of
	GameObject* mOwnerGameObject;
};

#endif /* GAMEOBJECTCOMPONENT_H_ */
