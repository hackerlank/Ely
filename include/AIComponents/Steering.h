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
#include "PhysicsComponents/CharacterController.h"
#include "ObjectModel/Component.h"

namespace ely
{
class SteeringTemplate;

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
 * \note This component should be used only with an object reparented to
 * the root scene node path (i.e. render).
 * \note For obstacle avoidance, obstacle collision objects (rigid bodies)
 * should have the MSB bit set.
 *
 * XML Param(s):
 * - "enabled"  						|single|"true"
 * - "throw_events"						|single|"false"
 * - "controlled_type"					|single|"nodepath" (nodepath,character_controller)
 * - "mass"  							|single|"1.0"
 * - "movt_force"  						|single|"1.0"
 * - "max_force"  						|single|"1.0"
 * - "obstacle_hit_mask"				|single|"0x80000000"
 * - "obstacle_max_distance_fraction"	|single|"1.0"
 */
class Steering: public Component
{
protected:
	friend class Object;
	friend class SteeringTemplate;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

public:
	Steering();
	Steering(SMARTPTR(SteeringTemplate)tmpl);
	virtual ~Steering();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an control manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \name Gets the node path of the designed target object.
	 */
	///@{
	NodePath getTargetNodePath(const ObjectId& target);
	NodePath getTargetNodePath(CSMARTPTR(Object) target);
	///@}

	/**
	 * \name Enabling/disabling.
	 * \brief Enables/disables this component.
	 */
	///@{
	void enable();
	void disable();
	bool isEnabled();
	///@}

	/**
	 * \brief Returns a reference to the underlined AICharacter.
	 * @return A reference to the underlined AICharacter.
	 */
	AICharacter* const getAiCharacter() const;

private:
	///The AICharacter associated to this Steering.
	AICharacter* mAICharacter;
	///@{
	///Enabling flags.
	bool mEnabled, mIsEnabled;
	///Fixed parameters.
	float mMass, mMovtForce, mMaxForce;
	std::string mType;
	///@}
	///The pointer to the real update member function.
	void (Steering::*mUpdatePtr)(float);
	///@{
	///Steered character controller items.
	SMARTPTR(BulletWorld) mWorld;
	SMARTPTR(CharacterController) mCharacterController;
	void enableMovRot(bool enable);
	bool mMovRotEnabled, mCurrentIsLocal;
	AIBehaviors *_steering;
	LVecBase3f calculate_prioritized(float dt);
	LVecBase3f do_seek();
	void flee_activate();
	LVecBase3f do_flee();
	LVecBase3f do_pursue();
	void evade_activate();
	LVecBase3f do_evade();
	void arrival_activate();
	LVecBase3f do_arrival(float dt);
	void flock_activate();
	LVecBase3f do_flock();
	LVecBase3f do_wander();
	void obstacle_avoidance_activate_bullet();
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
	void updateNodePath(float dt);
	void updateController(float dt);
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
}  // namespace ely

#endif /* STEERING_H_ */
