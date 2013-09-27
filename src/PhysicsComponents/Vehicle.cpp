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
#include "ObjectModel/Object.h"
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
	//chassis
	mChassisTmpl = mTmpl->parameter(std::string("chassis_object_template"));
	//chassis model
	mChassisModelParam = mTmpl->parameter(std::string("chassis_model"));
	mChassisScaleParam = mTmpl->parameter(std::string("chassis_scale"));
	//chassis rigid body
	mChassisShapeTypeParam= mTmpl->parameter(std::string("chassis_shape_type"));
	mChassisShapeSizeParam = mTmpl->parameter(std::string("chassis_shape_size"));
	mChassisMassParam = mTmpl->parameter(std::string("chassis_mass"));
	mChassisFrictionParam = mTmpl->parameter(std::string("chassis_friction"));
	mChassisRestitutionParam = mTmpl->parameter(std::string("chassis_restitution"));
	mChassisCollideMaskParam = mTmpl->parameter(std::string("chassis_collide_mask"));
	//wheels
	mWheelTmpl = mTmpl->parameter(std::string("wheel_object_template"));
	//wheel number
	mWheelNumber = strtol(mTmpl->parameter(std::string("wheels_number")).c_str(),
			NULL, 0);
	if(mWheelNumber < 0)
	{
		mWheelNumber = 0;
	}
	//
	std::list<std::string> paramList;
	std::list<std::string>::const_iterator paramListIter;
	//wheels' models
	mWheelModelParam.resize(mWheelNumber, std::string(""));
	paramList = mTmpl->parameterList(std::string("wheel_model"));
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	paramList = mTmpl->parameterList(std::string("wheel_connection_point_ratio"));
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
			mWheelConnectionPointRatio[idx][i] = strtof(
					rxryrz[i].c_str(),
					NULL);
		}
	}
	//wheel axle
	mWheelAxle.resize(mWheelNumber, LVector3f::zero());
	paramList = mTmpl->parameterList(std::string("wheel_axle"));
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
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
	for(paramListIter = paramList.begin(); paramListIter != paramList.end();
			++paramListIter)
	{
		std::vector<std::string> riIdx = parseCompoundString(*paramListIter,
				'@');
		int idx = idxClamp(strtol(riIdx[1].c_str(), NULL, 0), 0,
				mWheelNumber - 1);
		mWheelRollInfluence[idx] = strtof(riIdx[0].c_str(),
		NULL);
	}
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
//	mNodePath = NodePath(mVehicle);

	//attach it to Bullet World
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			mVehicle);

	//set this character controller node path as the object's one
//	mOwnerObject->setNodePath(mNodePath);
}

void Vehicle::onRemoveFromObjectCleanup()
{
	NodePath oldObjectNodePath;
	//set the object node path to the first child of rigid body's one (if any)
//	if(mNodePath.get_num_children() > 0)
//	{
//		oldObjectNodePath = mNodePath.get_child(0);
//		//detach the object node path from the rigid body's one
//		oldObjectNodePath.detach_node();
//	}
//	else
//	{
//		oldObjectNodePath = NodePath();
//	}
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
//	mNodePath.remove_node();
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
