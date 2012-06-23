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

ControlByEventTemplate::ControlByEventTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ControlByEventTemplate::ControlByEventTemplate: invalid PandaFramework or WindowFramework");
	}
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
	resetParameters();
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

void ControlByEventTemplate::resetParameters()
{
	//set component parameters to their default values
	mForward = std::string("w");
	mRollLeft = std::string("a");
	mBackward = std::string("s");
	mRollRight = std::string("d");
	mStrafeLeft = std::string("q");
	mStrafeRight = std::string("e");
	mUp = std::string("r");
	mDown = std::string("f");
	mSpeedKey = std::string("shift");
	mSpeed = std::string("100.0");
	mFastFactor = std::string("5.0");
	mMovSens = std::string("2.0");
	mRollSens = std::string("15.0");
	mSensX = std::string("0.2");
	mSensY = std::string("0.2");
	mInverted = std::string("false");
	mMouseEnabledH = std::string("false");
	mMouseEnabledP = std::string("false");
	mEnabled = std::string("true");
}

PandaFramework*& ControlByEventTemplate::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& ControlByEventTemplate::windowFramework()
{
	return mWindowFramework;
}

std::string& ControlByEventTemplate::backwardEvent()
{
	return mBackward;
}

std::string& ControlByEventTemplate::speedKey()
{
	return mSpeedKey;
}

std::string& ControlByEventTemplate::inverted()
{
	return mInverted;
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

std::string& ControlByEventTemplate::speed()
{
	return mSpeed;
}

std::string& ControlByEventTemplate::fastFactor()
{
	return mFastFactor;
}

std::string& ControlByEventTemplate::movSens()
{
	return mMovSens;
}

std::string& ControlByEventTemplate::mouseEnabledH()
{
	return mMouseEnabledH;
}

std::string& ControlByEventTemplate::mouseEnabledP()
{
	return mMouseEnabledP;
}

std::string& ControlByEventTemplate::enabled()
{
	return mEnabled;
}

std::string& ControlByEventTemplate::rollSens()
{
	return mRollSens;
}

std::string& ControlByEventTemplate::sensX()
{
	return mSensX;
}

std::string& ControlByEventTemplate::sensY()
{
	return mSensY;
}

std::string& ControlByEventTemplate::getParam(const std::string& name)
{
	std::string* str;
	if (name == std::string("enabled"))
	{
		str = &mEnabled;
	}
	if (name == std::string("mouse_enabled_h"))
	{
		mMouseEnabledH = iter->second;
	}
	if (name == std::string("mouse_enabled_p"))
	{
		mMouseEnabledP = iter->second;
	}
	if (name == std::string("forward"))
	{
		mForward = iter->second;
	}
	if (name == std::string("roll_left"))
	{
		mRollLeft = iter->second;
	}
	if (name == std::string("backward"))
	{
		mBackward = iter->second;
	}
	iter = parameterTable.find("roll_right");
	if (iter != parameterTable.end())
	{
		mRollRight = iter->second;
	}
	iter = parameterTable.find("strafe_left");
	if (iter != parameterTable.end())
	{
		mStrafeLeft = iter->second;
	}
	iter = parameterTable.find("strafe_right");
	if (iter != parameterTable.end())
	{
		mStrafeRight = iter->second;
	}
	iter = parameterTable.find("up");
	if (iter != parameterTable.end())
	{
		mUp = iter->second;
	}
	iter = parameterTable.find("down");
	if (iter != parameterTable.end())
	{
		mDown = iter->second;
	}
	iter = parameterTable.find("speed_key");
	if (iter != parameterTable.end())
	{
		mSpeedKey = iter->second;
	}
	iter = parameterTable.find("inverted");
	if (iter != parameterTable.end())
	{
		mInverted = iter->second;
	}
	//set sensitivity parameters
	iter = parameterTable.find("speed");
	if (iter != parameterTable.end())
	{
		mSpeed = iter->second;
	}
	iter = parameterTable.find("fast_factor");
	if (iter != parameterTable.end())
	{
		mFastFactor = iter->second;
	}
	iter = parameterTable.find("mov_sens");
	if (iter != parameterTable.end())
	{
		mMovSens = iter->second;
	}
	iter = parameterTable.find("roll_sens");
	if (iter != parameterTable.end())
	{
		mRollSens = iter->second;
	}
	iter = parameterTable.find("sens_x");
	if (iter != parameterTable.end())
	{
		mSensX = iter->second;
	}
	iter = parameterTable.find("sens_y");
	if (iter != parameterTable.end())
	{
		mSensY = iter->second;
	}
	//
	return *str;
}

void ControlByEventTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTable::iterator iter;
	//set (initial) enabling
	iter = parameterTable.find("enabled");
	if (iter != parameterTable.end())
	{
		mEnabled = iter->second;
	}
	//set mouse control
	iter = parameterTable.find("mouse_enabled_h");
	if (iter != parameterTable.end())
	{
		mMouseEnabledH = iter->second;
	}
	iter = parameterTable.find("mouse_enabled_p");
	if (iter != parameterTable.end())
	{
		mMouseEnabledP = iter->second;
	}
	//set event key names
	iter = parameterTable.find("forward");
	if (iter != parameterTable.end())
	{
		mForward = iter->second;
	}
	iter = parameterTable.find("roll_left");
	if (iter != parameterTable.end())
	{
		mRollLeft = iter->second;
	}
	iter = parameterTable.find("backward");
	if (iter != parameterTable.end())
	{
		mBackward = iter->second;
	}
	iter = parameterTable.find("roll_right");
	if (iter != parameterTable.end())
	{
		mRollRight = iter->second;
	}
	iter = parameterTable.find("strafe_left");
	if (iter != parameterTable.end())
	{
		mStrafeLeft = iter->second;
	}
	iter = parameterTable.find("strafe_right");
	if (iter != parameterTable.end())
	{
		mStrafeRight = iter->second;
	}
	iter = parameterTable.find("up");
	if (iter != parameterTable.end())
	{
		mUp = iter->second;
	}
	iter = parameterTable.find("down");
	if (iter != parameterTable.end())
	{
		mDown = iter->second;
	}
	iter = parameterTable.find("speed_key");
	if (iter != parameterTable.end())
	{
		mSpeedKey = iter->second;
	}
	iter = parameterTable.find("inverted");
	if (iter != parameterTable.end())
	{
		mInverted = iter->second;
	}
	//set sensitivity parameters
	iter = parameterTable.find("speed");
	if (iter != parameterTable.end())
	{
		mSpeed = iter->second;
	}
	iter = parameterTable.find("fast_factor");
	if (iter != parameterTable.end())
	{
		mFastFactor = iter->second;
	}
	iter = parameterTable.find("mov_sens");
	if (iter != parameterTable.end())
	{
		mMovSens = iter->second;
	}
	iter = parameterTable.find("roll_sens");
	if (iter != parameterTable.end())
	{
		mRollSens = iter->second;
	}
	iter = parameterTable.find("sens_x");
	if (iter != parameterTable.end())
	{
		mSensX = iter->second;
	}
	iter = parameterTable.find("sens_y");
	if (iter != parameterTable.end())
	{
		mSensY = iter->second;
	}
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEventTemplate::_type_handle;

