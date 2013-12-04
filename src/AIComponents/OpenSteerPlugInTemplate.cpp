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
 * \file /Ely/src/AIComponents/OpenSteerPlugInTemplate.cpp
 *
 * \date 04/dic/2013 (09:13:41)
 * \author consultit
 */
#include "AIComponents/OpenSteerPlugInTemplate.h"
#include "AIComponents/OpenSteerPlugIn.h"
#include "Game/GameAIManager.h"

namespace ely
{

OpenSteerPlugInTemplate::OpenSteerPlugInTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"OpenSteerPlugInTemplate::OpenSteerPlugInTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"OpenSteerPlugInTemplate::OpenSteerPlugInTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"OpenSteerPlugInTemplate::OpenSteerPlugInTemplate: invalid GameAIManager")
	//
	setParametersDefaults();
}

OpenSteerPlugInTemplate::~OpenSteerPlugInTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType OpenSteerPlugInTemplate::componentType() const
{
	return ComponentType("OpenSteerPlugIn");
}

ComponentFamilyType OpenSteerPlugInTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)OpenSteerPlugInTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(OpenSteerPlugIn) newOpenSteerPlugIn = new OpenSteerPlugIn(this);
	newOpenSteerPlugIn->setComponentId(compId);
	if (not newOpenSteerPlugIn->initialize())
	{
		return NULL;
	}
	return newOpenSteerPlugIn.p();
}

void OpenSteerPlugInTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("plugin_type", ""));
}

//TypedObject semantics: hardcoded
TypeHandle OpenSteerPlugInTemplate::_type_handle;

} /* namespace ely */
