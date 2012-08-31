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
 * \file /Ely/include/ControlComponents/Driver.h
 *
 * \date 31/mag/2012 (16:42:14)
 * \author marco
 */

#ifndef CONTROLBYEVENT_H_
#define CONTROLBYEVENT_H_

#include <string>
#include <set>
#include <cstdlib>
#include <nodePath.h>
#ifdef ELY_THREAD
#	include <pointerTo.h>
#endif
#include <typedObject.h>
#include <event.h>
#include <genericAsyncTask.h>
#include <mouseData.h>
#include <graphicsWindow.h>
#include <windowProperties.h>
#include <transformState.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Game/GameControlManager.h"
#include "Utilities/Tools.h"

class DriverTemplate;

/**
 * \brief Component designed for the control of object movement
 * through keyboard/mouse button events and mouse movement.
 *
 * Each basic movement (forward, backward, roll_left, roll_right etc...)
 * can be enabled/disabled through a corresponding "enabler", which in
 * turn set a control key true or false.
 * An event handlers could enable/disable movement calling the "enablers".
 * A task updates the position/orientation of the controlled object
 * based on the value of control keys.
 * The component can be enabled/disabled as a whole.
 * At configuration level (from xml config file), any "enabler" can be
 * enabled/disabled by setting corresponding configuration key
 * to "enabled"/"disabled".
 * Mouse movements tracking are special. Since "mouse move" events are not
 * defined by default (they can by using ButtonThrower::set_move_event()),
 * mouse movements are polled by default during the "update" task, that is
 * the corresponding "enabler" is disabled.
 * The object HEAD (i.e. YAW) and PITCH control through mouse movements
 * can be enabled/disabled separately (default: both disabled).
 * All movements (but up and down) can be inverted (default: not inverted).
 *
 * XML Param(s):
 * \li \c "enabled"  			|single|"true"
 * \li \c "forward"  			|single|"enabled"
 * \li \c "backward"  			|single|"enabled"
 * \li \c "roll_left"  			|single|"enabled"
 * \li \c "roll_right"  		|single|"enabled"
 * \li \c "strafe_left"  		|single|"enabled"
 * \li \c "strafe_right"  		|single|"enabled"
 * \li \c "up"  				|single|"enabled"
 * \li \c "down"  				|single|"enabled"
 * \li \c "mouse_move"  		|single|"disabled"
 * \li \c "mouse_enabled_h"  	|single|"false"
 * \li \c "mouse_enabled_p"  	|single|"false"
 * \li \c "speed_key"  			|single|"shift"
 * \li \c "speed"  				|single|"5.0"
 * \li \c "fast_factor"  		|single|"5.0"
 * \li \c "mov_sens"  			|single|"2.0"
 * \li \c "roll_sens"  			|single|"15.0"
 * \li \c "sens_x"  			|single|"0.2"
 * \li \c "sens_y"  			|single|"0.2"
 * \li \c "inverted_keyboard"  	|single|"false"
 * \li \c "inverted_mouse"  	|single|"false"
 */
class Driver: public Component
{
public:
	Driver();
	Driver(SMARTPTR(DriverTemplate) tmpl);
	virtual ~Driver();

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
	 * \name Enabling/disabling.
	 * \brief Enables/disables this component.
	 */
	///@{
	void enable();
	void disable();
	bool isEnabled();
	///@}

	/**
	 * \name Control keys' enablers.
	 *
	 * These routines should be typically called by
	 * event handlers, but this is not strictly required.
	 */
	///@{
	void enableForward(bool enable);
	void enableBackward(bool enable);
	void enableStrafeLeft(bool enable);
	void enableStrafeRight(bool enable);
	void enableUp(bool enable);
	void enableDown(bool enable);
	void enableRollLeft(bool enable);
	void enableRollRight(bool enable);
	void enableMouseMove(bool enable);
	///@}

	/**
	 * \name Speed setters.
	 */
	///@{
	void setSpeed();
	void setSpeedFast();
	///@}

private:
	///@{
	///Key controls and effective keys.
	bool mForward, mBackward, mStrafeLeft, mStrafeRight, mUp, mDown,
			mRollLeft, mRollRight, mMouseMove;
	bool mForwardKey, mBackwardKey, mStrafeLeftKey, mStrafeRightKey, mUpKey,
			mDownKey, mRollLeftKey, mRollRightKey, mMouseMoveKey;
	std::string mSpeedKey;
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

#ifdef ELY_THREAD
	///Actual transform state.
	CSMARTPTR(TransformState) mActualTransform;
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Driver", Component::get_class_type());
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
