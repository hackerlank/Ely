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
 * \file /Ely/include/AIComponents/Steering.h
 *
 * \date 03/dic/2012 (13:39:59)
 * \author consultit
 */

#ifndef STEERING_H_
#define STEERING_H_

#include <aiCharacter.h>
#include "PhysicsControlComponents/CharacterController.h"
#include "ObjectModel/Component.h"

namespace ely
{
class SteeringTemplate;

class AICharacterRef;

/**
 * \brief Component implementing AI Steering Behaviors and Path Finding.
 *
 * This is a tiny wrapper around an AICharacter object: any AI steering
 * behavior should be done by getting a reference to it, after having
 * enabled the whole component.\n
 * Steering behaviors' initializations can be done as usual into the
 * libraries' routines.\n
 * This component can throw 2 events (if enabled): "SteeringForceOn" and
 * "SteeringForceOff" on transitions when it begins its steering behavior
 * end when it ends it. These events have these parameters (in order):
 * - a pointer to this component,
 * - the owner object name of this component.\n
 * This component can throw (if enabled) "OnSteeringForceOn" and "OnSteeringForceOff"
 * events.
 *
 * \note This component should be used only with an object reparented to
 * the root scene node path (i.e. render).
 * \note For obstacle avoidance, obstacle collision objects (rigid bodies)
 * should have the MSB bit set.
 *
 * XML Param(s):
 * - "enabled"  						|single|"true"
 * - "throw_events"						|single|"false"
 * - "controlled_type"					|single|"nodepath" (values: nodepath|character_controller)
 * - "mass"  							|single|"1.0"
 * - "movt_force"  						|single|"1.0"
 * - "max_force"  						|single|"1.0"
 * - "obstacle_hit_mask"				|single|"0x80000000"
 * - "obstacle_max_distance_fraction"	|single|"1.0"
 *
 * \note parts inside [] are optional.\n
 */
class Steering: public Component
{
protected:
	friend class SteeringTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Steering();
	Steering(SMARTPTR(SteeringTemplate)tmpl);
	virtual ~Steering();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an ai manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	struct Result: public Component::Result
	{
		Result(int value):Component::Result(value)
		{
		}
		enum
		{
#ifdef ELY_THREAD
			STEERING_DISABLING = COMPONENT_RESULT_END + 1,
			STEERING_RESULT_END
#endif
		};
	};

	/**
	 * \name Enabling/disabling.
	 * \brief Enables/disables this component.
	 */
	///@{
	Result enable();
	Result disable();
	bool isEnabled();
	///@}

	/**
	 * \brief Returns a reference to the underlined AICharacter.
	 * @return A reference to the underlined AICharacter.
	 */
	SMARTPTR(AICharacterRef) getAiCharacter() const;

private:
	///The AICharacter associated to this Steering.
	SMARTPTR(AICharacterRef) mAICharacter;
	///@{
	///Enabling flags.
	bool mStartEnabled, mEnabled;
#ifdef ELY_THREAD
	bool mDisabling;
#endif

	/**
	 * \name Actual enabling/disabling.
	 */
	///@{
	void doEnable();
	void doDisable();
	///@}

	///Fixed parameters.
	float mMass, mMovtForce, mMaxForce;
	std::string mTypeParam;
	///@}
	///@{
	///Steered character controller items.
	SMARTPTR(BulletWorld) mWorld;
	SMARTPTR(CharacterController) mCharacterController;
	void doEnableMovRot(bool enable);
	bool mMovRotEnabled, mCurrentIsLocal;
	AIBehaviors *_steering;
	LVecBase3f do_calculate_prioritized(float dt);
	LVecBase3f do_seek();
	void do_flee_activate();
	LVecBase3f do_flee();
	LVecBase3f do_pursue();
	void do_evade_activate();
	LVecBase3f do_evade();
	void do_arrival_activate();
	LVecBase3f do_arrival(float dt);
	void do_flock_activate();
	LVecBase3f do_flock();
	LVecBase3f do_wander();
	void do_obstacle_avoidance_activate_bullet();
	LVecBase3f do_obstacle_avoidance_bullet();
	void do_follow();

	///Obstacle avoidance.
	float mObstacleMaxDistanceFraction, mObstacleMaxDist, mObstacleMaxDistSquared;
	BitMask32 mObstacleHitMask;
	LVector3f mOldHitNormal;
	NodePath mHitObstacle;
	///@}

	/**
	 * \name The real update member functions.
	 */
	///@{
	void doUpdateNodePath(float dt);
	void doUpdateController(float dt);
	///The pointer to the real update member function.
	void (Steering::*mUpdatePtr)(float);
	///@}

	///Throwing events.
	bool mThrowEvents, mSteeringForceOnSent, mSteeringForceOffSent;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Steering", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

/**
 * \brief AICharacter class with possibility to be reference counted.
 */
class AICharacterRef: public AICharacter, public TypedWritableReferenceCount
{
public:
	AICharacterRef(const std::string& name, NodePath model_np,
			double mass, double movt_force, double max_force);

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedObject::init_type();
		register_type(_type_handle, "AICharacterRef",
				TypedObject::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;
};

///inline definitions

inline void Steering::reset()
{
	//
	mAICharacter.clear();
	mStartEnabled = mEnabled = false;
#ifdef ELY_THREAD
	mDisabling = false;
#endif
	mMass = mMovtForce = mMaxForce;
	mTypeParam.clear();
	mWorld.clear();
	mCharacterController.clear();
	mMovRotEnabled = mCurrentIsLocal = false;
	_steering = NULL;
	mObstacleMaxDistanceFraction = mObstacleMaxDist = mObstacleMaxDistSquared = 0.0;
	mObstacleHitMask = BitMask32::all_off();
	mOldHitNormal = LVector3f::zero();
	mHitObstacle = NodePath();
	mUpdatePtr = NULL;
	mThrowEvents = mSteeringForceOnSent = mSteeringForceOffSent = false;
}

inline bool Steering::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mEnabled;
}

inline SMARTPTR(AICharacterRef) Steering::getAiCharacter() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,NULL)

	return mAICharacter;
}

}  // namespace ely

#endif /* STEERING_H_ */
