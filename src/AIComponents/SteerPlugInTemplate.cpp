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
 * \file /Ely/src/AIComponents/SteerPlugInTemplate.cpp
 *
 * \date 04/dic/2013 (09:13:41)
 * \author consultit
 */
#include "AIComponents/SteerPlugInTemplate.h"
#include "AIComponents/SteerPlugIn.h"
#include "Game/GameAIManager.h"

namespace ely
{

SteerPlugInTemplate::SteerPlugInTemplate(PandaFramework* pandaFramework,
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

SteerPlugInTemplate::~SteerPlugInTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType SteerPlugInTemplate::componentType() const
{
	return ComponentType(SteerPlugIn::get_class_type().get_name());
}

ComponentFamilyType SteerPlugInTemplate::familyType() const
{
	return ComponentFamilyType("AI");
}

SMARTPTR(Component)SteerPlugInTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(SteerPlugIn) newOpenSteerPlugIn = new SteerPlugIn(this);
	newOpenSteerPlugIn->setComponentId(compId);
	if (not newOpenSteerPlugIn->initialize())
	{
		return NULL;
	}
	return newOpenSteerPlugIn.p();
}

void SteerPlugInTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("type", "one_turning"));
	mParameterTable.insert(ParameterNameValue("pathway", "0.0,0.0,0.0:1.0,1.0,1.0$1.0$false"));
}

//TypedObject semantics: hardcoded
TypeHandle SteerPlugInTemplate::_type_handle;

} /* namespace ely */
