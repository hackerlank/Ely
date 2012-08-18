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
 * \file /Ely/src/AudioComponents/ListenerTemplate.cpp
 *
 * \date 27/giu/2012 (16:42:33)
 * \author marco
 */

#include "AudioComponents/ListenerTemplate.h"

ListenerTemplate::ListenerTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ListenerTemplate::ListenerTemplate: invalid PandaFramework or WindowFramework");
	}
	if (not GameAudioManager::GetSingletonPtr())
	{
		throw GameException(
				"ListenerTemplate::ListenerTemplate: invalid GameAudioManager");
	}
	setParametersDefaults();
}

ListenerTemplate::~ListenerTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType ListenerTemplate::componentType() const
{
	return ComponentType("Listener");
}

const ComponentFamilyType ListenerTemplate::familyType() const
{
	return ComponentFamilyType("Audio");
}

Component* ListenerTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	Listener* newListener = new Listener(this);
	newListener->setComponentId(compId);
	if (not newListener->initialize())
	{
		return NULL;
	}
	return newListener;
}

void ListenerTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	//no mandatory parameters
}

WindowFramework* const ListenerTemplate::windowFramework() const
{
	return mWindowFramework;
}

//TypedObject semantics: hardcoded
TypeHandle ListenerTemplate::_type_handle;

