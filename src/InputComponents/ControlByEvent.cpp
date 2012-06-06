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
 * \file /Ely/src/InputComponents/ControlByEvent.cpp
 *
 * \date 31/mag/2012 (16:42:14)
 * \author marco
 */

#include "InputComponents/ControlByEvent.h"
#include "InputComponents/ControlByEventTemplate.h"

ControlByEvent::ControlByEvent()
{
	// TODO Auto-generated constructor stub
}

ControlByEvent::ControlByEvent(ControlByEventTemplate* tmpl) :
		mTmpl(tmpl), mForward(false), mBackward(false), mStrafeLeft(false), mStrafeRight(
				false), mUp(false), mDown(false), mRollLeft(false), mRollRight(
				false), mTrue(true), mFalse(false)
{
	mUpdateData = NULL;
	mUpdateTask = NULL;
}

ControlByEvent::~ControlByEvent()
{
	//undefine key events
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mBackward);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mDown);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mForward);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mStrafeLeft);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mStrafeRight);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mRollLeft);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mRollRight);
	mTmpl->pandaFramework()->get_event_handler().remove_hooks_with(
			(void*) &this->mUp);
	//remove the task
	if (mUpdateTask)
	{
		mTmpl->pandaFramework()->get_task_mgr().remove(mUpdateTask);
	}
}

const ComponentFamilyType ControlByEvent::familyType() const
{
	return ComponentFamilyType("Input");
}

const ComponentType ControlByEvent::componentType() const
{
	return ComponentType("ControlByEvent");
}

bool ControlByEvent::initialize()
{
	bool result = true;
	std::string speedKey = mTmpl->speedKey();
	if (not (speedKey == "control" or speedKey == "alt" or speedKey == "shift"))
	{
		speedKey = "shift";
	}
	//helper variables
	std::string keyEvent, speedKeyEvent, upKeyEvent;
	//register the handlers
	//backward event keys (e.g. "s", "shift-s", "s-up")
	keyEvent = mTmpl->backwardEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "backward",
			&ControlByEvent::setTrue, (void*) &this->mBackward);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-backward",
			&ControlByEvent::setTrue, (void*) &this->mBackward);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "backward-up",
			&ControlByEvent::setFalse, (void*) &this->mBackward);

	//down event keys (e.g. "f", "shift-f", "f-up")
	keyEvent = mTmpl->downEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "down",
			&ControlByEvent::setTrue, (void*) &this->mDown);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-down",
			&ControlByEvent::setTrue, (void*) &this->mDown);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "down-up",
			&ControlByEvent::setFalse, (void*) &this->mDown);
	//forward event keys (e.g. "w", "shift-w", "w-up")
	keyEvent = mTmpl->forwardEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "forward",
			&ControlByEvent::setTrue, (void*) &this->mForward);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-forward",
			&ControlByEvent::setTrue, (void*) &this->mForward);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "forward-up",
			&ControlByEvent::setFalse, (void*) &this->mForward);
	//strafeLeft event keys (e.g. "a", "shift-a", "a-up")
	keyEvent = mTmpl->strafeLeftEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "strafeLeft",
			&ControlByEvent::setTrue, (void*) &this->mStrafeLeft);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeLeft",
			&ControlByEvent::setTrue, (void*) &this->mStrafeLeft);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeLeft-up",
			&ControlByEvent::setFalse, (void*) &this->mStrafeLeft);
	//strafeRight event keys (e.g. "d", "shift-d", "d-up")
	keyEvent = mTmpl->strafeRightEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "strafeRight",
			&ControlByEvent::setTrue, (void*) &this->mStrafeRight);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeRight",
			&ControlByEvent::setTrue, (void*) &this->mStrafeRight);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeRight-up",
			&ControlByEvent::setFalse, (void*) &this->mStrafeRight);
	//rollLeft event keys (e.g. "q", "shift-q", "q-up")
	keyEvent = mTmpl->rollLeftEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "rollLeft",
			&ControlByEvent::setTrue, (void*) &this->mRollLeft);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollLeft",
			&ControlByEvent::setTrue, (void*) &this->mRollLeft);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "rollLeft-up",
			&ControlByEvent::setFalse, (void*) &this->mRollLeft);
	//rollRight event keys (e.g. "e", "shift-e", "e-up")
	keyEvent = mTmpl->rollRightEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "rollRight",
			&ControlByEvent::setTrue, (void*) &this->mRollRight);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollRight",
			&ControlByEvent::setTrue, (void*) &this->mRollRight);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "rollRight-up",
			&ControlByEvent::setFalse, (void*) &this->mRollRight);
	//up event keys (e.g. "r", "shift-r", "r-up")
	keyEvent = mTmpl->upEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "up",
			&ControlByEvent::setTrue, (void*) &this->mUp);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-up",
			&ControlByEvent::setTrue, (void*) &this->mUp);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "up-up",
			&ControlByEvent::setFalse, (void*) &this->mUp);
	//set sensitivity parameters
	mSpeed = mTmpl->speed();
	mSpeedFast = mTmpl->speedFast();
	return result;
}

void ControlByEvent::onAddSetup()
{
	//add the task for updating the controlled object
	mUpdateData = new TaskInterface<ControlByEvent>::TaskData(this,
			&ControlByEvent::update);
	mUpdateTask = new GenericAsyncTask("ControlByEvent::update",
			&TaskInterface<ControlByEvent>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	mTmpl->pandaFramework()->get_task_mgr().add(mUpdateTask);
}

void ControlByEvent::setTrue(const Event* event, void* data)
{
	bool* boolPtr = (bool*) data;
	*boolPtr = true;
}

void ControlByEvent::setFalse(const Event* event, void* data)
{
	bool* boolPtr = (bool*) data;
	*boolPtr = false;
}

AsyncTask::DoneStatus ControlByEvent::update(GenericAsyncTask* task)
{
	return AsyncTask::DS_done;
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEvent::_type_handle;
