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

namespace ely
{

GameConfig::GameConfig()
{
	// TODO Auto-generated constructor stub
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

//TypedObject semantics: hardcoded
TypeHandle GameConfig::_type_handle;

} /* namespace ely */
