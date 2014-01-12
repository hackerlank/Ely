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
#include <bulletWheel.h>
#include <throw_event.h>

namespace
{
const std::string STARTEVENT("OnStartVehicle");
const std::string STOPEVENT("OnStopVehicle");
}  // namespace

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
	//wheel object template
	mWheelTmpl = mTmpl->parameter(std::string("wheel_object_template"));
	//
	float value;
	int valueInt;
	//wheels number
	valueInt = strtol(mTmpl->parameter(std::string("wheels_number")).c_str(),
	NULL, 0);
	mWheelNumber = (valueInt >= 0.0 ? valueInt : -valueInt);
	//
	std::string param;
	unsigned int idx, valueNum;
	std::vector<std::string> paramValuesStr;
	//wheels' model params
	param = mTmpl->parameter(std::string("wheel_model"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum < mWheelNumber)
	{
		paramValuesStr.resize(mWheelNumber, "");
	}
	mWheelModelParam = paramValuesStr;
	//wheel scale
	param = mTmpl->parameter(std::string("wheel_scale"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "1.0");
	}
	mWheelScaleParam = paramValuesStr;
	//wheel is front
	param = mTmpl->parameter(std::string("wheel_is_front"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "false");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		mWheelIsFront.push_back(
				paramValuesStr[idx] == std::string("true") ? true : false);
	}
	//wheel set steering
	param = mTmpl->parameter(std::string("wheel_set_steering"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "false");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		mWheelSetSteering.push_back(
				paramValuesStr[idx] == std::string("true") ? true : false);
	}
	//wheel apply engine force
	param = mTmpl->parameter(std::string("wheel_apply_engine_force"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "false");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		mWheelApplyEngineForce.push_back(
				paramValuesStr[idx] == std::string("true") ? true : false);
	}
	//wheel set brake
	param = mTmpl->parameter(std::string("wheel_set_brake"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "false");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		mWheelSetBrake.push_back(
				paramValuesStr[idx] == std::string("true") ? true : false);
	}
	//wheel connection point ratio
	param = mTmpl->parameter(std::string("wheel_connection_point_ratio"));
	paramValuesStr = parseCompoundString(param, '$');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "1.0,1.0,1.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		std::vector<std::string> paramValuesStrExt = parseCompoundString(
				paramValuesStr[idx], ',');
		if (paramValuesStrExt.size() < 3)
		{
			paramValuesStrExt.resize(3, "1.0");
		}
		LVecBase3f values;
		for (unsigned int i = 0; i < 3; ++i)
		{
			values[i] = strtof(paramValuesStrExt[i].c_str(), NULL);
		}
		mWheelConnectionPointRatio.push_back(values);
	}
	//wheel axle
	param = mTmpl->parameter(std::string("wheel_axle"));
	paramValuesStr = parseCompoundString(param, '$');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "1.0,0.0,0.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		std::vector<std::string> paramValuesStrExt = parseCompoundString(
				paramValuesStr[idx], ',');
		if (paramValuesStrExt.size() < 3)
		{
			paramValuesStrExt.resize(3, "0.0");
		}
		LVector3f values;
		for (unsigned int i = 0; i < 3; ++i)
		{
			values[i] = strtof(paramValuesStrExt[i].c_str(), NULL);
		}
		values.normalize();
		if (values.length_squared() == 0.0)
		{
			values = LVector3f(1.0, 0.0, 0.0);
		}
		mWheelAxle.push_back(values);
	}
	//wheel direction
	param = mTmpl->parameter(std::string("wheel_direction"));
	paramValuesStr = parseCompoundString(param, '$');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "0.0,0.0,-1.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		std::vector<std::string> paramValuesStrExt = parseCompoundString(
				paramValuesStr[idx], ',');
		if (paramValuesStrExt.size() < 3)
		{
			paramValuesStrExt.resize(3, "0.0");
		}
		LVector3f values;
		for (unsigned int i = 0; i < 3; ++i)
		{
			values[i] = strtof(paramValuesStrExt[i].c_str(), NULL);
		}
		values.normalize();
		if (values.length_squared() == 0.0)
		{
			values = LVector3f(0.0, 0.0, -1.0);
		}
		mWheelDirection.push_back(values);
	}
	//wheel suspension travel
	param = mTmpl->parameter(std::string("wheel_suspension_travel"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "40.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mWheelSuspensionTravel.push_back(value >= 0.0 ? value : -value);
	}
	//wheel suspension stiffness
	param = mTmpl->parameter(std::string("wheel_suspension_stiffness"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "40.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mWheelSuspensionStiffness.push_back(value >= 0.0 ? value : -value);
	}
	//wheel damping relaxation
	param = mTmpl->parameter(std::string("wheel_damping_relaxation"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "2.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mWheelDampingRelaxation.push_back(value >= 0.0 ? value : -value);
	}
	//wheel damping compression
	param = mTmpl->parameter(std::string("wheel_damping_compression"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "4.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mWheelDampingCompression.push_back(value >= 0.0 ? value : -value);
	}
	//wheel friction slip
	param = mTmpl->parameter(std::string("wheel_friction_slip"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "100.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mWheelFrictionSlip.push_back(value >= 0.0 ? value : -value);
	}
	//wheel roll influence
	param = mTmpl->parameter(std::string("wheel_roll_influence"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < mWheelNumber))
	{
		paramValuesStr.resize(mWheelNumber, paramValuesStr[0]);
	}
	else if (valueNum == 0)
	{
		paramValuesStr.resize(mWheelNumber, "0.1");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mWheelRollInfluence.push_back(value >= 0.0 ? value : -value);
	}
	//max engine force
	mMaxEngineForce = strtof(
			mTmpl->parameter(std::string("max_engine_force")).c_str(), NULL);
	//max brake
	mMaxBrake = strtof(mTmpl->parameter(std::string("max_brake")).c_str(),
	NULL);
	//steering clamp [0.0,90.0]
	value = strtof(mTmpl->parameter(std::string("steering_clamp")).c_str(),
	NULL) / 90.0;
	mSteeringClamp =
			(value >= 0.0 ?
					(value - floor(value)) * 90.0 : (ceil(value) - value) * 90.0);
	//steering increment
	value = strtof(mTmpl->parameter(std::string("steering_increment")).c_str(),
	NULL);
	mSteeringIncrement = (value >= 0.0 ? value : -value);
	//steering decrement
	value = strtof(mTmpl->parameter(std::string("steering_decrement")).c_str(),
	NULL);
	mSteeringDecrement = (value >= 0.0 ? value : -value);
	//forward key
	mForwardKey = (
			mTmpl->parameter(std::string("forward")) == std::string("enabled") ?
					true : false);
	//backward key
	mBackwardKey = (
			mTmpl->parameter(std::string("backward"))
					== std::string("enabled") ? true : false);
	//brake key
	mBrakeKey = (
			mTmpl->parameter(std::string("brake")) == std::string("enabled") ?
					true : false);
	//turn left key
	mTurnLeftKey = (
			mTmpl->parameter(std::string("turn_left"))
					== std::string("enabled") ? true : false);
	//turn right key
	mTurnRightKey = (
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
	SMARTPTR(Component) physicsComp =
			mOwnerObject->getComponent(ComponentFamilyType("Physics"));
	if ((not physicsComp) or
			(not physicsComp->is_of_type(RigidBody::get_class_type())))
	{
		PRINT_ERR_DEBUG("Vehicle::onAddToObjectSetup: '" <<
				mOwnerObject->objectId() <<
				"' hasn't a RigidBody Component");
		return;
	}
	//there is a RigidBody component
	SMARTPTR(RigidBody) rigidBodyComp = DCAST(RigidBody, physicsComp);
	//for vehicle disable deactivation
	static_cast<BulletRigidBodyNode&>(*rigidBodyComp).set_deactivation_enabled(false);

	//create BulletVehicle
	mVehicle = new BulletVehicle(
			GamePhysicsManager::GetSingletonPtr()->bulletWorld(),
			&(static_cast<BulletRigidBodyNode&>(*rigidBodyComp)));
	//set up axis
	mVehicle->set_coordinate_system(mUpAxis);
	//get chassis dimensions from owner object node path,
	GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
			mOwnerObject->getNodePath(),
			mVehicleDims, mVehicleDeltaCenter, mVehicleRadius);
	//add BulletVehicle to physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(mVehicle);
}

void Vehicle::onRemoveFromObjectCleanup()
{
	//check if there is a RigidBody component
	SMARTPTR(Component) physicsComp =
			mOwnerObject->getComponent(ComponentFamilyType("Physics"));
	RETURN_ON_COND((not physicsComp) or
			(not physicsComp->is_of_type(RigidBody::get_class_type())),)

	//remove BulletVehicle from physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(mVehicle);

	//re-add temporarily BulletRigidBodyNode of physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			&(DCAST(RigidBody, physicsComp)->getBulletRigidBodyNode()));
	//
	reset();
}

void Vehicle::onAddToSceneSetup()
{
	//check if there is a valid wheel Object template
	SMARTPTR(ObjectTemplate)wheelTmpl =
	ObjectTemplateManager::GetSingletonPtr()->
	getObjectTemplate(ObjectType(mWheelTmpl));

	if(not wheelTmpl)
	{
		PRINT_ERR_DEBUG("Vehicle::onAddToObjectSetup: '"
				<< mWheelTmpl <<
				"' Object template doesn't exist");
		return;
	}

	//wheels' objects procedure creation
	//check if wheel template has a Model or InstanceOf
	ComponentType sceneCompType;
	std::string sceneCompParam;
	SMARTPTR(ComponentTemplate) compTmpl =
	wheelTmpl->getComponentTemplate(ComponentType("Model"));
	if(compTmpl)
	{
		sceneCompType = ComponentType("Model");
		sceneCompParam = "model_file";
	}
	else
	{
		compTmpl = wheelTmpl->getComponentTemplate(ComponentType("InstanceOf"));
		if (compTmpl)
		{
			sceneCompType = ComponentType("InstanceOf");
			sceneCompParam = "instance_of";
		}
		else
		{
			PRINT_ERR_DEBUG("Vehicle::onAddToObjectSetup: '"
					<< mWheelTmpl <<
					"' Object template hasn't a required 'Scene' component");
		}
	}
	//prepare parameters tables and create the wheels' objects
	mWheelObjects.resize(mWheelNumber);
	mWheelRadius.resize(mWheelNumber);
	//owner wheels' parent == object's parent
	std::string parentId = mOwnerObject->objectTmpl()->parameter(std::string("parent"));
	for(unsigned int idx = 0; idx < mWheelNumber; ++idx)
	{
		//for each wheel:
		//set an object ParameterTable
		ParameterTable objTmplParam;
		//Object parent param
		objTmplParam.insert(
				ParameterTable::value_type("parent", parentId));
		//set a component ParameterTableMap
		ParameterTableMap compTmplParams;
		//Scene component model param
		compTmplParams[sceneCompType].insert(
				ParameterTable::value_type(sceneCompParam, mWheelModelParam[idx]));
		//Scene component scale param
		compTmplParams[sceneCompType].insert(
				ParameterTable::value_type("scale", mWheelScaleParam[idx]));
		//actually create the wheel object
		mWheelObjects[idx] = ObjectTemplateManager::GetSingletonPtr()->
		createObject(ObjectType(mWheelTmpl),
				ObjectId(mComponentId + std::string("Wheel") +
						dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(idx)).str()),
				objTmplParam, compTmplParams, false, mOwnerObject);
		//object initialization
		mWheelObjects[idx]->worldSetup();

		//add wheel to BulletVehicle
		//get the wheel radius from the object component:
		//the radius is taken along the up axis
		LVector3f wheelDims, wheelDeltaCenter;
		float wheelMaxRadius;
		GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
				mWheelObjects[idx]->getNodePath(),
				wheelDims, wheelDeltaCenter, wheelMaxRadius);
		//evaluate wheel radius
		if (mUpAxis == X_up)
		{
			mWheelRadius[idx] = wheelDims.get_x() / 2.0;
		}
		else if (mUpAxis == Y_up)
		{
			mWheelRadius[idx] = wheelDims.get_y() / 2.0;
		}
		else //mUpAxis == Z_up
		{
			mWheelRadius[idx] = wheelDims.get_z() / 2.0;
		}
		//evaluate wheel connection point
		LPoint3f wheelConnectionPoint(
				mVehicleDims.get_x() / 2.0 * mWheelConnectionPointRatio[idx].get_x(),
				mVehicleDims.get_y() / 2.0 * mWheelConnectionPointRatio[idx].get_y(),
				mVehicleDims.get_z() / 2.0 * mWheelConnectionPointRatio[idx].get_z()
		);
		//create effectively the wheel
		BulletWheel wheel = mVehicle->create_wheel();
		wheel.set_front_wheel(mWheelIsFront[idx]);
		wheel.set_wheel_radius(mWheelRadius[idx]);
		wheel.set_chassis_connection_point_cs(wheelConnectionPoint);
		wheel.set_wheel_axle_cs(mWheelAxle[idx]);
		wheel.set_wheel_direction_cs(mWheelDirection[idx]);
		wheel.set_max_suspension_travel_cm(mWheelSuspensionTravel[idx]);
		wheel.set_suspension_stiffness(mWheelSuspensionStiffness[idx]);
		wheel.set_wheels_damping_relaxation(mWheelDampingRelaxation[idx]);
		wheel.set_wheels_damping_compression(mWheelDampingCompression[idx]);
		wheel.set_friction_slip(mWheelFrictionSlip[idx]);
		wheel.set_roll_influence(mWheelRollInfluence[idx]);
		wheel.set_world_transform(LMatrix4f::ident_mat());
		//set the wheel node path
		wheel.set_node(mWheelObjects[idx]->getNodePath().node());
	}

	//Add to the physics manager update
	GamePhysicsManager::GetSingletonPtr()->addToPhysicsUpdate(this);

	//clear all no more needed "Param" variables
	mWheelModelParam.clear();
	mWheelScaleParam.clear();
}

void Vehicle::onRemoveFromSceneCleanup()
{
	//check if there is a valid wheel Object template
	SMARTPTR(ObjectTemplate)wheelTmpl =
	ObjectTemplateManager::GetSingletonPtr()->
	getObjectTemplate(ObjectType(mWheelTmpl));

	RETURN_ON_COND(not wheelTmpl,)

	//remove from the physics manager update
	GamePhysicsManager::GetSingletonPtr()->removeFromPhysicsUpdate(this);

	//wheels' objects procedure destruction
	for (unsigned int idx = 0; idx < mWheelNumber; ++idx)
	{
		//reset the wheel node path
		mVehicle->get_wheel(idx).set_node(NULL);
		//remove wheel object
		ObjectTemplateManager::GetSingletonPtr()->destroyObject(
				mWheelObjects[idx]->objectId());
	}
}

void Vehicle::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//update vehicle
	//process input
	float engineForce = 0.0;
	float brake = 0.0;
	//handle keys:
	if (mForward)
	{
		engineForce = mMaxEngineForce;
	}
	else if (mBackward)
	{
		engineForce = -mMaxEngineForce;
	}
	if (mBrake)
	{
		brake = mMaxBrake;
	}
	if (mTurnLeft)
	{
		mSteering += dt * mSteeringIncrement;
		mSteering = min(mSteering, mSteeringClamp);
	}
	else if (mTurnRight)
	{
		mSteering -= dt * mSteeringIncrement;
		mSteering = max(mSteering, -mSteeringClamp);
	}
	else
	{
		if (mSteering < 0.0)
		{
			mSteering = min(mSteering + mSteeringDecrement * dt, float(0.0));
		}
		else if (mSteering > 0.0)
		{
			mSteering = max(mSteering - mSteeringDecrement * dt, float(0.0));
		}
	}
	//Apply steering, engine and brake forces to wheels
	for (unsigned int idx = 0; idx < mWheelNumber; ++idx)
	{
		if (mWheelSetSteering[idx])
		{
			mVehicle->set_steering_value(mSteering, idx);
		}
		if (mWheelApplyEngineForce[idx])
		{
			mVehicle->apply_engine_force(engineForce, idx);
		}
		if (mWheelSetBrake[idx])
		{
			mVehicle->set_brake(brake, idx);
		}
	}

	if (mThrowEvents)
	{
		//handle Vehicle start-stop events
		if (mVehicle->get_current_speed_km_hour() == 0.0)
		{
			//throw mOnStart event (if enabled)
			if (not mOnStartSent)
			{
				throw_event(STARTEVENT, EventParameter(this),
						EventParameter(std::string(mOwnerObject->objectId())));
				mOnStartSent = true;
				mOnStopSent = false;
			}
		}
		else
		{
			//throw OnStop event (if enabled)
			if (not mOnStopSent)
			{
				throw_event(STOPEVENT, EventParameter(this),
						EventParameter(std::string(mOwnerObject->objectId())));
				mOnStopSent = true;
				mOnStartSent = false;
			}
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle Vehicle::_type_handle;

} /* namespace ely */
