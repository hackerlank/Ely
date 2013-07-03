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
 * \file /Ely/src/AIComponents/Steering.cpp
 *
 * \date 03/dic/2012 (13:39:59)
 * \author consultit
 */

#include "AIComponents/Steering.h"
#include "AIComponents/SteeringTemplate.h"
#include <seek.h>
#include <flee.h>
#include <pursue.h>
#include <evade.h>
#include <pathFollow.h>
#include <arrival.h>
#include <wander.h>
#include <obstacleAvoidance.h>

///XXX: conflicting declaration: ‘NodeArray’
//aiPathFinder.h: ‘typedef class std::vector<Node*, std::allocator<Node*> > NodeArray’
//btQuantizedBvh.h: ‘typedef class btAlignedObjectArray<btOptimizedBvhNode> NodeArray’
#ifndef _PATHFINDER_H
#define _PATHFINDER_H
#include <meshNode.h>
#include <lineSegs.h>
typedef vector<Node *> NodeArrayAI; //redefined
typedef vector<NodeArrayAI> NavMesh;
class EXPCL_PANDAAI PathFinder;
#endif
#include <pathFind.h>
#include <throw_event.h>
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"

namespace ely
{

Steering::Steering()
{
	// TODO Auto-generated constructor stub
}

Steering::Steering(SMARTPTR(SteeringTemplate)tmpl):mIsEnabled(false)
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"Steering::Steering: invalid GameAIManager")
	CHECKEXISTENCE(GamePhysicsManager::GetSingletonPtr(), "Steering::Steering: "
			"invalid GamePhysicsManager")
	//get bullet world reference
	mWorld = GamePhysicsManager::GetSingletonPtr()->bulletWorld();
	mTmpl = tmpl;
	mAICharacter = NULL;
	mUpdatePtr = NULL;
	mCharacterController = NULL;
	mMovRotEnabled = false;
}

Steering::~Steering()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	disable();
}

const ComponentFamilyType Steering::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Steering::componentType() const
{
	return mTmpl->componentType();
}

bool Steering::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//enabling setting
	mEnabled = (
			mTmpl->parameter(std::string("enabled")) == std::string("true") ?
					true : false);
	//throw events setting
	mThrowEvents = (
			mTmpl->parameter(std::string("throw_events"))
					== std::string("true") ? true : false);
	//set AICharacter parameters
	//
	float floatParam;
	//mass
	floatParam = (float) strtof(mTmpl->parameter(std::string("mass")).c_str(),
			NULL);
	floatParam > 0.0 ? mMass = floatParam : mMass = 1.0;
	//movt_force
	floatParam = (float) strtof(
			mTmpl->parameter(std::string("movt_force")).c_str(), NULL);
	floatParam > 0.0 ? mMovtForce = floatParam : mMovtForce = 1.0;
	//max_force
	floatParam = (float) strtof(
			mTmpl->parameter(std::string("max_force")).c_str(), NULL);
	floatParam > 0.0 ? mMaxForce = floatParam : mMaxForce = 1.0;
	//the type of the updatable item
	mType = mTmpl->parameter(std::string("controlled_type"));
	//obstacle hit mask
	std::string obstacleHitMask = mTmpl->parameter(
			std::string("obstacle_hit_mask"));
	if (obstacleHitMask == std::string("all_on"))
	{
		mObstacleHitMask = BitMask32::all_on();
	}
	else if (obstacleHitMask == std::string("all_off"))
	{
		mObstacleHitMask = BitMask32::all_off();
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(obstacleHitMask.c_str(), NULL, 0);
		mObstacleHitMask.set_word(mask);
#ifdef ELY_DEBUG
		mObstacleHitMask.write(std::cout, 0);
#endif
	}
	//obstacle max distance fraction
	floatParam =
			(float) strtof(
					mTmpl->parameter(
							std::string("obstacle_max_distance_fraction")).c_str(),
					NULL);
	floatParam > 0.0 ?
			mObstacleMaxDistanceFraction = floatParam :
			mObstacleMaxDistanceFraction = 1.0;
	//
	return result;
}

void Steering::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//setup event callbacks if any
	setupEvents();
}

void Steering::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//enable the component
	if (mEnabled)
	{
		enable();
	}
}

void Steering::enable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mIsEnabled or (not mOwnerObject))
	{
		return;
	}

	//enable only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//create the AICharacter...
	mAICharacter = new AICharacter(std::string(mComponentId),
			mOwnerObject->getNodePath(), mMass, mMovtForce, mMaxForce);
	//...add it to the AIWorld ...
	GameAIManager::GetSingletonPtr()->aiWorld()->add_ai_char(mAICharacter);
	//...get a reference to its AIBehaviors
	_steering = mAICharacter->get_ai_behaviors();

	//check the type of the updatable item
	if ((mType == std::string("character_controller"))
			and (mOwnerObject->getComponent(ComponentFamilyType("Physics"))->is_of_type(
					CharacterController::get_class_type())))
	{
		//update the character_controller
		mUpdatePtr = &Steering::updateController;
		//get a reference to the CharacterController component
		//(which is already created and set up)
		mCharacterController =
				DCAST(CharacterController, mOwnerObject->getComponent(
								ComponentFamilyType("Physics")));

		//save current movement and set it not local (global)
		mCurrentIsLocal = mCharacterController->getIsLocal();
		//set current movement global (not local)
		mCharacterController->setIsLocal(false);
		//reset events' sending
		mSteeringForceOnSent = false;
		mSteeringForceOffSent = true;
		//set obstacle avoidance distance and squared
		mObstacleMaxDist =
				mObstacleMaxDistanceFraction
						* mAICharacter->get_node_path().get_bounds()->as_bounding_sphere()->get_radius();
		mObstacleMaxDistSquared = pow(mObstacleMaxDist, 2);
		//enable movement/rotation
		enableMovRot(true);
	}
	else
	{
		//update the owner object nodepath: default
		mUpdatePtr = &Steering::updateNodePath;
	}

	//Add to the AI manager update
	GameAIManager::GetSingletonPtr()->addToAIUpdate(this);
	//
	mIsEnabled = not mIsEnabled;
	//register event callbacks if any
	registerEventCallbacks();
}

void Steering::disable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if ((not mIsEnabled) or (not mOwnerObject))
	{
		return;
	}

	//disable only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	if (mType == std::string("character_controller"))
	{
		//disable movement/rotation
		enableMovRot(false);
		//restore current movement
		mCharacterController->setIsLocal(mCurrentIsLocal);
		//throw SteeringForceOff event (if enabled)
		if (mThrowEvents and (not mSteeringForceOffSent))
		{
			throw_event(std::string("SteeringForceOff"), EventParameter(this),
					EventParameter(std::string(mOwnerObject->objectId())));
		}
	}
	//check if AI manager exists
	if (GameAIManager::GetSingletonPtr())
	{
		//remove from AIWorld
		GameAIManager::GetSingletonPtr()->aiWorld()->remove_ai_char(
				std::string(mComponentId));
	}
	delete mAICharacter;
	mAICharacter = NULL;

	//check if AI manager exists
	if (GameAIManager::GetSingletonPtr())
	{
		//remove from AI manager update
		GameAIManager::GetSingletonPtr()->removeFromAIUpdate(this);
	}
	//reset update ptr
	mUpdatePtr = NULL;
	//
	mIsEnabled = not mIsEnabled;
	//unregister event callbacks if any
	unregisterEventCallbacks();
}

bool Steering::isEnabled()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsEnabled;
}

AICharacter* const Steering::getAiCharacter() const
{
	return mAICharacter;
}

NodePath Steering::getTargetNodePath(const ObjectId& target)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	NodePath objectNodePath("");
	//check if there is an object with that ObjectId;
	//that object is supposed to be already created, set up,
	//added to the scene and added to the created objects table;
	CSMARTPTR(Object)targetObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			target);
	if (targetObject != NULL)
	{
		objectNodePath = targetObject->getNodePath();
	}
	//
	return objectNodePath;
}

NodePath Steering::getTargetNodePath(CSMARTPTR(Object)target)
{
	//lock (guard) the mutex
		HOLDMUTEX(mMutex)

		NodePath objectNodePath("");
		if (target != NULL)
		{
			objectNodePath = target->getNodePath();
		}
		//
		return objectNodePath;
	}

void Steering::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//call the designated update member
	((*this).*mUpdatePtr)(dt);
}

void Steering::updateNodePath(float dt)
{
	mAICharacter->update();
}

void Steering::updateController(float dt)
{

	if (!_steering->is_off(_steering->_none))
	{
		//enable movement/rotation
		if (not mMovRotEnabled)
		{
			enableMovRot(true);
		}
		LVecBase3f steering_force = calculate_prioritized(dt);
		LVecBase3f acceleration = steering_force / mAICharacter->get_mass();
		mAICharacter->_velocity = acceleration;
		LVecBase3f direction = _steering->_steering_force;
		direction.normalize();
		mCharacterController->setLinearSpeed(
				-mAICharacter->get_velocity().get_xy() / dt);
		if (steering_force.length() > 0)
		{
			//0 <= A <= 180.0
			//the heavier is a character the slower to turn it is
			float H = mAICharacter->get_node_path().get_h();
			float A = 57.295779513f * acos(direction.get_y());
			float deltaAngle;
			if (direction.get_x() <= 0.0)
			{
				if (H <= 0.0)
				{
					deltaAngle = -H + A - 180;
				}
				else
				{
					deltaAngle = (A <= H ? -H + A + 180 : -H + A - 180);
				}
			}
			else
			{
				if (H >= 0.0)
				{
					deltaAngle = -H - A + 180;
				}
				else
				{
					deltaAngle = (A >= -H ? -H - A + 180 : -H - A - 180);
				}
			}
			mCharacterController->setAngularSpeed(deltaAngle / (mMass * dt));
			//throw SteeringForceOn event (if enabled)
			if (mThrowEvents and (not mSteeringForceOnSent))
			{
				throw_event(std::string("SteeringForceOn"),
						EventParameter(this),
						EventParameter(std::string(mOwnerObject->objectId())));
				mSteeringForceOnSent = true;
				mSteeringForceOffSent = false;
			}
		}
		else
		{
			if (mMovRotEnabled)
			{
				enableMovRot(false);
			}
			//throw SteeringForceOff event (if enabled)
			if (mThrowEvents and (not mSteeringForceOffSent))
			{
				throw_event(std::string("SteeringForceOff"),
						EventParameter(this),
						EventParameter(std::string(mOwnerObject->objectId())));
				mSteeringForceOffSent = true;
				mSteeringForceOnSent = false;
			}
		}
	}
	else
	{
		_steering->_steering_force = LVecBase3f(0.0, 0.0, 0.0);
		_steering->_seek_force = LVecBase3f(0.0, 0.0, 0.0);
		_steering->_flee_force = LVecBase3f(0.0, 0.0, 0.0);
		_steering->_pursue_force = LVecBase3f(0.0, 0.0, 0.0);
		_steering->_evade_force = LVecBase3f(0.0, 0.0, 0.0);
		_steering->_arrival_force = LVecBase3f(0.0, 0.0, 0.0);
		_steering->_flock_force = LVecBase3f(0.0, 0.0, 0.0);
		_steering->_wander_force = LVecBase3f(0.0, 0.0, 0.0);
		//reset movements
		if (mMovRotEnabled)
		{
			enableMovRot(false);
		}
		//reset events' sending
		mSteeringForceOnSent = false;
		mSteeringForceOffSent = true;
	}
}

void Steering::enableMovRot(bool enable)
{
	//reset velocities
	mCharacterController->setLinearSpeed(LVecBase2f::zero());
	mCharacterController->setAngularSpeed(0.0);
	//enable/disable movements
	mCharacterController->enableForward(enable);
	mCharacterController->enableStrafeLeft(enable);
	mCharacterController->enableRollLeft(enable);
	mMovRotEnabled = enable;
}

///XXX AIBehaviors::<Steering methods> rewritten
//calculate_prioritized
LVecBase3f Steering::calculate_prioritized(float dt)
{
	LVecBase3f force;

	if (_steering->is_on(_steering->_seek))
	{
		if (_steering->_conflict)
		{
			force = do_seek() * _steering->_seek_obj->_seek_weight;
		}
		else
		{
			force = do_seek();
		}
		_steering->accumulate_force("seek", force);
	}

	if (_steering->is_on(_steering->_flee_activate))
	{
		for (_steering->_flee_itr = _steering->_flee_list.begin();
				_steering->_flee_itr != _steering->_flee_list.end();
				_steering->_flee_itr++)
		{
			flee_activate();
		}
	}

	if (_steering->is_on(_steering->_flee))
	{
		for (_steering->_flee_itr = _steering->_flee_list.begin();
				_steering->_flee_itr != _steering->_flee_list.end();
				_steering->_flee_itr++)
		{
			if (_steering->_flee_itr->_flee_activate_done)
			{
				if (_steering->_conflict)
				{
					force = do_flee() * _steering->_flee_itr->_flee_weight;
				}
				else
				{
					force = do_flee();
				}
				_steering->accumulate_force("flee", force);
			}
		}
	}

	if (_steering->is_on(_steering->_pursue))
	{
		if (_steering->_conflict)
		{
			force = do_pursue() * _steering->_pursue_obj->_pursue_weight;
		}
		else
		{
			force = do_pursue();
		}
		_steering->accumulate_force("pursue", force);
	}

	if (_steering->is_on(_steering->_evade_activate))
	{
		for (_steering->_evade_itr = _steering->_evade_list.begin();
				_steering->_evade_itr != _steering->_evade_list.end();
				_steering->_evade_itr++)
		{
			evade_activate();
		}
	}

	if (_steering->is_on(_steering->_evade))
	{
		for (_steering->_evade_itr = _steering->_evade_list.begin();
				_steering->_evade_itr != _steering->_evade_list.end();
				_steering->_evade_itr++)
		{
			if (_steering->_evade_itr->_evade_activate_done)
			{
				if (_steering->_conflict)
				{
					force = (do_evade())
							* (_steering->_evade_itr->_evade_weight);
				}
				else
				{
					force = do_evade();
				}
				_steering->accumulate_force("evade", force);
			}
		}
	}

	if (_steering->is_on(_steering->_arrival_activate))
	{
		arrival_activate();
	}

	if (_steering->is_on(_steering->_arrival))
	{
		force = do_arrival(dt);
		_steering->accumulate_force("arrival", force);
	}

	if (_steering->is_on(_steering->_flock_activate))
	{
		flock_activate();
	}

	if (_steering->is_on(_steering->_flock))
	{
		if (_steering->_conflict)
		{
			force = do_flock() * _steering->_flock_weight;
		}
		else
		{
			force = do_flock();
		}
		_steering->accumulate_force("flock", force);
	}

	if (_steering->is_on(_steering->_wander))
	{
		if (_steering->_conflict)
		{
			force = do_wander() * _steering->_wander_obj->_wander_weight;
		}
		else
		{
			force = do_wander();
		}
		_steering->accumulate_force("wander", force);
	}

	if (_steering->is_on(_steering->_obstacle_avoidance_activate))
	{
		obstacle_avoidance_activate_bullet();
	}

	if (_steering->is_on(_steering->_obstacle_avoidance))
	{
		force = do_obstacle_avoidance_bullet();
		_steering->accumulate_force("obstacle_avoidance", force);
	}

	if (_steering->_path_follow_obj != NULL)
	{
		if (_steering->_path_follow_obj->_start)
		{
			do_follow();
		}
	}

	_steering->is_conflict();

	_steering->_steering_force += _steering->_seek_force
			* int(_steering->is_on(_steering->_seek))
			+ _steering->_flee_force * int(_steering->is_on(_steering->_flee))
			+ _steering->_pursue_force
					* int(_steering->is_on(_steering->_pursue))
			+ _steering->_evade_force * int(_steering->is_on(_steering->_evade))
			+ _steering->_flock_force * int(_steering->is_on(_steering->_flock))
			+ _steering->_wander_force
					* int(_steering->is_on(_steering->_wander))
			+ _steering->_obstacle_avoidance_force
					* int(_steering->is_on(_steering->_obstacle_avoidance));

	if (_steering->_steering_force.length()
			> _steering->_ai_char->get_max_force())
	{
		_steering->_steering_force.normalize();
		_steering->_steering_force = _steering->_steering_force
				* _steering->_ai_char->get_max_force();
	}

	if (_steering->is_on(_steering->_arrival))
	{
		if (_steering->_seek_obj != NULL)
		{
			LVecBase3f dirn = _steering->_steering_force;
			dirn.normalize();
			_steering->_steering_force = ((_steering->_steering_force.length()
					- _steering->_arrival_force.length()) * dirn);
		}

		if (_steering->_pursue_obj != NULL)
		{
			LVecBase3f dirn = _steering->_steering_force;
			dirn.normalize();
			_steering->_steering_force = ((_steering->_steering_force.length()
					- _steering->_arrival_force.length())
					* _steering->_arrival_obj->_arrival_direction);
		}
	}
	return _steering->_steering_force;
}

//helpers
//seek
LVecBase3f Steering::do_seek()
{
	Seek *_seek_obj = _steering->_seek_obj;
	double target_distance = (_seek_obj->_seek_position
			- _seek_obj->_ai_char->_ai_char_np.get_pos(
					_seek_obj->_ai_char->_window_render)).get_xy().length();

	if (int(target_distance) == 0)
	{
		_seek_obj->_seek_done = true;
		_seek_obj->_ai_char->_steering->_steering_force = LVecBase3f(0.0, 0.0,
				0.0);
		_seek_obj->_ai_char->_steering->turn_off("seek");
		return (LVecBase3f(0.0, 0.0, 0.0));
	}

	LVecBase3f desired_force = _seek_obj->_seek_direction
			* _seek_obj->_ai_char->_movt_force;
	return (desired_force);
}
//flee
void Steering::flee_activate()
{
	ListFlee::iterator& _flee_itr = _steering->_flee_itr;
	LVecBase3f dirn;
	double distance;

	_flee_itr->_flee_activate_done = false;

	dirn = (_flee_itr->_ai_char->_ai_char_np.get_pos(
			_flee_itr->_ai_char->_window_render) - _flee_itr->_flee_position);
	distance = dirn.get_xy().length();

	if (distance < _flee_itr->_flee_distance)
	{
		_flee_itr->_flee_direction = _flee_itr->_ai_char->_ai_char_np.get_pos(
				_flee_itr->_ai_char->_window_render)
				- _flee_itr->_flee_position;
		_flee_itr->_flee_direction.normalize();
		_flee_itr->_flee_present_pos = _flee_itr->_ai_char->_ai_char_np.get_pos(
				_flee_itr->_ai_char->_window_render);
		_flee_itr->_ai_char->_steering->turn_off("flee_activate");
		_flee_itr->_ai_char->_steering->turn_on("flee");
		_flee_itr->_flee_activate_done = true;
	}
}
LVecBase3f Steering::do_flee()
{
	ListFlee::iterator& _flee_itr = _steering->_flee_itr;
	LVecBase3f dirn;
	double distance;
	LVecBase3f desired_force;

	dirn = _flee_itr->_ai_char->_ai_char_np.get_pos(
			_flee_itr->_ai_char->_window_render) - _flee_itr->_flee_present_pos;
	distance = dirn.get_xy().length();
	desired_force = _flee_itr->_flee_direction
			* _flee_itr->_ai_char->_movt_force;

	if (distance
			> (_flee_itr->_flee_distance + _flee_itr->_flee_relax_distance))
	{
		if ((_flee_itr->_ai_char->_steering->_behaviors_flags
				| _flee_itr->_ai_char->_steering->_flee)
				== _flee_itr->_ai_char->_steering->_flee)
		{
			_flee_itr->_ai_char->_steering->_steering_force = LVecBase3f(0.0,
					0.0, 0.0);
		}
		_flee_itr->_flee_done = true;
		_flee_itr->_ai_char->_steering->turn_off("flee");
		_flee_itr->_ai_char->_steering->turn_on("flee_activate");
		return (LVecBase3f(0.0, 0.0, 0.0));
	}
	else
	{
		return (desired_force);
	}
}
//pursue
LVecBase3f Steering::do_pursue()
{
	Pursue *_pursue_obj = _steering->_pursue_obj;
	assert(_pursue_obj->_pursue_target && "pursue target not assigned");

	LVecBase3f present_pos = _pursue_obj->_ai_char->_ai_char_np.get_pos(
			_pursue_obj->_ai_char->_window_render);
	double target_distance =
			(_pursue_obj->_pursue_target.get_pos(
					_pursue_obj->_ai_char->_window_render) - present_pos).get_xy().length();

	if (int(target_distance) == 0)
	{
		_pursue_obj->_pursue_done = true;
		_pursue_obj->_ai_char->_steering->_steering_force = LVecBase3f(0.0, 0.0,
				0.0);
		_pursue_obj->_ai_char->_steering->_pursue_force = LVecBase3f(0.0, 0.0,
				0.0);
		return (LVecBase3f(0.0, 0.0, 0.0));
	}
	else
	{
		_pursue_obj->_pursue_done = false;
	}

	_pursue_obj->_pursue_direction = _pursue_obj->_pursue_target.get_pos(
			_pursue_obj->_ai_char->_window_render) - present_pos;
	_pursue_obj->_pursue_direction.normalize();

	LVecBase3f desired_force = _pursue_obj->_pursue_direction
			* _pursue_obj->_ai_char->_movt_force;
	return (desired_force);
}
//evade
void Steering::evade_activate()
{
	ListEvade::iterator& _evade_itr = _steering->_evade_itr;
	_evade_itr->_evade_direction = (_evade_itr->_ai_char->_ai_char_np.get_pos(
			_evade_itr->_ai_char->_window_render)
			- _evade_itr->_evade_target.get_pos(
					_evade_itr->_ai_char->_window_render));
	double distance = _evade_itr->_evade_direction.get_xy().length();
	_evade_itr->_evade_activate_done = false;

	if (distance < _evade_itr->_evade_distance)
	{
		_evade_itr->_ai_char->_steering->turn_off("evade_activate");
		_evade_itr->_ai_char->_steering->turn_on("evade");
		_evade_itr->_evade_activate_done = true;
	}
}
LVecBase3f Steering::do_evade()
{
	ListEvade::iterator& _evade_itr = _steering->_evade_itr;
	assert(_evade_itr->_evade_target && "evade target not assigned");

	_evade_itr->_evade_direction = _evade_itr->_ai_char->_ai_char_np.get_pos(
			_evade_itr->_ai_char->_window_render)
			- _evade_itr->_evade_target.get_pos(
					_evade_itr->_ai_char->_window_render);
	double distance = _evade_itr->_evade_direction.get_xy().length();

	_evade_itr->_evade_direction.normalize();
	LVecBase3f desired_force = _evade_itr->_evade_direction
			* _evade_itr->_ai_char->_movt_force;

	if (distance
			> (_evade_itr->_evade_distance + _evade_itr->_evade_relax_distance))
	{
		if ((_evade_itr->_ai_char->_steering->_behaviors_flags
				| _evade_itr->_ai_char->_steering->_evade)
				== _evade_itr->_ai_char->_steering->_evade)
		{
			_evade_itr->_ai_char->_steering->_steering_force = LVecBase3f(0.0,
					0.0, 0.0);
		}
		_evade_itr->_ai_char->_steering->turn_off("evade");
		_evade_itr->_ai_char->_steering->turn_on("evade_activate");
		_evade_itr->_evade_done = true;
		return (LVecBase3f(0.0, 0.0, 0.0));
	}
	else
	{
		_evade_itr->_evade_done = false;
		return (desired_force);
	}
}
//arrival
void Steering::arrival_activate()
{
	Arrival *_arrival_obj = _steering->_arrival_obj;
	LVecBase3f dirn;
	if (_arrival_obj->_arrival_type)
	{
		dirn =
				(_arrival_obj->_ai_char->_ai_char_np.get_pos(
						_arrival_obj->_ai_char->_window_render)
						- _arrival_obj->_ai_char->get_ai_behaviors()->_pursue_obj->_pursue_target.get_pos(
								_arrival_obj->_ai_char->_window_render));
	}
	else
	{
		dirn =
				(_arrival_obj->_ai_char->_ai_char_np.get_pos(
						_arrival_obj->_ai_char->_window_render)
						- _arrival_obj->_ai_char->get_ai_behaviors()->_seek_obj->_seek_position);
	}
	double distance = dirn.get_xy().length();

	if (distance < _arrival_obj->_arrival_distance
			&& _arrival_obj->_ai_char->_steering->_steering_force.length() > 0)
	{
		_arrival_obj->_ai_char->_steering->turn_off("arrival_activate");
		_arrival_obj->_ai_char->_steering->turn_on("arrival");

		if (_arrival_obj->_ai_char->_steering->is_on(
				_arrival_obj->_ai_char->_steering->_seek))
		{
			_arrival_obj->_ai_char->_steering->turn_off("seek");
		}

		if (_arrival_obj->_ai_char->_steering->is_on(
				_arrival_obj->_ai_char->_steering->_pursue))
		{
			_arrival_obj->_ai_char->_steering->pause_ai("pursue");
		}
	}
}
LVecBase3f Steering::do_arrival(float dt)
{
	Arrival *_arrival_obj = _steering->_arrival_obj;
	LVecBase3f direction_to_target;
	double distance;

	if (_arrival_obj->_arrival_type)
	{
		direction_to_target =
				_arrival_obj->_ai_char->get_ai_behaviors()->_pursue_obj->_pursue_target.get_pos(
						_arrival_obj->_ai_char->_window_render)
						- _arrival_obj->_ai_char->_ai_char_np.get_pos(
								_arrival_obj->_ai_char->_window_render);
	}
	else
	{
		direction_to_target =
				_arrival_obj->_ai_char->get_ai_behaviors()->_seek_obj->_seek_position
						- _arrival_obj->_ai_char->_ai_char_np.get_pos(
								_arrival_obj->_ai_char->_window_render);
	}
	distance = direction_to_target.get_xy().length();

	_arrival_obj->_arrival_direction = direction_to_target;
	_arrival_obj->_arrival_direction.normalize();

	if (int(distance) == 0)
	{
		_arrival_obj->_ai_char->_steering->_steering_force = LVecBase3f(0.0,
				0.0, 0.0);
		_arrival_obj->_ai_char->_steering->_arrival_force = LVecBase3f(0.0, 0.0,
				0.0);

		if (_arrival_obj->_ai_char->_steering->_seek_obj != NULL)
		{
			_arrival_obj->_ai_char->_steering->turn_off("arrival");
			_arrival_obj->_ai_char->_steering->turn_on("arrival_activate");
		}
		_arrival_obj->_arrival_done = true;
		return (LVecBase3f(0.0, 0.0, 0.0));
	}
	else
	{
		_arrival_obj->_arrival_done = false;
	}

	double u = _arrival_obj->_ai_char->get_velocity().length() * dt;
	LVecBase3f desired_force = ((u * u) / (2 * distance))
			* _arrival_obj->_ai_char->get_mass();

	if (_arrival_obj->_ai_char->_steering->_seek_obj != NULL)
	{
		return (desired_force);
	}

	if (_arrival_obj->_ai_char->_steering->_pursue_obj != NULL)
	{

		if (distance > _arrival_obj->_arrival_distance)
		{
			_arrival_obj->_ai_char->_steering->turn_off("arrival");
			_arrival_obj->_ai_char->_steering->turn_on("arrival_activate");
			_arrival_obj->_ai_char->_steering->resume_ai("pursue");
		}

		return (desired_force);
	}

	cout << "Arrival works only with seek and pursue" << endl;
	return (LVecBase3f(0.0, 0.0, 0.0));
}
//flock
void Steering::flock_activate()
{
	if (_steering->is_on(_steering->_seek) || _steering->is_on(_steering->_flee)
			|| _steering->is_on(_steering->_pursue)
			|| _steering->is_on(_steering->_evade)
			|| _steering->is_on(_steering->_wander))
	{
		_steering->turn_off("flock_activate");
		_steering->turn_on("flock");
	}
}
LVecBase3f Steering::do_flock()
{
	//! Initialize variables required to compute the flocking force on the ai char.
	unsigned int neighbor_count = 0;
	LVecBase3f separation_force = LVecBase3f(0.0, 0.0, 0.0);
	LVecBase3f alignment_force = LVecBase3f(0.0, 0.0, 0.0);
	LVecBase3f cohesion_force = LVecBase3f(0.0, 0.0, 0.0);
	LVecBase3f avg_neighbor_heading = LVecBase3f(0.0, 0.0, 0.0);
	LVecBase3f total_neighbor_heading = LVecBase3f(0.0, 0.0, 0.0);
	LVecBase3f avg_center_of_mass = LVecBase3f(0.0, 0.0, 0.0);
	LVecBase3f total_center_of_mass = LVecBase3f(0.0, 0.0, 0.0);

	//! Loop through all the other AI units in the flock to check if they are neigbours.
	for (unsigned int i = 0; i < _steering->_flock_group->_ai_char_list.size();
			i++)
	{
		if (_steering->_flock_group->_ai_char_list[i]->_name
				!= _steering->_ai_char->_name)
		{

			//! Using visibilty cone to detect neighbors.
			LVecBase3f dist_vect =
					_steering->_flock_group->_ai_char_list[i]->_ai_char_np.get_pos()
							- _steering->_ai_char->_ai_char_np.get_pos();
			LVecBase3f ai_char_heading = _steering->_ai_char->get_velocity();
			ai_char_heading.normalize();

			//! Check if the current unit is a neighbor.
			if ((dist_vect.get_xy().dot(ai_char_heading.get_xy())
					> ((dist_vect.get_xy().length())
							* (ai_char_heading.get_xy().length())
							* cos(
									_steering->_flock_group->_flock_vcone_angle
											* (PHYSICS_PI / 180))))
					&& (dist_vect.get_xy().length()
							< _steering->_flock_group->_flock_vcone_radius))
			{
				//! Separation force calculation.
				LVecBase3f ai_char_to_units =
						_steering->_ai_char->_ai_char_np.get_pos()
								- _steering->_flock_group->_ai_char_list[i]->_ai_char_np.get_pos();
				float to_units_dist = ai_char_to_units.length();
				ai_char_to_units.normalize();
				separation_force += (ai_char_to_units / to_units_dist);

				//! Calculating the total heading and center of mass of all the neighbors.
				LVecBase3f neighbor_heading =
						_steering->_flock_group->_ai_char_list[i]->get_velocity();
				neighbor_heading.normalize();
				total_neighbor_heading += neighbor_heading;
				total_center_of_mass +=
						_steering->_flock_group->_ai_char_list[i]->_ai_char_np.get_pos();

				//! Update the neighbor count.
				++neighbor_count;
			}
		}
	}

	if (neighbor_count > 0)
	{
		//! Alignment force calculation
		avg_neighbor_heading = total_neighbor_heading / neighbor_count;
		LVector3f ai_char_heading = _steering->_ai_char->get_velocity();
		ai_char_heading.normalize();
		avg_neighbor_heading -= ai_char_heading;
		avg_neighbor_heading.normalize();
		alignment_force = avg_neighbor_heading;

		//! Cohesion force calculation
		avg_center_of_mass = total_center_of_mass / neighbor_count;
		LVecBase3f cohesion_dir = avg_center_of_mass
				- _steering->_ai_char->_ai_char_np.get_pos();
		cohesion_dir.normalize();
		cohesion_force = cohesion_dir * _steering->_ai_char->_movt_force;
	}
	else if (_steering->is_on(_steering->_seek)
			|| _steering->is_on(_steering->_flee)
			|| _steering->is_on(_steering->_pursue)
			|| _steering->is_on(_steering->_evade)
			|| _steering->is_on(_steering->_wander))
	{
		_steering->_flock_done = true;
		_steering->turn_off("flock");
		_steering->turn_on("flock_activate");
		return (LVecBase3f(0.0, 0.0, 0.0));
	}

	//! Calculate the resultant force on the ai character by taking into account the separation, alignment and cohesion
	//! forces along with their corresponding weights.
	return (separation_force * _steering->_flock_group->_separation_wt
			+ avg_neighbor_heading * _steering->_flock_group->_alignment_wt
			+ cohesion_force * _steering->_flock_group->_cohesion_wt);
}
} //ely

namespace
{
	//wander
	double rand_float()
	{
		const static double rand_max = 0x7fff;
		return ((rand()) / (rand_max + 1.0));
	}
	double random_clamped()
	{
		return (rand_float() - rand_float());
	}
}

namespace ely
{
LVecBase3f Steering::do_wander()
{
	Wander * _wander_obj = _steering->_wander_obj;
	LVecBase3f present_pos = _wander_obj->_ai_char->get_node_path().get_pos(
			_wander_obj->_ai_char->get_char_render());
	// Create the random slices to enable random movement of wander for x,y,z respectively
	double time_slice_1 = random_clamped() * 1.5;
	double time_slice_2 = random_clamped() * 1.5;
	double time_slice_3 = random_clamped() * 1.5;
	switch (_wander_obj->_flag)
	{
	case 0:
	{
		_wander_obj->_wander_target += LVecBase3f(time_slice_1, time_slice_2,
				0);
		break;
	}
	case 1:
	{
		_wander_obj->_wander_target += LVecBase3f(0, time_slice_1,
				time_slice_2);
		break;
	}
	case 2:
	{
		_wander_obj->_wander_target += LVecBase3f(time_slice_1, 0,
				time_slice_2);
		break;
	}
	case 3:
	{
		_wander_obj->_wander_target += LVecBase3f(time_slice_1, time_slice_2,
				time_slice_3);
		break;
	}

	default:
	{
		_wander_obj->_wander_target = LVecBase3f(time_slice_1, time_slice_2, 0);
		break;
	}
	}
	_wander_obj->_wander_target.normalize();
	_wander_obj->_wander_target *= _wander_obj->_wander_radius;
	LVecBase3f target =
			_wander_obj->_ai_char->get_char_render().get_relative_vector(
					_wander_obj->_ai_char->get_node_path(),
					LVector3f::forward());
	target.normalize();
	// Project wander target onto global space
	target = _wander_obj->_wander_target + target;
	LVecBase3f desired_target = present_pos + target;
	LVecBase3f desired_force = desired_target
			- _wander_obj->_ai_char->get_node_path().get_pos();
	desired_force.normalize();
	desired_force *= _wander_obj->_ai_char->_movt_force;
	double distance = (present_pos - _wander_obj->_init_pos).get_xy().length();
	if (_wander_obj->_area_of_effect > 0
			&& distance > _wander_obj->_area_of_effect)
	{
		LVecBase3f direction = present_pos - _wander_obj->_init_pos;
		direction.normalize();
		desired_force = -direction * _wander_obj->_ai_char->_movt_force;
		LVecBase3f dirn = _wander_obj->_ai_char->_steering->_steering_force;
		dirn.normalize();
		_wander_obj->_ai_char->_steering->_steering_force = LVecBase3f(0.0, 0.0,
				0.0);
	}
	return desired_force;
}
//obstacle avoidance
void Steering::obstacle_avoidance_activate_bullet()
{
	ObstacleAvoidance *_obstacle_avoidance_obj =
			_steering->_obstacle_avoidance_obj;
	//ray cast direction (already normalized)
	LVecBase3f forwardDirection =
			_obstacle_avoidance_obj->_ai_char->get_char_render().get_relative_vector(
					_obstacle_avoidance_obj->_ai_char->get_node_path(),
					-LVector3f::forward());
	//from_pos = _ai_char pos
	LPoint3f fromPos =
			_obstacle_avoidance_obj->_ai_char->get_char_render().get_relative_point(
					_obstacle_avoidance_obj->_ai_char->get_node_path(),
					LPoint3f::zero());
	//to_pos = very far from _ai_char pos along forwardDirection
	LPoint3f toPos = fromPos + forwardDirection * PHYSICS_MAX_DISTANCE;
	//detect obstacles
	BulletClosestHitRayResult result = mWorld->ray_test_closest(fromPos, toPos,
			mObstacleHitMask);
	//check if hit along forwardDirection and < mObstacleMaxDist (plane) distance
	if (result.has_hit()
			and ((result.get_hit_pos() - fromPos).get_xy().length_squared()
					< mObstacleMaxDistSquared))
	{
		//save hit normal and hit obstacle
		mOldHitNormal = result.get_hit_normal();
		mHitObstacle = NodePath(const_cast<PandaNode*>(result.get_node()));
		_obstacle_avoidance_obj->_ai_char->_steering->turn_off(
				"obstacle_avoidance_activate");
		_obstacle_avoidance_obj->_ai_char->_steering->turn_on(
				"obstacle_avoidance");
	}
}
LVecBase3f Steering::do_obstacle_avoidance_bullet()
{
	ObstacleAvoidance *_obstacle_avoidance_obj =
			_steering->_obstacle_avoidance_obj;
	//from_pos = _ai_char pos
	LPoint3f fromPos =
			_obstacle_avoidance_obj->_ai_char->get_char_render().get_relative_point(
					_obstacle_avoidance_obj->_ai_char->get_node_path(),
					LPoint3f::zero());
	//to_pos = very far from _ai_char pos along -mOldHitNormal
	LPoint3f toPos = fromPos - mOldHitNormal * PHYSICS_MAX_DISTANCE;
	//detect obstacles
	BulletClosestHitRayResult result = mWorld->ray_test_closest(fromPos, toPos,
			mObstacleHitMask);
	//check ray first along -mOldHitNormal if hit and < mObstacleMaxDist (plane) distance
	if (result.has_hit()
			and ((result.get_hit_pos() - fromPos).get_xy().length_squared()
					< mObstacleMaxDistSquared))
	{
		//there is an obstacle to avoid
		//get normal hit direction (already normalized)
		mOldHitNormal = result.get_hit_normal();
		//forward direction (already normalized)
		LVecBase3f forwardDirection =
				_obstacle_avoidance_obj->_ai_char->get_char_render().get_relative_vector(
						_obstacle_avoidance_obj->_ai_char->get_node_path(),
						-LVector3f::forward());
		//get the forwardDirection normal component
		LVector3f normalComponent = mOldHitNormal
				* forwardDirection.dot(mOldHitNormal);
		//calculate avoidance by reverting the forwardDirection normal component
		LVecBase3f avoidance = (forwardDirection - 2 * normalComponent)
				* (_obstacle_avoidance_obj->_ai_char->get_max_force()
						* _obstacle_avoidance_obj->_ai_char->_movt_force);
		return avoidance;
	}
	else
	{
		//we are near a corner: check ray with hit obstacle
		LPoint3f toPos =
				_obstacle_avoidance_obj->_ai_char->get_char_render().get_relative_point(
						mHitObstacle, LPoint3f::zero());
		//detect obstacles
		BulletClosestHitRayResult result = mWorld->ray_test_closest(fromPos,
				toPos, mObstacleHitMask);
		//check ray first along hit obstacle if hit and < mObstacleMaxDist (plane) distance
		if (result.has_hit()
				and ((result.get_hit_pos() - fromPos).get_xy().length_squared()
						< mObstacleMaxDistSquared))
		{
			//there is an obstacle to avoid
			//get normal hit direction (already normalized)
			mOldHitNormal = result.get_hit_normal();
			//forward direction (already normalized)
			LVecBase3f forwardDirection =
					_obstacle_avoidance_obj->_ai_char->get_char_render().get_relative_vector(
							_obstacle_avoidance_obj->_ai_char->get_node_path(),
							-LVector3f::forward());
			//get the forwardDirection normal component
			LVector3f normalComponent = mOldHitNormal
					* forwardDirection.dot(mOldHitNormal);
			//calculate avoidance by reverting the forwardDirection normal component
			LVecBase3f avoidance = (forwardDirection - 2 * normalComponent)
					* (mObstacleMaxDist
							* _obstacle_avoidance_obj->_ai_char->get_max_force()
							* _obstacle_avoidance_obj->_ai_char->_movt_force);
			return avoidance;
		}
	}
	_obstacle_avoidance_obj->_ai_char->_steering->turn_on(
			"obstacle_avoidance_activate");
	_obstacle_avoidance_obj->_ai_char->_steering->turn_off(
			"obstacle_avoidance");
	return LVecBase3f(0, 0, 0);
}
//follow
void Steering::do_follow()
{
	PathFollow *_path_follow_obj = _steering->_path_follow_obj;
	if ((_path_follow_obj->_myClock->get_real_time() - _path_follow_obj->_time)
			> 0.5)
	{
		if (_path_follow_obj->_type == "pathfind")
		{
			// This 'if' statement when 'true' causes the path to be re-calculated irrespective of target position.
			// This is done when _dynamice_avoid is active. More computationally expensive.
			if (_path_follow_obj->_ai_char->_steering->_path_find_obj->_dynamic_avoid)
			{
				_path_follow_obj->_ai_char->_steering->_path_find_obj->do_dynamic_avoid();
				if (_path_follow_obj->check_if_possible())
				{
					_path_follow_obj->_path.clear();
					_path_follow_obj->_ai_char->_steering->_path_find_obj->path_find(
							_path_follow_obj->_ai_char->_steering->_path_find_obj->_path_find_target);
					// Ensure that the path size is not 0.
					if (_path_follow_obj->_path.size() > 0)
					{
						_path_follow_obj->_curr_path_waypoint =
								_path_follow_obj->_path.size() - 1;
						_path_follow_obj->_dummy.set_pos(
								_path_follow_obj->_path[_path_follow_obj->_curr_path_waypoint]);
					}
					else
					{
						// Refresh the _curr_path_waypoint value if path size is <= 0.
						_path_follow_obj->_curr_path_waypoint = -1;
					}
				}
			}
			// This 'if' statement causes the path to be re-calculated only when there is a change in target position.
			// Less computationally expensive.
			else if (_path_follow_obj->_ai_char->_steering->_path_find_obj->_path_find_target.get_pos(
					_path_follow_obj->_ai_char->_window_render)
					!= _path_follow_obj->_ai_char->_steering->_path_find_obj->_prev_position)
			{
				if (_path_follow_obj->check_if_possible())
				{
					_path_follow_obj->_path.clear();
					_path_follow_obj->_ai_char->_steering->_path_find_obj->path_find(
							_path_follow_obj->_ai_char->_steering->_path_find_obj->_path_find_target);
					// Ensure that the path size is not 0.
					if (_path_follow_obj->_path.size() > 0)
					{
						_path_follow_obj->_curr_path_waypoint =
								_path_follow_obj->_path.size() - 1;
						_path_follow_obj->_dummy.set_pos(
								_path_follow_obj->_path[_path_follow_obj->_curr_path_waypoint]);
					}
					else
					{
						// Refresh the _curr_path_waypoint value if path size is 0.
						_path_follow_obj->_curr_path_waypoint = -1;
					}
				}
			}
			_path_follow_obj->_time =
					_path_follow_obj->_myClock->get_real_time();
		}
	}

	if (_path_follow_obj->_curr_path_waypoint > 0)
	{
		double distance =
				(_path_follow_obj->_path[_path_follow_obj->_curr_path_waypoint]
						- _path_follow_obj->_ai_char->_ai_char_np.get_pos(
								_path_follow_obj->_ai_char->_window_render)).get_xy().length();
		if (distance < 5)
		{
			_path_follow_obj->_curr_path_waypoint--;
			_path_follow_obj->_dummy.set_pos(
					_path_follow_obj->_path[_path_follow_obj->_curr_path_waypoint]);
		}
	}
}

//TypedObject semantics: hardcoded
TypeHandle Steering::_type_handle;

}  // namespace ely
