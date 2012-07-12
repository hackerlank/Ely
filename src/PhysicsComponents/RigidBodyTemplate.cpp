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
 * \file /Ely/src/PhysicsComponents/RigidBodyTemplate.cpp
 *
 * \date 07/lug/2012 (15:56:20)
 * \author marco
 */

#include "PhysicsComponents/RigidBodyTemplate.h"

RigidBodyTemplate::RigidBodyTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"RigidBodyTemplate::RigidBodyTemplate: invalid PandaFramework or WindowFramework");
	}
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
	if (not GamePhysicsManager::GetSingletonPtr())
	{
		throw GameException(
				"RigidBodyTemplate::RigidBodyTemplate: invalid GamePhysicsManager");
	}
	resetParameters();
}

RigidBodyTemplate::~RigidBodyTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType RigidBodyTemplate::componentType() const
{
	return ComponentType("RigidBody");
}

const ComponentFamilyType RigidBodyTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

Component* RigidBodyTemplate::makeComponent(const ComponentId& compId)
{
	RigidBody* newRigidBody = new RigidBody(this);
	newRigidBody->componentId() = compId;
	if (not newRigidBody->initialize())
	{
		return NULL;
	}
	return newRigidBody;
}

void RigidBodyTemplate::resetParameters()
{
	//set component parameters to their default values
	mBodyType = std::string("dynamic");
	mBodyMass = std::string("1.0");
	mShapeType = std::string("sphere");
	mCollideMask = std::string("all_on");
	mCcdMotionThreshold = std::string("");
	mCcdSweptSphereRadius = std::string("");
}

void RigidBodyTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTable::iterator iter;
	CASEITER(parameterTable, iter, "body_type", mBodyType)
	CASEITER(parameterTable, iter, "body_mass", mBodyMass)
	CASEITER(parameterTable, iter, "shape_type", mShapeType)
	CASEITER(parameterTable, iter, "collide_mask", mCollideMask)
	CASEITER(parameterTable, iter, "ccd_motion_threshold", mCcdMotionThreshold)
	CASEITER(parameterTable, iter, "ccd_swept_sphere_radius", mCcdSweptSphereRadius)
}

std::string& RigidBodyTemplate::parameter(const std::string& paramName)
{
	std::string* strPtr = &mUnknown;
	CASE(paramName, strPtr, "body_type", mBodyType)
	CASE(paramName, strPtr, "body_mass", mBodyMass)
	CASE(paramName, strPtr, "shape_type", mShapeType)
	CASE(paramName, strPtr, "collide_mask", mCollideMask)
	CASE(paramName, strPtr, "ccd_motion_threshold", mCcdMotionThreshold)
	CASE(paramName, strPtr, "ccd_swept_sphere_radius", mCcdSweptSphereRadius)
	//
	return *strPtr;
}

GamePhysicsManager* RigidBodyTemplate::gamePhysicsMgr()
{
	return GamePhysicsManager::GetSingletonPtr();
}

PandaFramework*& RigidBodyTemplate::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& RigidBodyTemplate::windowFramework()
{
	return mWindowFramework;
}

//TypedObject semantics: hardcoded
TypeHandle RigidBodyTemplate::_type_handle;

