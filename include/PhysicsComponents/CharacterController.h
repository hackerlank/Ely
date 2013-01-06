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
 * \file /Ely/include/PhysicsComponents/CharacterController.h
 *
 * \date 30/ott/2012 (17:03:49)
 * \author marco
 */

#ifndef CHARACTERCONTROLLER_H_
#define CHARACTERCONTROLLER_H_

#include <string>
#include <cstdlib>
#include <cmath>
#include <nodePath.h>
#include <lvecBase3.h>
#include <lvecBase2.h>
#include <lvector3.h>
#include <lpoint3.h>
#include <bulletShape.h>
#include <bulletCharacterControllerNode.h>
#include <bullet_utils.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GamePhysicsManager.h"
#include "Utilities/Tools.h"

class CharacterControllerTemplate;

/**
 * \brief Component representing a character controller attached to an object.
 *
 * The control is accomplished through physics.\n
 * It constructs a character controller with the single specified collision
 * shape_type along with relevant parameters.\n
 * The up axis is the Z axis.\n
 *
 * XML Param(s):
 * - "step_height"  			|single|"1.0"
 * - "collide_mask"  			|single|"all_on"
 * - "shape_type"  				|single|"sphere"
 * - "shape_size"  				|single|"medium"  (min, medium, max)
 * - "use_shape_of"				|single|no default
 * - "shape_radius"  			|single|no default (sphere,cylinder,capsule,cone)
 * - "shape_height"  			|single|no default (cylinder,capsule,cone)
 * - "shape_up"  				|single|no default (cylinder,capsule,cone)
 * - "shape_half_x"  			|single|no default (box)
 * - "shape_half_y"  			|single|no default (box)
 * - "shape_half_z"  			|single|no default (box)
 * - "fall_speed"  				|single|"55.0"
 * - "gravity"  				|single|"29.4" (3G)
 * - "jump_speed"  				|single|"10.0"
 * - "max_slope"  				|single|"45.0" (degrees)
 * - "max_jump_height"  		|single|no default
 * - "forward"  				|single|"enabled"
 * - "backward"  				|single|"enabled"
 * - "roll_left"  				|single|"enabled"
 * - "roll_right"  				|single|"enabled"
 * - "strafe_left"  			|single|"enabled"
 * - "strafe_right"  			|single|"enabled"
 * - "jump"  					|single|"enabled"
 * - "linear_speed"  			|single|"10.0"
 * - "angular_speed"  			|single|"45.0"
 * - "is_local"  				|single|"true"
 */
class CharacterController: public Component
{
public:
	CharacterController();
	CharacterController(SMARTPTR(CharacterControllerTemplate)tmpl);
	virtual ~CharacterController();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an control manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \name Control keys' enablers.
	 *
	 * These routines should be typically called by event handlers
	 * or AI algorithms, but this is not strictly required.
	 */
	///@{
	void enableForward(bool enable);
	bool isForwardEnabled();
	void enableBackward(bool enable);
	bool isBackwardEnabled();
	void enableStrafeLeft(bool enable);
	bool isStrafeLeftEnabled();
	void enableStrafeRight(bool enable);
	bool isStrafeRightEnabled();
	void enableRollLeft(bool enable);
	bool isRollLeftEnabled();
	void enableRollRight(bool enable);
	bool isRollRightEnabled();
	void enableJump(bool enable);
	bool isJumpEnabled();
	///@}

	/**
	 * \name Speeds getters/setters.
	 */
	///@{
	void setLinearSpeed(LVecBase2f speed);
	LVecBase2f getLinearSpeed();
	float getAngularSpeed();
	void setAngularSpeed(float speed);
	///@}

	/**
	 * \name Gets/sets if linear movement is local.
	 */
	///@{
	void setIsLocal(bool isLocal);
	bool getIsLocal();
	///@}

	/**
	 * \brief Gets/sets the node path of this character controller.
	 * @return The node path of this character controller.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

private:
	///The NodePath associated to this character controller.
	NodePath mNodePath;
	///The NodePath associated to this character controller.
	SMARTPTR(BulletCharacterControllerNode) mCharacterController;

	///Geometric functions and parameters.
	///@{
	BitMask32 mCollideMask;
	float mStepHeight;
	/**
	 * \brief Create a shape given its type.
	 * @param shapeType The shape type.
	 * @return The created shape.
	 */
	SMARTPTR(BulletShape) createShape(GamePhysicsManager::ShapeType shapeType);
	GamePhysicsManager::ShapeType mShapeType;
	GamePhysicsManager::ShapeSize mShapeSize;
	LVector3 mModelDims;
	float mModelRadius;
	//use shape of (another object).
	ObjectId mUseShapeOfId;
	//any model has a local frame and the tight bounding box is computed
	//wrt it; so mModelDeltaCenter represents a transform (translation) to
	//be applied to the model node path so that the middle point of the
	//bounding box will overlap the frame center of the parent's node path .
	LVector3 mModelDeltaCenter;
	bool mAutomaticShaping;
	float mDim1, mDim2, mDim3, mDim4;
	BulletUpAxis mUpAxis;
	///@}

	///Control functions and parameters.
	///@{
	float mAngularSpeed, mFallSpeed, mGravity, mJumpSpeed, mMaxSlope, mMaxJumpHeight;
	LVecBase2f mLinearSpeed;
	///Flag if linear movement is local.
	bool mIsLocal;
	/**
	 * \brief Sets control parameters of a character controller node (helper function).
	 */
	void setControlParameters();
	///Key controls and effective keys.
	bool mForward, mBackward, mStrafeLeft, mStrafeRight,mRollLeft, mRollRight, mJump;
	bool mForwardKey, mBackwardKey, mStrafeLeftKey, mStrafeRightKey,
	mRollLeftKey, mRollRightKey, mJumpKey;
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
		register_type(_type_handle, "CharacterController", Component::get_class_type());
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

#endif /* CHARACTERCONTROLLER_H_ */
