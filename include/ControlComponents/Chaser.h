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
 * \author consultit
 */

#ifndef CHASER_H_
#define CHASER_H_

#include <nodePath.h>
#include <cfloat>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"

namespace ely
{
class ChaserTemplate;

/**
 * \brief Component designed to make an object a chaser of another object.
 *
 * The up axis is the "z" axis.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *enabled*  				|single| *true* | -
 * | *backward*					|single| *true* | -
 * | *chased_object*			|single| - | -
 * | *fixed_relative_position*	|single| *true* | -
 * | *reference_object*			|single| - | -
 * | *abs_max_distance*			|single| - | -
 * | *abs_min_distance*			|single| - | -
 * | *abs_max_height*			|single| - | -
 * | *abs_min_height*			|single| - | -
 * | *friction*					|single| 1.0 | -
 * | *fixed_lookat*				|single| *true* | -
 * | *abs_lookat_distance*		|single| - | -
 * | *abs_lookat_height*		|single| - | -
 * | *mouse_enabled_h*  		|single| *false* | -
 * | *mouse_enabled_p*  		|single| *false* | -
 * | *head_left*  				|single| *enabled* | -
 * | *head_right*  				|single| *enabled* | -
 * | *pitch_up*  				|single| *enabled* | -
 * | *pitch_down*  				|single| *enabled* | -
 * | *sens_x*  					|single| 0.2 | -
 * | *sens_y*  					|single| 0.2 | -
 * | *inverted_rotation*		|single| *false* | -
 *
 * \note parts inside [] are optional.\n
 */
class Chaser: public Component
{
protected:
	friend class ChaserTemplate;

	Chaser(SMARTPTR(ChaserTemplate)tmpl);
	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	virtual ~Chaser();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * \see OgreBulletDemos.
	 *
	 * Will be called automatically by an control manager update.
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
			CHASER_DISABLING = COMPONENT_RESULT_END + 1,
			CHASER_RESULT_END
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
	 * \name Getting/setting parameters.
	 */
	///@{
	float getAbsMaxDistance() const;
	void setAbsMaxDistance(float absMaxDistance);
	float getAbsMinDistance() const;
	void setAbsMinDistance(float absMinDistance);
	float getAbsMaxHeight() const;
	void setAbsMaxHeight(float absMaxHeight);
	float getAbsMinHeight() const;
	void setAbsMinHeight(float absMinHeight);
	float getAbsLookAtDistance() const;
	void setAbsLookAtDistance(float absLookAtDistance);
	float getAbsLookAtHeight() const;
	void setAbsLookAtHeight(float absLookAtHeight);
	void setFriction(float friction);
	float getFriction() const;
	void enableFixedRelativePosition(bool enable);
	bool isFixedRelativePosition();
	///@}

	/**
	 * \name Control keys' getters/setters.
	 */
	///@{
	void enableHeadLeft(bool enable);
	bool isHeadLeftEnabled();
	void enableHeadRight(bool enable);
	bool isHeadRightEnabled();
	void enablePitchUp(bool enable);
	bool isPitchUpEnabled();
	void enablePitchDown(bool enable);
	bool isPitchDownEnabled();
	void holdLookAt(bool enable);
	///@}

	/**
	 * \name Gets/sets chased object.
	 */
	///@{
	ObjectId getChasedObject();
	void setChasedObject(const ObjectId& objectId);
	///@}

private:
	///The chased object's node path.
	NodePath mChasedNodePath;
	///The reference object's node path.
	NodePath mReferenceNodePath;
	///Auxiliary node path to track the fixed look at.
	NodePath mFixedLookAtNodePath;
	///Flags.
	bool mStartEnabled, mEnabled, mFixedRelativePosition, mBackward,
	mFixedLookAt, mHoldLookAt;
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
	/**
	 * \name Main parameters.
	 */
	///@{
	///Chased object id.
	ObjectId mChasedId;
	///Reference object id.
	ObjectId mReferenceId;
	///@}
	///Kinematic parameters.
	float mAbsLookAtDistance, mAbsLookAtHeight, mAbsMaxDistance, mAbsMinDistance,
	mAbsMinHeight, mAbsMaxHeight, mFriction;
	///Positions.
	LPoint3f mChaserPosition, mLookAtPosition;
	///@{
	///Key controls and effective keys.
	bool mHeadLeft, mHeadRight, mPitchUp, mPitchDown;
	bool mHeadLeftKey, mHeadRightKey, mPitchUpKey, mPitchDownKey;
	///@}
	///@{
	///Key control values.
	bool mMouseEnabledH,	mMouseEnabledP;
	int mSignOfMouse;
	///@}
	///@{
	/// Sensitivity settings.
	float mSensX, mSensY, mHeadSensX, mHeadSensY;
	int mCentX, mCentY;
	///@}

	/**
	 * \brief Calculates the dynamic position of the chaser.
	 * \see OgreBulletDemos.
	 * @param desiredChaserPos The desired chaser position (wrt reference).
	 * @param currentChaserPos The current chaser position (wrt reference).
	 * @param deltaTime The delta time update.
	 * @return The dynamic chaser position.
	 */
	LPoint3f doGetChaserPos(LPoint3f desiredChaserPos,
			LPoint3f currentChaserPos, float deltaTime);
	/**
	 * \brief Correct the dynamic height of the chaser.
	 * @param newPos The position whose height may be corrected.
	 * @param baseHeight The corrected height cannot be shorter than this.
	 */
	void doCorrectChaserHeight(LPoint3f& newPos, float baseHeight);

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

///inline definitions

inline void Chaser::reset()
{
	//
	mChasedNodePath = NodePath();
	mReferenceNodePath = NodePath();
	mFixedLookAtNodePath = NodePath();
	mStartEnabled = mEnabled = mFixedRelativePosition = mBackward =
			mHoldLookAt = false;
	mFixedLookAt = true;
#ifdef ELY_THREAD
	mDisabling = false;
#endif
	mChasedId = ObjectId();
	mReferenceId = ObjectId();
	mAbsLookAtDistance = mAbsLookAtHeight = mFriction = 0.0;
	mAbsMaxDistance = mAbsMaxHeight = FLT_MAX;
	mAbsMinDistance = mAbsMinHeight = FLT_MIN;
	mChaserPosition = mLookAtPosition = LPoint3f::zero();
	mHeadLeftKey = mHeadRightKey = mPitchUpKey = mPitchDownKey = false;
	mHeadLeft = mHeadRight = mPitchUp = mPitchDown = mMouseEnabledH =
			mMouseEnabledP = false;
	mSignOfMouse = 1;
	mSensX = mSensY = 0.0;
	mCentX = mCentY = 0.0;
}

inline bool Chaser::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mEnabled;
}

inline float Chaser::getAbsMaxDistance() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMaxDistance;
}

inline void Chaser::setAbsMaxDistance(float absMaxDistance)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMaxDistance >= 0.0 ? mAbsMaxDistance = absMaxDistance : mAbsMaxDistance =
									-absMaxDistance;
}

inline float Chaser::getAbsMinDistance() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMinDistance;
}

inline void Chaser::setAbsMinDistance(float absMinDistance)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMinDistance >= 0.0 ? mAbsMinDistance = absMinDistance : mAbsMinDistance =
									-absMinDistance;
}

inline float Chaser::getAbsMaxHeight() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMaxHeight;
}

inline void Chaser::setAbsMaxHeight(float absMaxHeight)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMaxHeight >= 0.0 ? mAbsMaxHeight = absMaxHeight : mAbsMaxHeight =
									-absMaxHeight;
}

inline float Chaser::getAbsMinHeight() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsMinHeight;
}

inline void Chaser::setAbsMinHeight(float absMinHeight)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absMinHeight >= 0.0 ? mAbsMinHeight = absMinHeight : mAbsMinHeight =
									-absMinHeight;
}

inline float Chaser::getAbsLookAtDistance() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsLookAtDistance;
}

inline void Chaser::setAbsLookAtDistance(float absLookAtDistance)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absLookAtDistance >= 0.0 ? mAbsLookAtDistance = absLookAtDistance : mAbsLookAtDistance =
									-absLookAtDistance;
}

inline float Chaser::getAbsLookAtHeight() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mAbsLookAtHeight;
}

inline void Chaser::setAbsLookAtHeight(float absLookAtHeight)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	absLookAtHeight >= 0.0 ? mAbsLookAtHeight = absLookAtHeight : mAbsLookAtHeight =
									-absLookAtHeight;
}

inline float Chaser::getFriction() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFriction;
}

inline void Chaser::setFriction(float friction)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	friction >= 0.0 ? mFriction = friction : mFriction =
									-friction;
}

inline void Chaser::enableHeadLeft(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadLeftKey)
	{
		mHeadLeft = enable;
	}
}

inline bool Chaser::isHeadLeftEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadLeft;
}

inline void Chaser::enableHeadRight(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mHeadRightKey)
	{
		mHeadRight = enable;
	}
}

inline bool Chaser::isHeadRightEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHeadRight;
}

inline void Chaser::enablePitchUp(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchUpKey)
	{
		mPitchUp = enable;
	}
}

inline bool Chaser::isPitchUpEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchUp;
}

inline void Chaser::enablePitchDown(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mPitchDownKey)
	{
		mPitchDown = enable;
	}
}

inline bool Chaser::isPitchDownEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mPitchDown;
}

inline void Chaser::holdLookAt(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mHoldLookAt = enable;
}

inline void Chaser::enableFixedRelativePosition(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mFixedRelativePosition = enable;
}

inline bool Chaser::isFixedRelativePosition()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFixedRelativePosition;
}

inline ObjectId Chaser::getChasedObject()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mChasedId;
}

inline void Chaser::setChasedObject(const ObjectId& objectId)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//set the (node path of) object chased by this component;
	//that object is supposed to be already created,
	//set up and added to the created objects table;
	//if not, this component chases nothing.
	mChasedId = objectId;
	SMARTPTR(Object)chasedObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mChasedId);
	if (chasedObject != NULL)
	{
		mChasedNodePath = chasedObject->getNodePath();

		//set the (node path of) reference object;
		//that object is supposed to be already created,
		//set up and added to the created objects table;
		//if not, this will be the parent of the chased object.
		SMARTPTR(Object)referenceObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				mReferenceId);
		if (referenceObject != NULL)
		{
			mReferenceNodePath = referenceObject->getNodePath();
		}
		else
		{
			mReferenceNodePath = mChasedNodePath.get_parent();
		}
	}
}

///Template

class ChaserTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	ChaserTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ChaserTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType familyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "ChaserTemplate",
				ComponentTemplate::get_class_type());
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

#endif /* CHASER_H_ */
