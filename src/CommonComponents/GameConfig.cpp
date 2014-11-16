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
 * \file /Ely/src/CommonComponents/GameConfig.cpp
 *
 * \date 28/set/2014 (11:07:27)
 * \author consultit
 */

#include "CommonComponents/GameConfig.h"
#include "CommonComponents/GameConfigTemplate.h"
#include "ObjectModel/Object.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

GameConfig::GameConfig()
{
	reset();
}

GameConfig::GameConfig(SMARTPTR(GameConfigTemplate)tmpl)
{
	mTmpl = tmpl;
	reset();
}

GameConfig::~GameConfig()
{
}

ComponentFamilyType GameConfig::familyType() const
{
	return mTmpl->familyType();
}

ComponentType GameConfig::componentType() const
{
	return mTmpl->componentType();
}

bool GameConfig::initialize()
{
	bool result = true;
	//thrown events
	mThrownEventsParam = mTmpl->parameter(std::string("thrown_events"));
	//
	return result;
}

void GameConfig::onAddToObjectSetup()
{
	//set thrown events if any
	unsigned int idx1, valueNum1;
	std::vector<std::string> paramValuesStr1, paramValuesStr2;
	if (mThrownEventsParam != std::string(""))
	{
		//events specified
		//event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]
		paramValuesStr1 = parseCompoundString(mThrownEventsParam, ':');
		valueNum1 = paramValuesStr1.size();
		for (idx1 = 0; idx1 < valueNum1; ++idx1)
		{
			//eventX@[event_nameX]@[frequencyX]
			paramValuesStr2 = parseCompoundString(paramValuesStr1[idx1], '@');
			if (paramValuesStr2.size() >= 3)
			{
				ThrowEventData eventData;
				//get default name prefix
				std::string objectType = std::string(
						mOwnerObject->objectTmpl()->objectType());
				//get name
				std::string name = paramValuesStr2[1];
				//get frequency
				float frequency = strtof(paramValuesStr2[2].c_str(), NULL);
				if (frequency <= 0.0)
				{
					frequency = 30.0;
				}
				//get event
				if (paramValuesStr2[0] == "collision_notify")
				{
					GamePhysicsManager::EventThrown event;
					event = GamePhysicsManager::COLLISIONNOTIFY;
					//set event data
					eventData.mEnable = true;
					eventData.mEventName = "collision_notify"; //must not be null
					eventData.mTimeElapsed = 0;
					eventData.mFrequency = frequency;
					//enable the event
					GamePhysicsManager::GetSingletonPtr()->enableCollisionNotify(event, eventData);
				}
				else
				{
					//paramValuesStr2[0] is not a suitable event:
					//continue with the next event
					continue;
				}
			}
		}
	}
	//clear all no more needed "Param" variables
	mThrownEventsParam.clear();
}

//TypedObject semantics: hardcoded
TypeHandle GameConfig::_type_handle;

} /* namespace ely */
