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
 * \file /Ely/src/PhysicsComponents/Vehicle.cpp
 *
 * \date 15/set/2013 (10:40:11)
 * \author consultit
 */

#include "PhysicsComponents/Vehicle.h"
#include "PhysicsComponents/VehicleTemplate.h"
#include <throw_event.h>

namespace ely
{

Vehicle::Vehicle()
{
	// TODO Auto-generated constructor stub
}

Vehicle::Vehicle(SMARTPTR(VehicleTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"Vehicle::Vehicle: invalid GamePhysicsManager")

	mTmpl = tmpl;
	reset();
}

Vehicle::~Vehicle()
{
	// TODO Auto-generated destructor stub
}

ComponentFamilyType Vehicle::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Vehicle::componentType() const
{
	return mTmpl->componentType();
}

bool Vehicle::initialize()
{
	bool result = true;
	//throw events setting
	mThrowEvents = (
			mTmpl->parameter(std::string("throw_events"))
					== std::string("true") ? true : false);

	//
	return result;
}

void Vehicle::onAddToObjectSetup()
{
	//create the chassis
	//set a ParameterTable for each component
	ParameterTableMap compTmplParams;
	compTmplParams["RigidBody"].insert(
		ParameterTable::value_type("", ""));


	//create a node path for the vehicle
	mNodePath = NodePath(mVehicle);

	//attach it to Bullet World
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			mVehicle);

	//set this character controller node path as the object's one
	mOwnerObject->setNodePath(mNodePath);
}

void Vehicle::onRemoveFromObjectCleanup()
{
	NodePath oldObjectNodePath;
	//set the object node path to the first child of rigid body's one (if any)
	if(mNodePath.get_num_children() > 0)
	{
		oldObjectNodePath = mNodePath.get_child(0);
		//detach the object node path from the rigid body's one
		oldObjectNodePath.detach_node();
	}
	else
	{
		oldObjectNodePath = NodePath();
	}
	//set the object node path to the old one
	mOwnerObject->setNodePath(oldObjectNodePath);

	//check if game physics manager exists
	GamePhysicsManager* physicsMgrPtr = GamePhysicsManager::GetSingletonPtr();
	if (physicsMgrPtr)
	{
		//remove character controller from the physics world
		physicsMgrPtr->bulletWorld()->remove(DCAST(TypedObject, mVehicle));
	}

	//Remove node path
	mNodePath.remove_node();
	//
	reset();
}

void Vehicle::onAddToSceneSetup()
{
	//Add to the physics manager update
	GamePhysicsManager::GetSingletonPtr()->addToPhysicsUpdate(this);
}

void Vehicle::onRemoveFromSceneCleanup()
{
	//remove from the physics manager update
	GamePhysicsManager::GetSingletonPtr()->removeFromPhysicsUpdate(this);
}


void Vehicle::update(void* data)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	LVector3 speed(0, 0, 0);
	float omega = 0.0;

	//handle Vehicle start-stop events
	if (mVehicle->get_current_speed_km_hour() == 0.0)
	{
		//throw mOnStart event (if enabled)
		if (mThrowEvents and (not mOnStartSent))
		{
			throw_event(std::string("OnStart"), EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mOnStartSent = true;
			mOnStopSent = false;
		}
	}
	else
	{
		//throw OnStop event (if enabled)
		if (mThrowEvents and (not mOnStopSent))
		{
			throw_event(std::string("OnStop"), EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
			mOnStopSent = true;
			mOnStartSent = false;
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle Vehicle::_type_handle;

} /* namespace ely */
