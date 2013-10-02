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
	std::string param;
	unsigned int idx;
	std::vector<std::string> paramValuesStr;
	//wheels' model params
	param = mTmpl->parameter(std::string("wheel_model"));
	paramValuesStr = parseCompoundString(param, ',');
	if(paramValuesStr.size() < mWheelNumber)
	{
		paramValuesStr.resize(mWheelNumber, "");
	}
	mWheelModelParam = paramValuesStr;
	//wheels' scale params
	param = mTmpl->parameter(std::string("wheel_scale"));
	paramValuesStr = parseCompoundString(param, ',');
	if(paramValuesStr.size() < mWheelNumber)
	{
		paramValuesStr.resize(mWheelNumber, "1.0");
	}
	mWheelScaleParam = paramValuesStr;
	//wheel is front
	param = mTmpl->parameter(std::string("wheel_is_front"));
	paramValuesStr = parseCompoundString(param, ',');
	if(paramValuesStr.size() < mWheelNumber)
	{
		paramValuesStr.resize(mWheelNumber, "false");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		mWheelIsFront.push_back(
				paramValuesStr[idx] == std::string("true") ? true : false);
	}
	//wheel connection point ratio
	param = mTmpl->parameter(std::string("wheel_connection_point_ratio"));
	paramValuesStr = parseCompoundString(param, '$');
	if(paramValuesStr.size() < mWheelNumber)
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
			if (values[i] < 0.0)
			{
				values[i] = -values[i];
			}
		}
		mWheelConnectionPointRatio.push_back(values);
	}
	//wheel axle
	param = mTmpl->parameter(std::string("wheel_axle"));
	paramValuesStr = parseCompoundString(param, '$');
	if(paramValuesStr.size() < mWheelNumber)
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
			values.normalize();
			if (values.length_squared() == 0.0)
			{
				values = LVector3f(1.0, 0.0, 0.0);
			}
		}
		mWheelAxle.push_back(values);
	}
	//wheel direction
	param = mTmpl->parameter(std::string("wheel_direction"));
	paramValuesStr = parseCompoundString(param, '$');
	if(paramValuesStr.size() < mWheelNumber)
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
			values.normalize();
			if (values.length_squared() == 0.0)
			{
				values = LVector3f(0.0, 0.0, -1.0);
			}
		}
		mWheelDirection.push_back(values);
	}
	//wheel suspension travel
	param = mTmpl->parameter(std::string("wheel_suspension_travel"));
	paramValuesStr = parseCompoundString(param, ',');
	if(paramValuesStr.size() < mWheelNumber)
	{
		paramValuesStr.resize(mWheelNumber, "40.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		float value = strtof(paramValuesStr[idx].c_str(), NULL);
		if(value < 0.0)
		{
			value = -value;
		}
		mWheelSuspensionTravel.push_back(value);
	}
	//wheel suspension stiffness
	param = mTmpl->parameter(std::string("wheel_suspension_stiffness"));
	paramValuesStr = parseCompoundString(param, ',');
	if(paramValuesStr.size() < mWheelNumber)
	{
		paramValuesStr.resize(mWheelNumber, "40.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		float value = strtof(paramValuesStr[idx].c_str(), NULL);
		if(value < 0.0)
		{
			value = -value;
		}
		mWheelSuspensionStiffness.push_back(value);
	}
	//wheel damping relaxation
	param = mTmpl->parameter(std::string("wheel_damping_relaxation"));
	paramValuesStr = parseCompoundString(param, ',');
	if(paramValuesStr.size() < mWheelNumber)
	{
		paramValuesStr.resize(mWheelNumber, "2.0");
	}
	for (idx = 0; idx < mWheelNumber; ++idx)
	{
		float value = strtof(paramValuesStr[idx].c_str(), NULL);
		if(value < 0.0)
		{
			value = -value;
		}
		mWheelDampingRelaxation.push_back(value);
	}

	///TODO
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
	//get chassis dimensions from Scene component node path,
	//which is child of RigidBody component node path;
	GamePhysicsManager::GetSingletonPtr()->getBoundingDimensions(
			rigidBodyComp->getNodePath().get_child(0),
			vehicleDims, vehicleDeltaCenter, vehicleRadius);
	//add BulletVehicle to physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(mVehicle);
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
	//wheels' objects procedure creation
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

	//check if wheel template has a Model or InstanceOf
	std::string sceneComp, sceneCompParam;
	SMARTPTR(ComponentTemplate) compTmpl =
	wheelTmpl->getComponentTemplate(ComponentType("Model"));
	if(compTmpl)
	{
		sceneComp = "Model";
		sceneCompParam = "model_file";
	}
	else
	{
		compTmpl = wheelTmpl->getComponentTemplate(ComponentType("InstanceOf"));
		if (compTmpl)
		{
			sceneComp = "InstanceOf";
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
	//owner object's parent == wheels' parent
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
		compTmplParams[sceneComp].insert(
				ParameterTable::value_type(sceneCompParam, mWheelModelParam[idx]));
		//Scene component scale param
		compTmplParams[sceneComp].insert(
				ParameterTable::value_type("scale", mWheelScaleParam[idx]));
		//actually create the wheel object
		std::ostringstream idxStr;
		idxStr << idx;
		mWheelObjects[idx] = ObjectTemplateManager::GetSingletonPtr()->
				createObject(ObjectType(mWheelTmpl),
						ObjectId(mComponentId + "Wheel" + idxStr),
						objTmplParam, compTmplParams, false);
	}
	//add wheels to BulletVehicle
	for(unsigned int idx = 0; idx < mWheelNumber; ++idx)
	{
		//get the wheel radius from the Scene component
	}



//    wheelFR = app.render.attachNewNode("wheelFR")
//    wheel.instanceTo(wheelFR)
//    wOffset = LVector3f(-vehicleDims.getX() / 2.0 * 0.8,
//                        vehicleDims.getY() / 2.0 * 0.65,
//                        - vehicleDims.getZ() / 2.0 * 0.8)
//    wheelBodyFR = createWheel(True, wheelDims.getZ() / 2.0,
//                              LPoint3f(wOffset))
//
//                              def createWheel(isFront, radius,
//                                              connectionPointCs,
//                                              axleCs=LVector3f(1, 0, 0),
//                                              directionCs=Vec3(0, 0, -1),
//                                              suspensionTravelCm=40.0, suspensionStiffness=40.0,
//                                              dampingRelaxation=2.3, dampingCompression=4.4,
//                                              frictionSlip=100.0, rollInfluence=0.1):
//                                  wheelBody = vehicle.createWheel()
//                                  wheelBody.setFrontWheel(isFront)
//                                  wheelBody.setWheelRadius(radius)
//                                  wheelBody.setChassisConnectionPointCs(connectionPointCs)
//                                  wheelBody.setWheelAxleCs(axleCs)
//                                  wheelBody.setWheelDirectionCs(directionCs)
//                                  wheelBody.setMaxSuspensionTravelCm(suspensionTravelCm)
//                                  wheelBody.setSuspensionStiffness(suspensionStiffness)
//                                  wheelBody.setWheelsDampingRelaxation(dampingRelaxation)
//                                  wheelBody.setWheelsDampingCompression(dampingCompression)
//                                  wheelBody.setFrictionSlip(frictionSlip)
//                                  wheelBody.setRollInfluence(rollInfluence)
//                                  return wheelBody
//
//    wheelBodyFR.setNode(wheelFR.node())













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
