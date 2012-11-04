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
 * \file /Ely/src/AudioComponents/Sound3dTemplate.cpp
 *
 * \date 20/giu/2012 (12:48:00)
 * \author marco
 */

#include "AudioComponents/Sound3dTemplate.h"

Sound3dTemplate::Sound3dTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"Sound3dTemplate::Sound3dTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"Sound3dTemplate::Sound3dTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameAudioManager::GetSingletonPtr(),
			"Sound3dTemplate::Sound3dTemplate: invalid GameAudioManager")
	//
	setParametersDefaults();
}

Sound3dTemplate::~Sound3dTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType Sound3dTemplate::componentType() const
{
	return ComponentType("Sound3d");
}

const ComponentFamilyType Sound3dTemplate::familyType() const
{
	return ComponentFamilyType("Audio");
}

SMARTPTR(Component) Sound3dTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(Sound3d) newSound3d = new Sound3d(this);
	newSound3d->setComponentId(compId);
	if (not newSound3d->initialize())
	{
		return NULL;
	}
	return newSound3d.p();
}

void Sound3dTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	//no mandatory parameters
}

//TypedObject semantics: hardcoded
TypeHandle Sound3dTemplate::_type_handle;

