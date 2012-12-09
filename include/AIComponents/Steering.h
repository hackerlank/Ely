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
 * \author marco
 */

#ifndef STEERING_H_
#define STEERING_H_

#include <string>
#include <aiCharacter.h>
#include <seek.h>
#include <pathFollow.h>
#include <arrival.h>
#include <nodePath.h>
#include <lvecBase3.h>
#include <lvector3.h>
#include <cmath>
#include "ControlComponents/Driver.h"
#include "PhysicsComponents/CharacterController.h"
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Utilities/Tools.h"

class SteeringTemplate;

/**
 * \brief Component implementing AI Steering Behaviors and Path Finding.
 *
 * \note This component should be used only with an object reparented to
 * the root scene nodepath (i.e. render)
 * XML Param(s):
 * - "enabled"  			|single|"true"
 * - "controlled_type"		|single|"nodepath" (driver, nodepath,
 * 											character_controller)
 * - "behavior"				|single|"seek" (seek,flee,pursue,evade,arrival,
 * 											wander,flock,obstacle_avoidance,
 * 											path_follow)
 * - "mass"  				|single|"1.0"
 * - "movt_force"  			|single|"1.0"
 * - "max_force"  			|single|"1.0"
 * - "target_object"		|single|no default (seek)
 * - "target_x"				|single|no default (seek)
 * - "target_y"				|single|no default (seek)
 * - "target_z"				|single|no default (seek)
 * - "seek_wt"  			|single|"1.0" (seek)
 */
class Steering: public Component
{
public:
	Steering();
	Steering(SMARTPTR(SteeringTemplate)tmpl);
	virtual ~Steering();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an control manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \brief Switch the steering behavior.
	 * @param behavior The steering behavior: seek|flee|pursue|
	 * evade|arrival|wander|flock|obstacle_avoidance|path_follow
	 */
	void switchBehavior();

	/**
	 * \name Parameters' setters.
	 */
	///@{
	void setBehavior(const std::string& behavior);
	void setTarget(const ObjectId& target);
	void setTarget(LVecBase3f target);
	void setSeekWT(float seekWT);
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

private:
	///The AICharacter associated to this Steering.
	AICharacter* mAICharacter;
	///@{
	///Enabling flags.
	bool mEnabled, mIsEnabled;
	///Fixed parameters.
	float mMass, mMovtForce, mMaxForce;
	std::string mType;
	///Variable parameters.
	std::string mBehavior;
	ObjectId mTargetObject;
	LVecBase3f mTargetPoint;
	//seek
	float mSeekWT;
	///@}
	///The pointer to the real update member function.
	void (Steering::*mUpdatePtr)(float);
	///@{
	///Controlled items.
	enum ControllerType
	{
		CHARACTER_CONTROLLER,
		DRIVER,
	};
	ControllerType mControllerType;
	SMARTPTR(CharacterController) mCharacterController;
	SMARTPTR(Driver) mDriver;
	LVecBase3f calculate_prioritized(AIBehaviors *_steering);
	void enableMovRot(bool enable);
	bool mMovRotEnabled;
	///Used only with character controller.
	bool mCurrentIsLocal;
	///Used only with driver.
	bool mCurrentEnabled;
	///@}

	/**
	 * \name The real update member functions.
	 */
	///@{
	void updateNodePath(float dt);
	void updateController(float dt);
	///@}

	/**
	 * \name The setup behaviors member functions.
	 */
	///@{
	void setupSeek();
	void setupFlee();
	void setupPursue();
	void setupEvade();
	void setupArrival();
	void setupWander();
	void setupFlock();
	void setupObstacleAvoidance();
	void setupPathFollow();
	///@}

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

#endif /* STEERING_H_ */
