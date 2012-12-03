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
 * \file /Ely/include/ControlComponents/Chaser.h
 *
 * \date 11/nov/2012 (09:45:00)
 * \author marco
 */

#ifndef CHASER_H_
#define CHASER_H_

#include <cmath>
#include <nodePath.h>
#include <lvector3.h>
#include <lpoint3.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GamePhysicsManager.h"
#include "Utilities/Tools.h"

class ChaserTemplate;

/**
 * \brief Component designed to make an object a chaser of another object.
 *
 * The up axis is the "z" axis.
 *
 * XML Param(s):
 * - "enabled"  			|single|"true"
 * - "backward"				|single|"true"
 * - "chased_object"		|single|no default
 * - "reference_object"		|single|no default
 * - "distance"				|single|no default
 * - "abs_lookat_distance"	|single|no default
 * - "abs_min_distance"		|single|no default
 * - "abs_min_height"		|single|no default
 * - "friction"				|single|"1.0"
 */
class Chaser: public Component
{
public:
	Chaser();
	Chaser(SMARTPTR(ChaserTemplate)tmpl);
	virtual ~Chaser();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * \see OgreBulletDemos.
	 *
	 * Will be called automatically by an control manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

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
	 * \name Getting/setting parameters.
	 */
	///@{
	float getAbsLookAtDistance() const;
	void setAbsLookAtDistance(float absLookAtDistance);
	float getAbsMinDistance() const;
	void setAbsMinDistance(float absMinDistance);
	float getAbsMinHeight() const;
	void setAbsMinHeight(float absMinHeight);
	float getDistance() const;
	void setDistance(float distance);
	///@}

private:
	///The chased object's node path.
	NodePath mChasedNodePath;
	///The reference object's node path.
	NodePath mReferenceNodePath;
	///Enabling & backward flags.
	bool mEnabled, mIsEnabled, mBackward;
	///Kinematic parameters.
	float mAbsLookAtDistance, mDistance, mAbsMinDistance, mAbsMinHeight, mFriction;
	///Positions.
	LPoint3f mChaserPosition, mLookAtPosition;
	/**
	 * \brief Calculates the dynamic position of the chaser.
	 * \see OgreBulletDemos.
	 * @param desiredChaserPos The desired chaser position (wrt reference).
	 * @param actualChaserPos The current chaser position (wrt reference).
	 * @param deltaTime The delta time update.
	 * @return The dynamic chaser position.
	 */
	LPoint3f getChaserPos(LPoint3f desiredChaserPos,
			LPoint3f actualChaserPos, float deltaTime);

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Chaser", Component::get_class_type());
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

#endif /* CHASER_H_ */
