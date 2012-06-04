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
#include <nodePath.h>
#include <typedObject.h>
#include <event.h>
#include <asyncTask.h>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

class ControlByEventTemplate;

/**
 * \brief Component representing the control of object movement
 * through keyboard and mouse button events.
 *
 * To each basic movement (forward, backward, left, right etc...)
 * are associated a button event and a control key, which tracks
 * its state (true/false). Event handlers commute basic movements
 * states. A (global) task updates the position/orientation of
 * the controlled object.
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
	 *
	 * Each of these events will be associated to an handler that will
	 * control one specific movement of the object associated to this component.
	 */
	///@{
	void backwardHandler(const Event* event, void* data);
	void downHandler(const Event* event, void* data);
	void forwardHandler(const Event* event, void* data);
	void strafeLeftHandler(const Event* event, void* data);
	void strafeRightHandler(const Event* event, void* data);
	void rollLeftHandler(const Event* event, void* data);
	void rollRightHandler(const Event* event, void* data);
	void upHandler(const Event* event, void* data);
	///@}

	/**
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * @param task The task object.
	 * @return The "done" status.
	 */
	virtual AsyncTask::DoneStatus update(GenericAsyncTask* task);

private:
	///The template used to construct this component.
	ControlByEventTemplate* mTmpl;
	///@{
	///Key controls.
	bool mForward, mBackward, mStrafeLeft, mStrafeRight, mUp, mDown, mRollLeft, mRollRight;
	///@}
	///@{
	///Key control values.
	bool mTrue, mFalse;
	///@}
	///@{
	/// Sensitivity settings.
	float mSpeed, mSpeedFast;
	float mMovSens, mMovSensFast;
	float mRollSens;
	float mSensX, mSensY;
	///@}
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
