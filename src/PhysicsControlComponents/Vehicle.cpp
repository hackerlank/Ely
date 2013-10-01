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
 * \file /Ely/src/PhysicsControlComponents/Vehicle.cpp
 *
 * \date 15/set/2013 (10:40:11)
 * \author consultit
 */

#include "PhysicsControlComponents/Vehicle.h"
#include "PhysicsControlComponents/VehicleTemplate.h"
#include "PhysicsComponents/RigidBody.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GamePhysicsManager.h"
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
	//up axis
	std::string upAxis = mTmpl->parameter(std::string("up_axis"));
	if (upAxis == std::string("x"))
	{
		mUpAxis = X_up;
	}
	else if (upAxis == std::string("y"))
	{
		mUpAxis = Y_up;
	}
	else
	{
		mUpAxis = Z_up;
	}
	//wheels
	mWheelTmpl = mTmpl->parameter(std::string("wheel_object_template"));
	//wheel number
	mWheelNumber = strtol(
			mTmpl->parameter(std::string("wheels_number")).c_str(),
			NULL, 0);
	if (mWheelNumber < 0)
	{
		mWheelNumber = 0;
	}
	//
	std::list<std::string> paramList;
	std::list<std::string>::const_iterator paramListIter;
	//wheels' models
	mWheelModelParam.resize(mWheelNumber, std::string(""));
	paramList = mTmpl->parameterList(std::string("wheel_model"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> modelIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(modelIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelModelParam[idx] = modelIdx[0];
	}
	mWheelScaleParam.resize(mWheelNumber, std::string("1.0"));
	paramList = mTmpl->parameterList(std::string("wheel_scale"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> scaleIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(scaleIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelScaleParam[idx] = scaleIdx[0];
	}
	//wheel is front
	mWheelIsFront.resize(mWheelNumber, false);
	paramList = mTmpl->parameterList(std::string("wheel_is_front"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> valueIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(valueIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelIsFront[idx] =
				(valueIdx[0] == std::string("true") ? true : false);
	}
	//wheel connection point ratio
	mWheelConnectionPointRatio.resize(mWheelNumber, LVecBase3f::zero());
	paramList = mTmpl->parameterList(
			std::string("wheel_connection_point_ratio"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> rxryrzIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(rxryrzIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		std::vector<std::string> rxryrz = parseCompoundString(*paramListIter,
				',');
		for (unsigned int i = 0; i < rxryrz.size() and i < 3; ++i)
		{
			mWheelConnectionPointRatio[idx][i] = strtof(rxryrz[i].c_str(),
			NULL);
		}
	}
	//wheel axle
	mWheelAxle.resize(mWheelNumber, LVector3f::zero());
	paramList = mTmpl->parameterList(std::string("wheel_axle"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> axayazIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(axayazIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		std::vector<std::string> axayaz = parseCompoundString(*paramListIter,
				',');
		for (unsigned int i = 0; i < axayaz.size() and i < 3; ++i)
		{
			mWheelAxle[idx][i] = strtof(axayaz[i].c_str(),
			NULL);
		}
	}
	//wheel direction
	mWheelDirection.resize(mWheelNumber, LVector3f::zero());
	paramList = mTmpl->parameterList(std::string("wheel_direction"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> dxdydzIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(dxdydzIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		std::vector<std::string> dxdydz = parseCompoundString(*paramListIter,
				',');
		for (unsigned int i = 0; i < dxdydz.size() and i < 3; ++i)
		{
			mWheelDirection[idx][i] = strtof(dxdydz[i].c_str(),
			NULL);
		}
	}
	//wheel suspension travel
	mWheelSuspensionTravel.resize(mWheelNumber, 40.0);
	paramList = mTmpl->parameterList(std::string("wheel_suspension_travel"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> stIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(stIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelSuspensionTravel[idx] = strtof(stIdx[0].c_str(),
		NULL);
	}
	//wheel suspension stiffness
	mWheelSuspensionStiffness.resize(mWheelNumber, 40.0);
	paramList = mTmpl->parameterList(std::string("wheel_suspension_stiffness"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> ssIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(ssIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelSuspensionStiffness[idx] = strtof(ssIdx[0].c_str(),
		NULL);
	}
	//wheel damping relaxation
	mWheelDampingRelaxation.resize(mWheelNumber, 2.0);
	paramList = mTmpl->parameterList(std::string("wheel_damping_relaxation"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> drIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(drIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelDampingRelaxation[idx] = strtof(drIdx[0].c_str(),
		NULL);
	}
	//wheel damping compression
	mWheelDampingCompression.resize(mWheelNumber, 4.0);
	paramList = mTmpl->parameterList(std::string("wheel_damping_compression"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> dcIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(dcIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelDampingCompression[idx] = strtof(dcIdx[0].c_str(),
		NULL);
	}
	//wheel friction slip
	mWheelFrictionSlip.resize(mWheelNumber, 100.0);
	paramList = mTmpl->parameterList(std::string("wheel_friction_slip"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> fsIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(fsIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelFrictionSlip[idx] = strtof(fsIdx[0].c_str(),
		NULL);
	}
	//wheel roll influence
	mWheelRollInfluence.resize(mWheelNumber, 0.1);
	paramList = mTmpl->parameterList(std::string("wheel_roll_influence"));
	for (paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> riIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(riIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelRollInfluence[idx] = strtof(riIdx[0].c_str(),
		NULL);
	}
	//physics parameter
	mMaxEngineForce = strtof(
			mTmpl->parameter(std::string("max_engine_force")).c_str(), NULL);
	mMaxBrakeForce = strtof(
			mTmpl->parameter(std::string("max_brake_force")).c_str(), NULL);
	mSteeringClamp = strtof(
			mTmpl->parameter(std::string("steering_clamp")).c_str(), NULL);
	mSteeringIncrement = strtof(
			mTmpl->parameter(std::string("steering_increment")).c_str(), NULL);
	mSteeringDecrement = strtof(
			mTmpl->parameter(std::string("steering_decrement")).c_str(), NULL);
	//forward key
	mForwardKey = (
			mTmpl->parameter(std::string("forward")) == std::string("enabled") ?
					true : false);
	//backward key
	mBackwardKey = (
			mTmpl->parameter(std::string("backward"))
					== std::string("enabled") ? true : false);
	//turn left key
	mForwardKey = (
			mTmpl->parameter(std::string("turn_left"))
					== std::string("enabled") ? true : false);
	//turn right key
	mBackwardKey = (
			mTmpl->parameter(std::string("turn_right"))
					== std::string("enabled") ? true : false);
	//
	return result;
}

void Vehicle::onAddToObjectSetup()
{
	//setup the chassis: at this point "Scene" (Model or InstanceOf)
	//and "Physics" (RigidBody) components should already have
	//been already setup.
	//The RigidBody has been added to physics: it must be removed
	//and then re-added in association with this Vehicle component.
	//Moreover the Object node path is the RigidBody one.

	//check if there is a RigidBody component
	SMARTPTR(RigidBody)rigidBodyComp = DCAST(RigidBody,
			mOwnerObject->getComponent(ComponentFamilyType("Physics")));
	if (not rigidBodyComp)
	{
		PRINT_ERR_DEBUG("Vehicle::onAddToObjectSetup: '" <<
				mOwnerObject->objectId() <<
				"' hasn't a RigidBody Component");
		return;
	}
	//remove rigid body from the physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
			&(static_cast<BulletRigidBodyNode&>(*rigidBodyComp)));
	//create BulletVehicle
	mVehicle = new BulletVehicle(
			GamePhysicsManager::GetSingletonPtr()->bulletWorld(),
			&(static_cast<BulletRigidBodyNode&>(*rigidBodyComp)));
	//set up axis
	mVehicle->set_coordinate_system(mUpAxis);
	//add BulletVehicle to physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(mVehicle);
	//
}

void Vehicle::onRemoveFromObjectCleanup()
{
	//remove BulletVehicle from physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(mVehicle);
	//check if there is a RigidBody component
	SMARTPTR(RigidBody)rigidBodyComp = DCAST(RigidBody,
			mOwnerObject->getComponent(ComponentFamilyType("Physics")));
	if (not rigidBodyComp)
	{
		PRINT_ERR_DEBUG("Vehicle::onAddToObjectSetup: '" <<
				mOwnerObject->objectId() <<
				"' hasn't a RigidBody Component");
		return;
	}
	//re-add rigid body to the physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			&(static_cast<BulletRigidBodyNode&>(*rigidBodyComp)));
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
	HOLD_REMUTEX(mMutex)

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
