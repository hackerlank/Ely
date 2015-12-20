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
 * \file /Ely/src/CommonComponents/Default.cpp
 *
 * \date 2014-08-27 
 * \author consultit
 */

#include "CommonComponents/Default.h"

namespace ely
{

Default::Default(SMARTPTR(DefaultTemplate)tmpl)
{
	mTmpl = tmpl;
	reset();
}

Default::~Default()
{
}

//TypedObject semantics: hardcoded
TypeHandle Default::_type_handle;

//MemoryPool semantics: hardcoded
GCC_MEMORYPOOL_DEFINITION(Default);

///Template

DefaultTemplate::DefaultTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	//
	setParametersDefaults();
}

DefaultTemplate::~DefaultTemplate()
{
	
}

ComponentType DefaultTemplate::componentType() const
{
	return ComponentType(Default::get_class_type().get_name());
}

ComponentFamilyType DefaultTemplate::componentFamilyType() const
{
	return ComponentFamilyType("Common");
}

SMARTPTR(Component)DefaultTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(Default) newDefault = new Default(this);
	newDefault->setComponentId(compId);
	if (not newDefault->initialize())
	{
		return NULL;
	}
	return newDefault.p();
}

void DefaultTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
}

//TypedObject semantics: hardcoded
TypeHandle DefaultTemplate::_type_handle;

} /* namespace ely */
