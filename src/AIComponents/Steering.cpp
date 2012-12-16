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
 * \author marco
 */

#include "AIComponents/Steering.h"
#include "AIComponents/SteeringTemplate.h"

Steering::Steering()
{
	// TODO Auto-generated constructor stub
}

Steering::Steering(SMARTPTR(SteeringTemplate)tmpl):mIsEnabled(false)
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"Steering::Steering: invalid GameAIManager")
	mTmpl = tmpl;
	mAICharacter = NULL;
	mUpdatePtr = NULL;
	mCharacterController = NULL;
	mMovRotEnabled = false;
	mSteeringForceOnSent = false;
	mSteeringForceOffSent = true;
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
	floatParam = (float) atof(mTmpl->parameter(std::string("mass")).c_str());
	floatParam > 0.0 ? mMass = floatParam : mMass = 1.0;
	//movt_force
	floatParam = (float) atof(
			mTmpl->parameter(std::string("movt_force")).c_str());
	floatParam > 0.0 ? mMovtForce = floatParam : mMovtForce = 1.0;
	//max_force
	floatParam = (float) atof(
			mTmpl->parameter(std::string("max_force")).c_str());
	floatParam > 0.0 ? mMaxForce = floatParam : mMaxForce = 1.0;
	//the type of the updatable item
	mType = mTmpl->parameter(std::string("controlled_type"));
	//
	return result;
}

void Steering::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//setup event callbacks if any
	setupEvents();
}

void Steering::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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

	//create the AICharacter...
	mAICharacter = new AICharacter(std::string(mComponentId),
			mOwnerObject->getNodePath(), mMass, mMovtForce, mMaxForce);
	//...add it to the AIWorld
	GameAIManager::GetSingletonPtr()->aiWorld()->add_ai_char(mAICharacter);

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

	if (mType == std::string("character_controller"))
	{
		//disable movement/rotation
		enableMovRot(false);
		//restore current movement
		mCharacterController->setIsLocal(mCurrentIsLocal);
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
	AIBehaviors *_steering = mAICharacter->get_ai_behaviors();

	if (!_steering->is_off(_steering->_none))
	{
		//enable movement/rotation
		if (not mMovRotEnabled)
		{
			enableMovRot(true);
		}
		LVecBase3f steering_force = calculate_prioritized(_steering);
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

LVecBase3f Steering::calculate_prioritized(AIBehaviors *_steering)
{
	LVecBase3f force;
	if (_steering->is_on(_steering->_seek))
	{
		//<!--_seek_obj->do_seek rewritten
		Seek *_seek_obj = _steering->_seek_obj;
		LVecBase3f do_seek;
		{
			double target_distance =
					(_seek_obj->_seek_position
							- _seek_obj->_ai_char->_ai_char_np.get_pos(
									_seek_obj->_ai_char->_window_render)).get_xy().length();
			if (int(target_distance) == 0)
			{
				_seek_obj->_seek_done = true;
				_seek_obj->_ai_char->_steering->_steering_force = LVecBase3f(
						0.0, 0.0, 0.0);
				_seek_obj->_ai_char->_steering->turn_off("seek");
				do_seek = LVecBase3f(0.0, 0.0, 0.0);
			}
			else
			{
				LVecBase3f desired_force = _seek_obj->_seek_direction
						* _seek_obj->_ai_char->_movt_force;
				do_seek = desired_force;
			}
		}
		//_seek_obj->do_seek rewritten-->
		if (_steering->_conflict)
		{
			force = do_seek * _seek_obj->_seek_weight;
		}
		else
		{
			force = do_seek;
		}
		_steering->accumulate_force("seek", force);
	}

	if (_steering->is_on(_steering->_flee_activate))
	{
		for (_steering->_flee_itr = _steering->_flee_list.begin();
				_steering->_flee_itr != _steering->_flee_list.end();
				_steering->_flee_itr++)
		{
			//<!--_flee_itr->flee_activate rewritten
			ListFlee::iterator _flee_itr = _steering->_flee_itr;
			{
				_flee_itr->_flee_activate_done = false;
				LVecBase3f dirn = (_flee_itr->_ai_char->_ai_char_np.get_pos(
						_flee_itr->_ai_char->_window_render)
						- _flee_itr->_flee_position);
				double distance = dirn.get_xy().length();
				if (distance < _flee_itr->_flee_distance)
				{
					_flee_itr->_flee_direction =
							_flee_itr->_ai_char->_ai_char_np.get_pos(
									_flee_itr->_ai_char->_window_render)
									- _flee_itr->_flee_position;
					_flee_itr->_flee_direction.normalize();
					_flee_itr->_flee_present_pos =
							_flee_itr->_ai_char->_ai_char_np.get_pos(
									_flee_itr->_ai_char->_window_render);
					_flee_itr->_ai_char->_steering->turn_off("flee_activate");
					_flee_itr->_ai_char->_steering->turn_on("flee");
					_flee_itr->_flee_activate_done = true;
				}
			}
			//_flee_itr->flee_activate rewritten-->
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
				//<!--_flee_itr->do_flee rewritten
				ListFlee::iterator _flee_itr = _steering->_flee_itr;
				LVecBase3f do_flee;
				{
					LVecBase3f dirn = (_flee_itr->_ai_char->_ai_char_np.get_pos(
							_flee_itr->_ai_char->_window_render)
							- _flee_itr->_flee_present_pos);
					double distance = dirn.get_xy().length();
					LVecBase3f desired_force = _flee_itr->_flee_direction
							* _flee_itr->_ai_char->_movt_force;

					if (distance
							> (_flee_itr->_flee_distance
									+ _flee_itr->_flee_relax_distance))
					{
						if ((_flee_itr->_ai_char->_steering->_behaviors_flags
								| _flee_itr->_ai_char->_steering->_flee)
								== _flee_itr->_ai_char->_steering->_flee)
						{
							_flee_itr->_ai_char->_steering->_steering_force =
									LVecBase3f(0.0, 0.0, 0.0);
						}
						_flee_itr->_flee_done = true;
						_flee_itr->_ai_char->_steering->turn_off("flee");
						_flee_itr->_ai_char->_steering->turn_on(
								"flee_activate");
						do_flee = LVecBase3f(0.0, 0.0, 0.0);
					}
					else
					{
						do_flee = desired_force;
					}
				}
				//_flee_itr->do_flee rewritten-->

				if (_steering->_conflict)
				{
					force = do_flee * _steering->_flee_itr->_flee_weight;
				}
				else
				{
					force = do_flee;
				}
				_steering->accumulate_force("flee", force);
			}
		}
	}

	if (_steering->is_on(_steering->_pursue))
	{
		//<!--_pursue_obj->do_pursue rewritten
		Pursue *_pursue_obj = _steering->_pursue_obj;
		LVecBase3f do_pursue;
		{
			LVecBase3f present_pos = _pursue_obj->_ai_char->_ai_char_np.get_pos(
					_pursue_obj->_ai_char->_window_render);
			double target_distance =
					(_pursue_obj->_pursue_target.get_pos(
							_pursue_obj->_ai_char->_window_render) - present_pos).get_xy().length();

			if (int(target_distance) == 0)
			{
				_pursue_obj->_pursue_done = true;
				_pursue_obj->_ai_char->_steering->_steering_force = LVecBase3f(
						0.0, 0.0, 0.0);
				_pursue_obj->_ai_char->_steering->_pursue_force = LVecBase3f(
						0.0, 0.0, 0.0);
				do_pursue = LVecBase3f(0.0, 0.0, 0.0);
			}
			else
			{
				_pursue_obj->_pursue_done = false;

				_pursue_obj->_pursue_direction =
						_pursue_obj->_pursue_target.get_pos(
								_pursue_obj->_ai_char->_window_render)
								- present_pos;
				_pursue_obj->_pursue_direction.normalize();

				LVecBase3f desired_force = _pursue_obj->_pursue_direction
						* _pursue_obj->_ai_char->_movt_force;
				do_pursue = desired_force;
			}
		}
		//_pursue_obj->do_pursue rewritten-->

		if (_steering->_conflict)
		{
			force = do_pursue * _steering->_pursue_obj->_pursue_weight;
		}
		else
		{
			force = do_pursue;
		}
		_steering->accumulate_force("pursue", force);
	}

	if (_steering->is_on(_steering->_evade_activate))
	{
		for (_steering->_evade_itr = _steering->_evade_list.begin();
				_steering->_evade_itr != _steering->_evade_list.end();
				_steering->_evade_itr++)
		{
			//<!--_evade_itr->evade_activate rewritten
			ListEvade::iterator _evade_itr = _steering->_evade_itr;
			{
				_evade_itr->_evade_direction =
						(_evade_itr->_ai_char->_ai_char_np.get_pos(
								_evade_itr->_ai_char->_window_render)
								- _evade_itr->_evade_target.get_pos(
										_evade_itr->_ai_char->_window_render));
				double distance =
						_evade_itr->_evade_direction.get_xy().length();
				_evade_itr->_evade_activate_done = false;

				if (distance < _evade_itr->_evade_distance)
				{
					_evade_itr->_ai_char->_steering->turn_off("evade_activate");
					_evade_itr->_ai_char->_steering->turn_on("evade");
					_evade_itr->_evade_activate_done = true;
				}
			}
			//_evade_itr->evade_activate rewritten-->
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
				//<!--_evade_itr->do_evade rewritten
				ListEvade::iterator _evade_itr = _steering->_evade_itr;
				LVecBase3f do_evade;
				{
					_evade_itr->_evade_direction =
							(_evade_itr->_ai_char->_ai_char_np.get_pos(
									_evade_itr->_ai_char->_window_render)
									- _evade_itr->_evade_target.get_pos(
											_evade_itr->_ai_char->_window_render));
					double distance =
							(_evade_itr->_evade_direction).get_xy().length();

					_evade_itr->_evade_direction.normalize();
					LVecBase3f desired_force = _evade_itr->_evade_direction
							* _evade_itr->_ai_char->_movt_force;

					if (distance
							> (_evade_itr->_evade_distance
									+ _evade_itr->_evade_relax_distance))
					{
						if ((_evade_itr->_ai_char->_steering->_behaviors_flags
								| _evade_itr->_ai_char->_steering->_evade)
								== _evade_itr->_ai_char->_steering->_evade)
						{
							_evade_itr->_ai_char->_steering->_steering_force =
									LVecBase3f(0.0, 0.0, 0.0);
						}
						_evade_itr->_ai_char->_steering->turn_off("evade");
						_evade_itr->_ai_char->_steering->turn_on(
								"evade_activate");
						_evade_itr->_evade_done = true;
						do_evade = LVecBase3f(0.0, 0.0, 0.0);
					}
					else
					{
						_evade_itr->_evade_done = false;
						do_evade = desired_force;
					}
				}
				//_evade_itr->do_evade rewritten-->

				if (_steering->_conflict)
				{
					force = do_evade * (_evade_itr->_evade_weight);
				}
				else
				{
					force = do_evade;
				}
				_steering->accumulate_force("evade", force);
			}
		}
	}

	if (_steering->is_on(_steering->_arrival_activate))
	{
		//<!--_arrival_obj->arrival_activate rewritten
		Arrival *_arrival_obj = _steering->_arrival_obj;
		{
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
					&& _arrival_obj->_ai_char->_steering->_steering_force.length()
							> 0)
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
		//_arrival_obj->arrival_activate rewritten-->
	}

	if (_steering->is_on (_steering->_arrival))
	{
		//<!--_arrival_obj->do_arrival rewritten
		Arrival *_arrival_obj = _steering->_arrival_obj;
		LVecBase3f do_arrival;
		{
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
				_arrival_obj->_ai_char->_steering->_steering_force = LVecBase3f(
						0.0, 0.0, 0.0);
				_arrival_obj->_ai_char->_steering->_arrival_force = LVecBase3f(
						0.0, 0.0, 0.0);

				if (_arrival_obj->_ai_char->_steering->_seek_obj != NULL)
				{
					_arrival_obj->_ai_char->_steering->turn_off("arrival");
					_arrival_obj->_ai_char->_steering->turn_on(
							"arrival_activate");
				}
				_arrival_obj->_arrival_done = true;
				do_arrival = LVecBase3f(0.0, 0.0, 0.0);
			}
			else
			{
				_arrival_obj->_arrival_done = false;

				double u = _arrival_obj->_ai_char->get_velocity().length();
				LVecBase3f desired_force = ((u * u) / (2 * distance))
						* _arrival_obj->_ai_char->get_mass();

				if (_arrival_obj->_ai_char->_steering->_seek_obj != NULL)
				{
					do_arrival = desired_force;
				}
				else
				{
					if (_arrival_obj->_ai_char->_steering->_pursue_obj != NULL)
					{
						if (distance > _arrival_obj->_arrival_distance)
						{
							_arrival_obj->_ai_char->_steering->turn_off(
									"arrival");
							_arrival_obj->_ai_char->_steering->turn_on(
									"arrival_activate");
							_arrival_obj->_ai_char->_steering->resume_ai(
									"pursue");
						}
						do_arrival = desired_force;
					}
					else
					{
						cout << "Arrival works only with seek and pursue"
								<< endl;
						do_arrival = LVecBase3f(0.0, 0.0, 0.0);
					}
				}
			}
		}
		//_arrival_obj->do_arrival rewritten-->
		force = do_arrival;
		_steering->accumulate_force("arrival", force);
	}

//	if (is_on (_flock_activate))
//	{
//		flock_activate();
//	}
//
//	if (is_on (_flock))
//	{
//		if (_conflict)
//		{
//			force = do_flock() * _flock_weight;
//		}
//		else
//		{
//			force = do_flock();
//		}
//		accumulate_force("flock", force);
//	}
//
//	if (is_on (_wander))
//	{
//		if (_conflict)
//		{
//			force = _wander_obj->do_wander() * _wander_obj->_wander_weight;
//		}
//		else
//		{
//			force = _wander_obj->do_wander();
//		}
//		accumulate_force("wander", force);
//	}
//
//	if (is_on (_obstacle_avoidance_activate))
//	{
//		_obstacle_avoidance_obj->obstacle_avoidance_activate();
//	}
//
//	if (is_on (_obstacle_avoidance))
//	{
//		if (_conflict)
//		{
//			force = _obstacle_avoidance_obj->do_obstacle_avoidance();
//		}
//		else
//		{
//			force = _obstacle_avoidance_obj->do_obstacle_avoidance();
//		}
//		accumulate_force("obstacle_avoidance", force);
//	}

	if (_steering->_path_follow_obj != NULL)
	{
		if (_steering->_path_follow_obj->_start)
		{
			_steering->_path_follow_obj->do_follow();
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

//TypedObject semantics: hardcoded
TypeHandle Steering::_type_handle;
