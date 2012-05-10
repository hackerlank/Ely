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
 * \file /Ely/include/GameObject.h
 *
 * \date 07/mag/2012 (18:10:37)
 * \author marco
 */

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <nodePath.h>
#include <referenceCount.h>
#include <pointerTo.h>
#include "GameObjectComponent.h"
#include "Utilitiy.h"

#include <iostream>
#include <map>

/**
 * \brief A unique identifier for GameObjects (by default
 * the name of the NodePath component).
 */
typedef std::string GameObjectId;

/**
 * \brief The game object is the basic entity that can exist in the game world.
 *
 * Because Panda3d is used as game engine, a game object should
 * "compose" the different aspects (visualization, sound, ai,
 * physics, animation, etc...), already existing in Panda3d.
 * An game object can have only one component of a given family.
 * Each game object represents an instance of a PandaNode so the
 * first and more important component is a NodePath related to
 * this object. This NodePath component is treated differently
 * from the others: it is embedded.
 *
 * \note The Object Model is based on the article "Game Object
 * Component System" by Chris Stoy in "Game Programming Gems 6" book.
 */
class GameObject: public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	GameObject(NodePath* nodePath);

	/**
	 * \brief Destructor.
	 */
	virtual ~GameObject();

	/**
	 * \brief Return the node path.
	 * @return the node path.
	 */
	NodePath* getNodePath() const;
	/**
	 * \brief Set the node path.
	 * @param nodePath the node path to be set.
	 */
	void setNodePath(NodePath* nodePath);

	/**
	 * \brief Get the component of that given family.
	 * @param familyID the family of the component.
	 * @return The component.
	 */
	GameObjectComponent* getGameObjectComponent(
			const GameObjectComponentId& familyID);
	/**
	 * \brief Set a new component into this object.
	 * @param newComponent the new component.
	 * @return The component.
	 */
	GameObjectComponent* setGameObjectComponent(
			GameObjectComponent* newComponent);
	/**
	 * \brief Clears the table of all components of this object.
	 */
	void clearGameObjectComponents();

	/**
	 * \brief Get the id of this object.
	 * @return The id of this object.
	 */
	const GameObjectId& getGameObjectId() const;
	/**
	 * \brief Set the id of this object.
	 * @param gameObjectId The id of this object.
	 */
	void setGameObjectId(GameObjectId& gameObjectId);

private:
	///NodePath associated with this object.
	NodePath* mNodePath;
	///Unique identifier for this object.
	GameObjectId mGameObjectId;
	///Map of all components.
	typedef std::map<const GameObjectComponentId, PT(GameObjectComponent)> ComponentTable;
	ComponentTable mComponents;

};

#endif /* GAMEOBJECT_H_ */
