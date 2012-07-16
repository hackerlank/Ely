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
#include <clockObject.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
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
 * A basic movement is disabled if its control key cannot be interpreted
 * or is the null string ("").
 * Mouse movement for HEAD (i.e. YAW) and PITCH control can be enabled,
 * separately (default: both disabled).
 * All movements (but up and down) can be inverted (default: not inverted).
 * This component can be enabled/disabled as a whole (default: enabled).
 *
 * XML Param(s):
 * \li \c "enabled"  |single|"true"
 * \li \c "forward"  |single|"w"
 * \li \c "backward"  |single|"s"
 * \li \c "roll_left"  |single|"a"
 * \li \c "roll_right"  |single|"d"
 * \li \c "strafe_left"  |single|"q"
 * \li \c "strafe_right"  |single|"e"
 * \li \c "up"  |single|"r"
 * \li \c "down"  |single|"f"
 * \li \c "speed_key"  |single|"shift"
 * \li \c "speed"  |single|"5.0"
 * \li \c "fast_factor"  |single|"5.0"
 * \li \c "mov_sens"  |single|"2.0"
 * \li \c "roll_sens"  |single|"15.0"
 * \li \c "sens_x"  |single|"0.2"
 * \li \c "sens_y"  |single|"0.2"
 * \li \c "inverted_keyboard"  |single|"false"
 * \li \c "inverted_mouse"  |single|"false"
 * \li \c "mouse_enabled_h"  |single|"false"
 * \li \c "mouse_enabled_p"  |single|"false"
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
	virtual void onAddToObjectSetup();

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
	bool mTrue, mFalse, mInvertedKeyboard, mInvertedMouse, mMouseEnabledH,
			mMouseEnabledP;
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

	///@}
	///A task data for update.
	PT(TaskInterface<ControlByEvent>::TaskData) mUpdateData;PT(AsyncTask) mUpdateTask;
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
