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
		mTmpl(tmpl), mForward(false), mBackward(false), mLeft(false), mRight(
				false), mUp(false), mDown(false), mRollLeft(false), mRollRight(
				false), mSpeed(1.0), mSpeedFast(5.0)
{
}

ControlByEvent::~ControlByEvent()
{
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
	//register the handlers
	std::string speedKeyEvent = speedKey + "-" + mTmpl->backwardEvent();
	std::string upKeyEvent = mTmpl->backwardEvent() + "-up";
	mTmpl->pandaFramework()->define_key(mTmpl->backwardEvent(), "backward",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::backwardHandler>,
			NULL);

	mTmpl->pandaFramework()->define_key(mTmpl->downEvent(), "down",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::downHandler>,
			NULL);
	mTmpl->pandaFramework()->define_key(mTmpl->forwardEvent(), "forward",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::forwardHandler>,
			NULL);
	mTmpl->pandaFramework()->define_key(mTmpl->strafeLeftEvent(), "strafeLeft",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeLeftHandler>, NULL);
	mTmpl->pandaFramework()->define_key(mTmpl->strafeRightEvent(),
			"strafeRight",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::strafeRightHandler>, NULL);
	mTmpl->pandaFramework()->define_key(mTmpl->rollLeftEvent(), "rollLeft",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::rollLeftHandler>,
			NULL);
	mTmpl->pandaFramework()->define_key(mTmpl->rollRightEvent(), "rollRight",
			&handlerTmpl<ControlByEvent, *this,
					&ControlByEvent::rollRightHandler>, NULL);
	mTmpl->pandaFramework()->define_key(mTmpl->upEvent(), "up",
			&handlerTmpl<ControlByEvent, *this, &ControlByEvent::upHandler>,
			NULL);
	return result;
}

void ControlByEvent::onAddSetup()
{
	//add the task for updating the controlled object
	mUpdateData = new TaskInterface<ControlByEvent>::TaskData(this,
			&ControlByEvent::update);
	AsyncTask* task = new GenericAsyncTask("update task",
			&TaskInterface<ControlByEvent>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	mTmpl->pandaFramework()->get_task_mgr().add(task);
}

void ControlByEvent::backwardHandler(const Event* event, void* data)
{
}

void ControlByEvent::downHandler(const Event* event, void* data)
{
}

void ControlByEvent::forwardHandler(const Event* event, void* data)
{
}

void ControlByEvent::strafeLeftHandler(const Event* event, void* data)
{
}

void ControlByEvent::strafeRightHandler(const Event* event, void* data)
{
}

void ControlByEvent::rollLeftHandler(const Event* event, void* data)
{
}

void ControlByEvent::rollRightHandler(const Event* event, void* data)
{
}

void ControlByEvent::upHandler(const Event* event, void* data)
{
}

AsyncTask::DoneStatus ControlByEvent::update(GenericAsyncTask* task)
{
	return AsyncTask::DS_done;
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEvent::_type_handle;
