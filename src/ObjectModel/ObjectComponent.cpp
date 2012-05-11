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
 * \file /Ely/src/ObjectComponent.cpp
 *
 * \date 09/mag/2012 (16:09:17)
 * \author marco
 */

#include "ObjectModel/ObjectComponent.h"

ObjectComponent::ObjectComponent()
{
	// TODO Auto-generated constructor stub

}

void ObjectComponent::update()
{
}

Object* ObjectComponent::getOwnerGameObject()
{
	return mOwnerGameObject;
}

void ObjectComponent::setOwnerGameObject(Object* owner)
{
	mOwnerGameObject = owner;
}
