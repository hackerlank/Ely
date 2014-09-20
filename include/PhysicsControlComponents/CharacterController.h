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
 * \file /Ely/include/PhysicsControlComponents/CharacterController.h
 *
 * \date 30/ott/2012 (17:03:49)
 * \author consultit
 */

#ifndef CHARACTERCONTROLLER_H_
#define CHARACTERCONTROLLER_H_

#include "PhysicsComponents/BulletLocal/bulletCharacterControllerNode.h"
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include <throw_event.h>
#include "Game/GamePhysicsManager.h"

namespace ely
{
class CharacterControllerTemplate;

/**
 * \brief Component representing a character controller attached to an object.
 *
 * The control is accomplished through physics.\n
 * It constructs a character controller with the single specified collision
 * shape_type along with relevant parameters.\n
 * The up axis is the Z axis.\n
 * If specified in "thrown_events", this component can throw
 * these events (shown with default names):
 * - when standing on ground (<ObjectType>_CharacterController_OnGround)
 * - when soaring in air (<ObjectType>_CharacterController_InAir)
 * Events are thrown continuously at a frequency which is the minimum between
 * the fps and the frequency specified (which defaults to 30 times per seconds).\n
 * The argument of each event is a reference to this component.\n
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *thrown_events*			|single| - | specified as "event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]" with eventX = on_ground,in_air
 * | *step_height*  			|single| 1.0 | -
 * | *collide_mask*  			|single| *all_on* | -
 * | *shape_type*  				|single| *sphere* | -
 * | *shape_size*  				|single| *medium* | values min,medium,max
 * | *use_shape_of*				|single| - | -
 * | *shape_radius*  			|single| - | for sphere,cylinder,capsule,cone
 * | *shape_height*  			|single| - | for cylinder,capsule,cone
 * | *shape_up*  				|single| *z* | values: x,y,z for cylinder,capsule,cone
 * | *shape_half_x*  			|single| - | for box
 * | *shape_half_y*  			|single| - | for box
 * | *shape_half_z*  			|single| - | for box
 * | *fall_speed*  				|single| 55.0 | -
 * | *gravity*  				|single| 29.4 | ==3G
 * | *jump_speed*  				|single| 10.0 | -
 * | *max_slope*  				|single| 45.0 | measured in degrees
 * | *max_jump_height*  		|single| - | -
 * | *forward*  				|single| *enabled* | -
 * | *backward*  				|single| *enabled* | -
 * | *up*  						|single| *enabled* | -
 * | *down*  					|single| *enabled* | -
 * | *head_left*  				|single| *enabled* | -
 * | *head_right*  				|single| *enabled* | -
 * | *strafe_left*  			|single| *enabled* | -
 * | *strafe_right*  			|single| *enabled* | -
 * | *jump*  					|single| *enabled* | -
 * | *linear_speed*  			|single| 10.0 | -
 * | *angular_speed*  			|single| 45.0 | -
 * | *is_local*  				|single| *true* | -
 *
 * \note parts inside [] are optional.\n
 */
class CharacterController: public Component
{
protected:
	friend class CharacterControllerTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	CharacterController();
	CharacterController(SMARTPTR(CharacterControllerTemplate)tmpl);
	virtual ~CharacterController();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an physics manager update.
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
	void enableUp(bool enable);
	bool isUpEnabled();
	void enableDown(bool enable);
	bool isDownEnabled();
	void enableStrafeLeft(bool enable);
	bool isStrafeLeftEnabled();
	void enableStrafeRight(bool enable);
	bool isStrafeRightEnabled();
	void enableHeadLeft(bool enable);
	bool isHeadLeftEnabled();
	void enableHeadRight(bool enable);
	bool isHeadRightEnabled();
	void enableJump(bool enable);
	bool isJumpEnabled();
	///@}

	/**
	 * \name Speeds getters/setters.
	 */
	///@{
	void setLinearSpeed(const LVecBase3f& speed);
	LVecBase3f getLinearSpeed();
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
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \name BulletCharacterControllerNode reference getter & conversion function.
	 */
	///@{
	BulletCharacterControllerNode& getBulletCharacterControllerNode();
	operator BulletCharacterControllerNode&();
	///@}

	///CharacterController thrown events.
	enum EventThrown
	{
		ONGROUNDEVENT,
		INAIREVENT
	};

	/**
	 * \brief Enables/disables the CharacterController event to be thrown.
	 * @param event The CharacterController event.
	 * @param eventData The CharacterController event data. ThrowEventData::mEnable
	 * will enable/disable the event.
	 */
	void enableCharacterControllerEvent(EventThrown event, ThrowEventData eventData);

private:
	///The NodePath associated to this character controller.
	NodePath mNodePath;
	///The underlying BulletCharacterControllerNode (read-only after creation & before destruction).
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
	SMARTPTR(BulletShape) doCreateShape(GamePhysicsManager::ShapeType shapeType);
	GamePhysicsManager::ShapeType mShapeType;
	GamePhysicsManager::ShapeSize mShapeSize;
	LVector3f mModelDims;
	float mModelRadius;
	//use shape of (another object).
	ObjectId mUseShapeOfId;
	//any model has a local frame and the tight bounding box is computed
	//wrt it; so mModelDeltaCenter represents a transform (translation) to
	//be applied to the model node path so that the middle point of the
	//bounding box will overlap the frame center of the parent's node path .
	LVector3f mModelDeltaCenter;
	bool mAutomaticShaping;
	float mDim1, mDim2, mDim3, mDim4;
	BulletUpAxis mUpAxis;
	///@}

	///Control functions and parameters.
	///@{
	float mAngularSpeed, mFallSpeed, mGravity, mJumpSpeed, mMaxSlope, mMaxJumpHeight;
	LVecBase3f mLinearSpeed;
	///Flag if linear movement is local.
	bool mIsLocal;
	/**
	 * \brief Sets control parameters of a character controller node (helper function).
	 */
	void doSetControlParameters();
	///Key controls and effective keys.
	bool mForward, mBackward, mUp, mDown, mStrafeLeft, mStrafeRight,mHeadLeft, mHeadRight, mJump;
	bool mForwardKey, mBackwardKey, mUpKey, mDownKey, mStrafeLeftKey, mStrafeRightKey,
	mHeadLeftKey, mHeadRightKey, mJumpKey;
	///@}

	/**
	 * \name Throwing CharacterController events.
	 */
	///@{
	ThrowEventData mOnGround, mInAir;
	///Helper.
	void doEnableCharacterControllerEvent(EventThrown event, ThrowEventData eventData);
	void doThrowEvent(ThrowEventData& eventData);
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

///inline definitions

inline void CharacterController::reset()
{
	//
	mNodePath = NodePath();
	mCharacterController.clear();
	mCollideMask = BitMask32::all_off();
	mStepHeight = 0.0;
	mShapeType = GamePhysicsManager::SPHERE;
	mShapeSize = GamePhysicsManager::MEDIUM;
	mModelDims = LVector3f::zero();
	mModelRadius = 0.0;
	mUseShapeOfId = ObjectId();
	mModelDeltaCenter = LVector3f::zero();
	mAutomaticShaping = false;
	mDim1 = mDim2 = mDim3 = mDim4 = 0.0;
	mUpAxis = Z_up;
	mAngularSpeed = mFallSpeed = mGravity = mJumpSpeed = mMaxSlope =
			mMaxJumpHeight = 0.0;
	mLinearSpeed = LVector3f::zero();
	mIsLocal = false;
	mForward = mBackward = mStrafeLeft = mStrafeRight = mUp = mDown =
			mHeadLeft = mHeadRight = mJump = false;
	mForwardKey = mBackwardKey = mStrafeLeftKey = mStrafeRightKey = mUpKey, mDownKey =
			mHeadLeftKey = mHeadRightKey = mJumpKey = false;
	mOnGround = mInAir = ThrowEventData();
}

inline void CharacterController::enableForward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mForwardKey)
	{
		mForward = enable;
	}
}

inline bool CharacterController::isForwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mForward;
}

inline void CharacterController::enableBackward(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mBackwardKey)
	{
		mBackward = enable;
	}
}

inline bool CharacterController::isBackwardEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mBackward;
}

inline void CharacterController::enableUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mUpKey)
	{
		mUp = enable;
	}
}

inline bool CharacterController::isUpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mUp;
}

inline void CharacterController::enableDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mDownKey)
	{
		mDown = enable;
	}
}

inline bool CharacterController::isDownEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mDown;
}

inline void CharacterController::enableStrafeLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStrafeLeftKey)
	{
		mStrafeLeft = enable;
	}
}

inline bool CharacterController::isStrafeLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStrafeLeft;
}

inline void CharacterController::enableStrafeRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mStrafeRightKey)
	{
		mStrafeRight = enable;
	}
}

inline bool CharacterController::isStrafeRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mStrafeRight;
}

inline void CharacterController::enableHeadLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadLeftKey)
	{
		mHeadLeft = enable;
	}
}

inline bool CharacterController::isHeadLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadLeft;
}

inline void CharacterController::enableHeadRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadRightKey)
	{
		mHeadRight = enable;
	}
}

inline bool CharacterController::isHeadRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadRight;
}

inline void CharacterController::enableJump(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mJumpKey)
	{
		mJump = enable;
	}
}

inline bool CharacterController::isJumpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mJump;
}

inline LVecBase3f CharacterController::getLinearSpeed()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mLinearSpeed;
}

inline void CharacterController::setLinearSpeed(const LVecBase3f& speed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mLinearSpeed = speed;
}

inline float CharacterController::getAngularSpeed()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAngularSpeed;
}

inline void CharacterController::setAngularSpeed(float speed)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mAngularSpeed = speed;
}

inline void CharacterController::setIsLocal(bool isLocal)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mIsLocal = isLocal;
}

inline bool CharacterController::getIsLocal()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mIsLocal;
}

inline NodePath CharacterController::getNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mNodePath;
}

inline void CharacterController::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mNodePath = nodePath;
}

inline BulletCharacterControllerNode& CharacterController::getBulletCharacterControllerNode()
{
	return *mCharacterController;
}

inline CharacterController::operator BulletCharacterControllerNode&()
{
	return *mCharacterController;
}

inline void CharacterController::enableCharacterControllerEvent(EventThrown event, ThrowEventData eventData)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	doEnableCharacterControllerEvent(event, eventData);
}

inline void CharacterController::doThrowEvent(ThrowEventData& eventData)
{
	if (eventData.mThrown)
	{
		eventData.mTimeElapsed += ClockObject::get_global_clock()->get_dt();
		if (eventData.mTimeElapsed >= eventData.mPeriod)
		{
			//enough time is passed: throw the event
			throw_event(eventData.mEventName, EventParameter(this));
			//update elapsed time
			eventData.mTimeElapsed -= eventData.mPeriod;
		}
	}
	else
	{
		//throw the event
		throw_event(eventData.mEventName, EventParameter(this));
		eventData.mThrown = true;
	}
}

}  // namespace ely

#endif /* CHARACTERCONTROLLER_H_ */
