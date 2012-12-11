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
	//set AICharacter parameters
	//behavior
	mBehavior = mTmpl->parameter(std::string("behavior"));
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
	//target params
	mTargetObject = ObjectId(mTmpl->parameter(std::string("target_object")));
	mTargetPoint.set_x(
			(float) atof(mTmpl->parameter(std::string("target_x")).c_str()));
	mTargetPoint.set_y(
			(float) atof(mTmpl->parameter(std::string("target_y")).c_str()));
	mTargetPoint.set_z(
			(float) atof(mTmpl->parameter(std::string("target_z")).c_str()));
	//seek
	//seek_wt
	mSeekWT = (float) atof(mTmpl->parameter(std::string("seek_wt")).c_str());
	//flee
	//panic_distance, relax_distance, flee_wt
	mPanicDistance = (float) atof(
			mTmpl->parameter(std::string("panic_distance")).c_str());
	mRelaxDistance = (float) atof(
			mTmpl->parameter(std::string("relax_distance")).c_str());
	mFleeWT = (float) atof(mTmpl->parameter(std::string("flee_wt")).c_str());
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

	//switch to the indicated behavior
	switchBehavior();

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

void Steering::switchBehavior()
{
	if (not mAICharacter)
	{
		return;
	}
	//switch to the indicated behavior
	if (mBehavior == std::string("seek"))
	{
		setupSeek();
	}
	else if (mBehavior == std::string("flee"))
	{
		setupFlee();
	}
	else if (mBehavior == std::string("pursue"))
	{
	}
	else if (mBehavior == std::string("evade"))
	{
	}
	else if (mBehavior == std::string("arrival"))
	{
	}
	else if (mBehavior == std::string("wander"))
	{
	}
	else if (mBehavior == std::string("flock"))
	{
	}
	else if (mBehavior == std::string("obstacle_avoidance"))
	{
	}
	else if (mBehavior == std::string("path_follow"))
	{
	}
}

void Steering::setBehavior(const std::string& behavior)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mBehavior = behavior;
}

void Steering::setTarget(const ObjectId& target)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mTargetObject = target;
}

void Steering::setTarget(LVecBase3f target)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mTargetPoint = target;
}

void Steering::setSeekWT(float seekWT)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mSeekWT = seekWT;
}

void Steering::setupSeek()
{
	//seek
	//check if there is an object this component has to seek;
	//that object is supposed to be already created, set up,
	//added to the scene and added to the created objects table;
	ObjectId targetObjectId = ObjectId(mTargetObject);
	SMARTPTR(Object)targetObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			targetObjectId);
	if (targetObject != NULL)
	{
		//the target is an object get a reference to its
		//AIBehaviors and set the seek target object
		mAICharacter->get_ai_behaviors()->seek(targetObject->getNodePath(),
				mSeekWT);
	}
	else
	{
		//otherwise this component has to seek a point;
		mAICharacter->get_ai_behaviors()->seek(mTargetPoint, mSeekWT);
	}
}

void Steering::setupFlee()
{
	//flee
	//check if there is an object this component has to flee;
	//that object is supposed to be already created, set up,
	//added to the scene and added to the created objects table;
	ObjectId targetObjectId = ObjectId(mTargetObject);
	SMARTPTR(Object)targetObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
			targetObjectId);
	if (targetObject != NULL)
	{
		//the target is an object get a reference to its
		//AIBehaviors and set the flee target object
		mAICharacter->get_ai_behaviors()->flee(targetObject->getNodePath(),
				mPanicDistance, mRelaxDistance, mFleeWT);
	}
	else
	{
		//otherwise this component has to flee a point;
		mAICharacter->get_ai_behaviors()->flee(mTargetPoint, mPanicDistance,
				mRelaxDistance, mFleeWT);
	}
}

void Steering::setupPursue()
{
}

void Steering::setupEvade()
{
}

void Steering::setupArrival()
{
}

void Steering::setupWander()
{
}

void Steering::setupFlock()
{
}

void Steering::setupObstacleAvoidance()
{
}

void Steering::setupPathFollow()
{
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
		LVecBase3f steering_force;
		steering_force = calculate_prioritized(_steering);
		LVecBase3f acceleration = steering_force / mAICharacter->get_mass();
		mAICharacter->_velocity = acceleration;
		LVecBase3f direction = _steering->_steering_force;
		direction.normalize();
		mCharacterController->setLinearSpeed(
				-mAICharacter->get_velocity().get_xy() / dt);
		if (steering_force.length() > 0)
		{
			//0 <= A <= 180.0
			//the heavier a character is the slower it turns
			float H = mAICharacter->get_node_path().get_h();
			float A = 57.295779513f * acos(direction.get_y());
			if (direction.get_x() <= 0)
			{
				mCharacterController->setAngularSpeed(
						-(H + (180 - A)) / (mMass * dt));
			}
			else
			{
				mCharacterController->setAngularSpeed(
						-(H - (180 - A)) / (mMass * dt));
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
		//
		if (mMovRotEnabled)
		{
			enableMovRot(false);
		}
	}
}

void Steering::enableMovRot(bool enable)
{
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
		//_seek_obj->do_seek rewritten
		Seek *_seek_obj = _steering->_seek_obj;
		LVecBase3f do_seek;
		{
			double target_distance =
					(_seek_obj->_seek_position.get_xy()
							- _seek_obj->_ai_char->_ai_char_np.get_pos(
									_seek_obj->_ai_char->_window_render).get_xy()).length();
			if (int(target_distance) == 0)
			{
				_seek_obj->_seek_done = true;
				_seek_obj->_ai_char->_steering->_steering_force = LVecBase3f(
						0.0, 0.0, 0.0);
				_seek_obj->_ai_char->_steering->turn_off("seek");
				do_seek = LVecBase3f(0.0, 0.0, 0.0);
			}
			LVecBase3f desired_force = _seek_obj->_seek_direction
					* _seek_obj->_ai_char->_movt_force;
			do_seek = desired_force;
		}
		//
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
			//TODO
			//_flee_itr->flee_activate rewritten
			void Flee::flee_activate() {
			  LVecBase3f dirn;
			  double distance;

			  _flee_activate_done = false;

			  dirn = (_ai_char->_ai_char_np.get_pos(_ai_char->_window_render) - _flee_position);
			  distance = dirn.length();

			  if(distance < _flee_distance) {
			      _flee_direction = _ai_char->_ai_char_np.get_pos(_ai_char->_window_render) - _flee_position;
			      _flee_direction.normalize();
			      _flee_present_pos = _ai_char->_ai_char_np.get_pos(_ai_char->_window_render);
			      _ai_char->_steering->turn_off("flee_activate");
			      _ai_char->_steering->turn_on("flee");
			      _flee_activate_done = true;
			  }
			}
			////////




			_steering->_flee_itr->flee_activate();
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


				//TODO
				//_flee_itr->do_flee rewritten
				ListFlee::iterator _flee_itr = _steering->_flee_itr;
				LVecBase3f Flee::do_flee() {
				  LVecBase3f dirn;
				  double distance;
				  LVecBase3f desired_force;

				  dirn = _ai_char->_ai_char_np.get_pos(_ai_char->_window_render) - _flee_present_pos;
				  distance = dirn.length();
				  desired_force = _flee_direction * _ai_char->_movt_force;

				  if(distance > (_flee_distance + _flee_relax_distance)) {
				    if((_ai_char->_steering->_behaviors_flags | _ai_char->_steering->_flee) == _ai_char->_steering->_flee) {
				        _ai_char->_steering->_steering_force = LVecBase3f(0.0, 0.0, 0.0);
				    }
				    _flee_done = true;
				    _ai_char->_steering->turn_off("flee");
				    _ai_char->_steering->turn_on("flee_activate");
				    return(LVecBase3f(0.0, 0.0, 0.0));
				  }
				  else {
				      return(desired_force);
				  }
				}
				//////////





				if (_steering->_conflict)
				{
					force = _steering->_flee_itr->do_flee()
							* _steering->_flee_itr->_flee_weight;
				}
				else
				{
					force = _steering->_flee_itr->do_flee();
				}
				_steering->accumulate_force("flee", force);
			}
		}
	}

//	if (is_on (_pursue))
//	{
//		if (_conflict)
//		{
//			force = _pursue_obj->do_pursue() * _pursue_obj->_pursue_weight;
//		}
//		else
//		{
//			force = _pursue_obj->do_pursue();
//		}
//		accumulate_force("pursue", force);
//	}
//
//	if (is_on (_evade_activate))
//	{
//		for (_evade_itr = _evade_list.begin(); _evade_itr != _evade_list.end();
//				_evade_itr++)
//		{
//			_evade_itr->evade_activate();
//		}
//	}
//
//	if (is_on (_evade))
//	{
//		for (_evade_itr = _evade_list.begin(); _evade_itr != _evade_list.end();
//				_evade_itr++)
//		{
//			if (_evade_itr->_evade_activate_done)
//			{
//				if (_conflict)
//				{
//					force = (_evade_itr->do_evade())
//							* (_evade_itr->_evade_weight);
//				}
//				else
//				{
//					force = _evade_itr->do_evade();
//				}
//				accumulate_force("evade", force);
//			}
//		}
//	}
//
//	if (is_on (_arrival_activate))
//	{
//		_arrival_obj->arrival_activate();
//	}
//
//	if (is_on (_arrival))
//	{
//		force = _arrival_obj->do_arrival();
//		accumulate_force("arrival", force);
//	}
//
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
