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
 * \file /Ely/src/SceneComponents/NodePathWrapperTemplate.cpp
 *
 * \date 28/giu/2012 (20:15:15)
 * \author marco
 */

#include "SceneComponents/NodePathWrapperTemplate.h"

NodePathWrapperTemplate::NodePathWrapperTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"NodePathWrapperTemplate::NodePathWrapperTemplate: invalid PandaFramework or WindowFramework");

	}
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
	setParametersDefaults();

}

NodePathWrapperTemplate::~NodePathWrapperTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType NodePathWrapperTemplate::componentType() const
{
	return ComponentType("NodePathWrapper");
}

const ComponentFamilyType NodePathWrapperTemplate::familyType() const
{
	return ComponentFamilyType("Scene");
}

Component* NodePathWrapperTemplate::makeComponent(const ComponentId& compId)
{
	NodePathWrapper* newNodePathWrapper = new NodePathWrapper(this);
	newNodePathWrapper->componentId() = compId;
	if (not newNodePathWrapper->initialize())
	{
		return NULL;
	}
	return newNodePathWrapper;
}

void NodePathWrapperTemplate::setParametersDefaults()
{
	//sets the (mandatory) parameters to their default values:
	//no mandatory parameters
}

PandaFramework*& NodePathWrapperTemplate::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& NodePathWrapperTemplate::windowFramework()
{
	return mWindowFramework;
}

//TypedObject semantics: hardcoded
TypeHandle NodePathWrapperTemplate::_type_handle;
