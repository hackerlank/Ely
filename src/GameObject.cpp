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
 * \file /Ely/src/GameObject.cpp
 *
 * \date 07/mag/2012 (18:10:37)
 * \author marco
 */

#include "GameObject.h"

GameObject::GameObject(const NodePath& nodePath)
{
	if ((not nodePath) or (nodePath.is_empty()))
	{
		throw GameException("NULL or Empty NodePath");
	}
	mNodePath = nodePath;
	mGameObjectId = static_cast<GameObjectId>(mNodePath.get_name());
}

GameObject::~GameObject()
{
	mNodePath.remove_node();
}

const GameObjectId& GameObject::getGameObjectId() const
{
	return mGameObjectId;
}

void GameObject::setGameObjectId(GameObjectId& gameObjectId)
{
	mGameObjectId = gameObjectId;
}

void GameObject::clearGameObjectComponents()
{
	mComponents.clear();
}

NodePath& GameObject::getNodePath()
{
	return mNodePath;
}

void GameObject::setNodePath(NodePath& nodePath)
{
	mNodePath = nodePath;
}

GameObject::operator NodePath()
{
	return mNodePath;
}

