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
 * \file /Ely/src/SceneComponents/ModelTemplate.cpp
 *
 * \date 16/mag/2012 (16:26:41)
 * \author marco
 */

#include "SceneComponents/ModelTemplate.h"

ModelTemplate::ModelTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework)
{
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ModelTemplate::ModelTemplate: invalid PandaFramework or WindowFramework");

	}
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
	setParametersDefaults();
}

ModelTemplate::~ModelTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentType ModelTemplate::componentType() const
{
	return ComponentType("Model");
}

const ComponentFamilyType ModelTemplate::familyType() const
{
	return ComponentFamilyType("Scene");
}

Component* ModelTemplate::makeComponent(const ComponentId& compId)
{
	Model* newModel = new Model(this);
	newModel->componentId() = compId;
	if (not newModel->initialize())
	{
		return NULL;
	}
	return newModel;
}

PandaFramework*& ModelTemplate::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& ModelTemplate::windowFramework()
{
	return mWindowFramework;
}

void ModelTemplate::setParametersDefaults()
{
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("is_static","false"));
}

//TypedObject semantics: hardcoded
TypeHandle ModelTemplate::_type_handle;
