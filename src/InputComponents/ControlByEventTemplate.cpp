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
 * \file /Ely/src/InputComponents/ControlByEventTemplate.cpp
 *
 * \date 31/mag/2012 (16:44:06)
 * \author marco
 */

#include "InputComponents/ControlByEventTemplate.h"

ControlByEventTemplate::ControlByEventTemplate(PandaFramework* pandaFramework)
{
	if (not pandaFramework)
	{
		throw GameException(
				"ControlByEventTemplate::ControlByEventTemplate: invalid PandaFramework");

	}
	mPandaFramework = pandaFramework;
	reset();
}

ControlByEventTemplate::~ControlByEventTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType ControlByEventTemplate::componentType() const
{
	return ComponentType("ControlByEvent");
}

const ComponentFamilyType ControlByEventTemplate::familyType() const
{
	return ComponentFamilyType("Input");
}

Component* ControlByEventTemplate::makeComponent(ComponentId& compId)
{
	ControlByEvent* newControl = new ControlByEvent(this);
	newControl->componentId() = compId;
	if (not newControl->initialize())
	{
		return NULL;
	}
	return newControl;
}

void ControlByEventTemplate::reset()
{
	mForward = std::string("w");
	mRollLeft = std::string("a");
	mBackward = std::string("s");
	mRollRight = std::string("d");
	mStrafeLeft = std::string("q");
	mStrafeRight = std::string("e");
	mUp = std::string("r");
	mDown = std::string("f");
	mSpeedKey = std::string("shift");
}

PandaFramework*& ControlByEventTemplate::pandaFramework()
{
	return mPandaFramework;
}

std::string& ControlByEventTemplate::backwardEvent()
{
	return mBackward;
}

std::string& ControlByEventTemplate::speedKey()
{
	return mSpeedKey;
}

std::string& ControlByEventTemplate::downEvent()
{
	return mDown;
}

std::string& ControlByEventTemplate::forwardEvent()
{
	return mForward;
}

std::string& ControlByEventTemplate::strafeLeftEvent()
{
	return mStrafeLeft;
}

std::string& ControlByEventTemplate::strafeRightEvent()
{
	return mStrafeRight;
}

std::string& ControlByEventTemplate::rollLeftEvent()
{
	return mRollLeft;
}

std::string& ControlByEventTemplate::rollRightEvent()
{
	return mRollRight;
}

std::string& ControlByEventTemplate::upEvent()
{
	return mUp;
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEventTemplate::_type_handle;

