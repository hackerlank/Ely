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
 * \file /Ely/src/PhysicsComponents/RigidBody.cpp
 *
 * \date 07/lug/2012 (15:58:35)
 * \author marco
 */

#include "PhysicsComponents/RigidBody.h"
#include "PhysicsComponents/RigidBodyTemplate.h"

RigidBody::RigidBody()
{
	// TODO Auto-generated constructor stub

}

RigidBody::RigidBody(RigidBodyTemplate* tmpl) :
		mTmpl(tmpl)
{
}

RigidBody::~RigidBody()
{
	// TODO Auto-generated destructor stub
}

const ComponentFamilyType RigidBody::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType RigidBody::componentType() const
{
	return mTmpl->componentType();
}

bool RigidBody::initialize()
{
	bool result = true;
	//get shape type

	//
	return result;
}

void RigidBody::onAddToObjectSetup()
{
}

BulletRigidBodyNode* RigidBody::rigidBodyNode()
{
	return mRigidBodyNode.p();
}

void RigidBody::switchType(BodyType bodyType)
{
}

//TypedObject semantics: hardcoded
TypeHandle RigidBody::_type_handle;


