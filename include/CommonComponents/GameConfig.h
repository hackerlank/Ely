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
 * \file /Ely/include/CommonComponents/GameConfig.h
 *
 * \date 28/set/2014 (11:07:27)
 * \author consultit
 */

#ifndef GAMECONFIG_H_
#define GAMECONFIG_H_

#include "ObjectModel/Component.h"

namespace ely
{
class GameConfigTemplate;

/**
 * \brief Component representing the overall game configuration at startup.
 *
 * Game configuration is done through both xml parameters and its methods.
 * .
 * > *Collision notify*
 * This configuration allows throwing events when objects collide. It is performed by
 * the GamePhysicsManager and, by default, it's disabled.\n
 * It can be enabled at startup by specifying "collision_notify@<EVENTNAME>@<FREQUENCY>" in "thrown_events"
 * xml parameter, with <EVENTNAME> ignored (so it could be empty).
 * \see GamePhysicsManager documentation for details.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *thrown_events* 			|single| - | specified as "event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]" with eventX = collision_notify
 *
 * \note parts inside [] are optional.\n
 */

class GameConfig: public Component
{
protected:
	friend class GameConfigTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	GameConfig();
	GameConfig(SMARTPTR(GameConfigTemplate)tmpl);
	virtual ~GameConfig();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

private:

	/**
	 * \name Throwing notify collision events.
	 */
	///@{
	///Helper.
	std::string mThrownEventsParam;
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
		register_type(_type_handle, "GameConfig", Component::get_class_type());
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

///inline definitions

inline void GameConfig::reset()
{
	//
	mThrownEventsParam.clear();
}

inline void GameConfig::onRemoveFromObjectCleanup()
{
}

inline void GameConfig::onAddToSceneSetup()
{
}

inline void GameConfig::onRemoveFromSceneCleanup()
{
}

} /* namespace ely */

#endif /* GAMECONFIG_H_ */
