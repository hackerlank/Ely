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
 * \file /Ely/include/InputComponents/ControlByEvent.h
 *
 * \date 31/mag/2012 (16:42:14)
 * \author marco
 */

#ifndef CONTROLBYEVENT_H_
#define CONTROLBYEVENT_H_

#include <string>
#include <cstdlib>
#include <nodePath.h>
#include <typedObject.h>
#include <event.h>
#include <genericAsyncTask.h>
#include <mouseData.h>
#include <graphicsWindow.h>
#include <windowProperties.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

class ControlByEventTemplate;

/**
 * \brief Component representing the control of object movement
 * through keyboard/mouse button events and mouse movement (optional).
 *
 * To each basic movement (forward, backward, left, right etc...)
 * are associated a button event and a control key, which tracks
 * its state (true/false). Event handlers commute basic movements
 * states. A (global) task updates the position/orientation of
 * the controlled object.
 * Default control key:
 * \li \c Forward : "w"
 * \li \c Backward : "s"
 * \li \c RollLeft : "a"
 * \li \c RollRight : "d"
 * \li \c StrafeLeft : "q"
 * \li \c StrafeRight : "e"
 * \li \c Up : "r"
 * \li \c Down : "f"
 * A basic movement can be disabled by putting its control key
 * to null string ("").
 * Mouse movement for HEAD (i.e. YAW) and PITCH control can be enabled,
 * separately (default: both disabled).
 * All movements (but up and down) can be inverted (default: not inverted).
 * This component can be enabled/disabled as a whole (default: enabled).
 */
class ControlByEvent: public Component
{
public:
	ControlByEvent();
	ControlByEvent(ControlByEventTemplate* tmpl);
	virtual ~ControlByEvent();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddSetup();

	/**
	 * \name Event handlers.
	 * \brief The set of handlers associated to the events.
	 */
	///@{
	static void setControlTrue(const Event* event, void* data);
	static void setControlFalse(const Event* event, void* data);
	static void setSpeed(const Event* event, void* data);
	static void setSpeedFast(const Event* event, void* data);
	///@}

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * @param task The task object.
	 * @return The "done" status.
	 */
	virtual AsyncTask::DoneStatus update(GenericAsyncTask* task);

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
	///The template used to construct this component.
	ControlByEventTemplate* mTmpl;
	///@{
	///Key controls and effective keys.
	bool mForward, mBackward, mStrafeLeft, mStrafeRight, mUp, mDown, mRollLeft,
			mRollRight;
	std::string mForwardKey, mBackwardKey, mStrafeLeftKey, mStrafeRightKey,
			mUpKey, mDownKey, mRollLeftKey, mRollRightKey, mSpeedKey;
	///@}
	///@{
	///Key control values.
	bool mTrue, mFalse, mInverted, mMouseEnabledH, mMouseEnabledP;
	///@}
	///@{
	/// Sensitivity settings.
	float mSpeed, mFastFactor, mSpeedActual;
	float mMovSens, mRollSens;
	float mSensX, mSensY;
	int mCentX, mCentY;
	///@}
	///Enabling flags.
	bool mEnabled, mIsEnabled;
	///A task data for update.
	PT(TaskInterface<ControlByEvent>::TaskData) mUpdateData;
	PT(AsyncTask) mUpdateTask;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "ControlByEvent",
				Component::get_class_type());
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

#endif /* CONTROLBYEVENT_H_ */
