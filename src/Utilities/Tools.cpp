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
 * \file /Ely/src/Utilities/Tools.cpp
 *
 * \date 19/mag/2012 (12:39:59)
 * \author marco
 */

#include "Utilities/Tools.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplate.h"
#include "Utilities/ComponentSuite.h"

void initTypedObjects()
{
	Component::init_type();
	ComponentTemplate::init_type();
	Object::init_type();
	ObjectTemplate::init_type();
	//
	Listener::init_type();
	ListenerTemplate::init_type();
	Sound3d::init_type();
	Sound3dTemplate::init_type();
	Activity::init_type();
	ActivityTemplate::init_type();
	Chaser::init_type();
	ChaserTemplate::init_type();
	Driver::init_type();
	DriverTemplate::init_type();
	CharacterController::init_type();
	CharacterControllerTemplate::init_type();
	RigidBody::init_type();
	RigidBodyTemplate::init_type();
	InstanceOf::init_type();
	InstanceOfTemplate::init_type();
	Model::init_type();
	ModelTemplate::init_type();
	NodePathWrapper::init_type();
	NodePathWrapperTemplate::init_type();
	Terrain::init_type();
	TerrainTemplate::init_type();
	GeoMipTerrainRef::init_type();
}
