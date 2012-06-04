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

ControlByEvent::ControlByEvent()
{
	// TODO Auto-generated constructor stub
}

ControlByEvent::ControlByEvent(ControlByEventTemplate* tmpl) :
		mTmpl(tmpl), mForward(false), mBackward(false), mStrafeLeft(false), mStrafeRight(
				false), mUp(false), mDown(false), mRollLeft(false), mRollRight(
				false), mTrue(true), mFalse(false)
{
}

ControlByEvent::~ControlByEvent()
{
	//remove the task
	mTmpl->pandaFramework()->get_task_mgr().remove(mUpdateTask);
	//undefine key events
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
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::backwardHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-backward",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::backwardHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "backward-up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::backwardHandler>,
			(void*) &mFalse);
	//down event keys (e.g. "f", "shift-f", "f-up")
	keyEvent = mTmpl->downEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "down",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::downHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-down",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::downHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "down-up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::downHandler>,
			(void*) &mFalse);
	//forward event keys (e.g. "w", "shift-w", "w-up")
	keyEvent = mTmpl->forwardEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "forward",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::forwardHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-forward",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::forwardHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "forward-up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::forwardHandler>,
			(void*) &mFalse);
	//strafeLeft event keys (e.g. "a", "shift-a", "a-up")
	keyEvent = mTmpl->strafeLeftEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "strafeLeft",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeLeftHandler>, (void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeLeft",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeLeftHandler>, (void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeLeft-up",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeLeftHandler>, (void*) &mFalse);
	//strafeRight event keys (e.g. "d", "shift-d", "d-up")
	keyEvent = mTmpl->strafeRightEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "strafeRight",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeRightHandler>, (void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-strafeRight",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeRightHandler>, (void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "strafeRight-up",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeRightHandler>, (void*) &mFalse);
	//rollLeft event keys (e.g. "q", "shift-q", "q-up")
	keyEvent = mTmpl->rollLeftEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "rollLeft",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::rollLeftHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollLeft",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::rollLeftHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "rollLeft-up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::rollLeftHandler>,
			(void*) &mFalse);
	//rollRight event keys (e.g. "e", "shift-e", "e-up")
	keyEvent = mTmpl->rollRightEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "rollRight",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::rollRightHandler>, (void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-rollRight",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::rollRightHandler>, (void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "rollRight-up",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::rollRightHandler>, (void*) &mFalse);
	//up event keys (e.g. "r", "shift-r", "r-up")
	keyEvent = mTmpl->upEvent();
	speedKeyEvent = speedKey + "-" + keyEvent;
	upKeyEvent = keyEvent + "-up";
	mTmpl->pandaFramework()->define_key(keyEvent, "up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::upHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(speedKeyEvent, "speed-up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::upHandler>,
			(void*) &mTrue);
	mTmpl->pandaFramework()->define_key(upKeyEvent, "up-up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::upHandler>,
			(void*) &mFalse);
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

void ControlByEvent::backwardHandler(const Event* event, void* data)
{
	mBackward = (bool) (*data);
}

void ControlByEvent::downHandler(const Event* event, void* data)
{
	mDown = (bool) (*data);
}

void ControlByEvent::forwardHandler(const Event* event, void* data)
{
	mForward = (bool) (*data);
}

void ControlByEvent::strafeLeftHandler(const Event* event, void* data)
{
	mStrafeLeft = (bool) (*data);
}

void ControlByEvent::strafeRightHandler(const Event* event, void* data)
{
	mStrafeRight = (bool) (*data);
}

void ControlByEvent::rollLeftHandler(const Event* event, void* data)
{
	mRollLeft = (bool) (*data);
}

void ControlByEvent::rollRightHandler(const Event* event, void* data)
{
	mRollRight = (bool) (*data);
}

void ControlByEvent::upHandler(const Event* event, void* data)
{
	mUp = (bool) (*data);
}

AsyncTask::DoneStatus ControlByEvent::update(GenericAsyncTask* task)
{
	return AsyncTask::DS_done;
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEvent::_type_handle;
