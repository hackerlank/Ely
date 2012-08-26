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
 * \file /Ely/src/SceneComponents/Terrain.cpp
 *
 * \date 15/ago/2012 (10:00:08)
 * \author marco
 */

#include "SceneComponents/Terrain.h"
#include "SceneComponents/TerrainTemplate.h"

Terrain::Terrain()
{
	// TODO Auto-generated constructor stub
}

Terrain::Terrain(TerrainTemplate* tmpl)
{
	mTmpl = tmpl;
}

Terrain::~Terrain()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath.remove_node();
}

const ComponentFamilyType Terrain::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Terrain::componentType() const
{
	return mTmpl->componentType();
}

bool Terrain::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void Terrain::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set the node path of the object to the
	//node path of this model
	mOwnerObject->setNodePath(mNodePath);
	//register event callbacks if any
	registerEventCallbacks();
}

NodePath Terrain::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void Terrain::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

//TypedObject semantics: hardcoded
TypeHandle Terrain::_type_handle;

