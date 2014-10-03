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
 * \file /Ely/src/CommonComponents/GameConfigTemplate.cpp
 *
 * \date 28/set/2014 (11:09:29)
 * \author consultit
 */

#include "CommonComponents/GameConfigTemplate.h"
#include "CommonComponents/GameConfig.h"

namespace ely
{

GameConfigTemplate::GameConfigTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	//
	setParametersDefaults();
}

GameConfigTemplate::~GameConfigTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType GameConfigTemplate::componentType() const
{
	return ComponentType(GameConfig::get_class_type().get_name());
}

ComponentFamilyType GameConfigTemplate::familyType() const
{
	return ComponentFamilyType("Common");
}

SMARTPTR(Component)GameConfigTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(GameConfig) newGameConfig = new GameConfig(this);
	newGameConfig->setComponentId(compId);
	if (not newGameConfig->initialize())
	{
		return NULL;
	}
	return newGameConfig.p();
}

void GameConfigTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
}

//TypedObject semantics: hardcoded
TypeHandle GameConfigTemplate::_type_handle;

} /* namespace ely */
