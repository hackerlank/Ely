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
 * \author consultit
 */

#include "AudioComponents/ListenerTemplate.h"

using namespace ely;

ListenerTemplate::ListenerTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"ListenerTemplate::ListenerTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"ListenerTemplate::ListenerTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameAudioManager::GetSingletonPtr(),
			"ListenerTemplate::ListenerTemplate: invalid GameAudioManager")
	//
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

SMARTPTR(Component)ListenerTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(Listener) newListener = new Listener(this);
	newListener->setComponentId(compId);
	if (not newListener->initialize())
	{
		return NULL;
	}
	return newListener.p();
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

//TypedObject semantics: hardcoded
TypeHandle ListenerTemplate::_type_handle;

