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
	mForward = std::string("w");
	mRollLeft = std::string("a");
	mBackward = std::string("s");
	mRollRight = std::string("d");
	mStrafeLeft = std::string("q");
	mStrafeRight = std::string("e");
	mUp = std::string("r");
	mDown = std::string("f");
	mSpeedKey = std::string("shift");
	mSpeed = 100.0;
	mFastFactor = 5.0;
	mMovSens = 2.0;
	mRollSens = 15.0;
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

float& ControlByEventTemplate::speed()
{
	return mSpeed;
}

float& ControlByEventTemplate::fastFactor()
{
	return mFastFactor;
}

float& ControlByEventTemplate::movSens()
{
	return mMovSens;
}

void ControlByEventTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTable::iterator iter;
	//set event key names
	iter = parameterTable.find("Forward");
	if (iter != parameterTable.end())
	{
		mForward = iter->second;
	}
	iter = parameterTable.find("RollLeft");
	if (iter != parameterTable.end())
	{
		mRollLeft = iter->second;
	}
	iter = parameterTable.find("Backward");
	if (iter != parameterTable.end())
	{
		mBackward = iter->second;
	}
	iter = parameterTable.find("RollRight");
	if (iter != parameterTable.end())
	{
		mRollRight = iter->second;
	}
	iter = parameterTable.find("StrafeLeft");
	if (iter != parameterTable.end())
	{
		mStrafeLeft = iter->second;
	}
	iter = parameterTable.find("StrafeRight");
	if (iter != parameterTable.end())
	{
		mStrafeRight = iter->second;
	}
	iter = parameterTable.find("Up");
	if (iter != parameterTable.end())
	{
		mUp = iter->second;
	}
	iter = parameterTable.find("Down");
	if (iter != parameterTable.end())
	{
		mDown = iter->second;
	}
	iter = parameterTable.find("SpeedKey");
	if (iter != parameterTable.end())
	{
		mSpeedKey = iter->second;
	}
	//set sensitivity parameters
	iter = parameterTable.find("Speed");
	if (iter != parameterTable.end())
	{
		mSpeed = (float) atof(iter->second.c_str());
	}
	iter = parameterTable.find("FastFactor");
	if (iter != parameterTable.end())
	{
		mFastFactor = (float) atof(iter->second.c_str());
	}
	iter = parameterTable.find("MovSens");
	if (iter != parameterTable.end())
	{
		mMovSens = (float) atof(iter->second.c_str());
	}
	iter = parameterTable.find("RollSens");
	if (iter != parameterTable.end())
	{
		mRollSens = (float) atof(iter->second.c_str());
	}
}

float& ControlByEventTemplate::rollSens()
{
	return mRollSens;
}

//TypedObject semantics: hardcoded
TypeHandle ControlByEventTemplate::_type_handle;

