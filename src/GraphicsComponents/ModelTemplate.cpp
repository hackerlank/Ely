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
 * \file /Ely/src/GraphicsComponents/ModelTemplate.cpp
 *
 * \date 16/mag/2012 (16:26:41)
 * \author marco
 */

#include "GraphicsComponents/ModelTemplate.h"

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
	reset();
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
	return ComponentFamilyType("Graphics");
}

Component* ModelTemplate::makeComponent(ComponentId& compId)
{
	Model* newModel = new Model(this);
	newModel->componentId() = compId;
	if (not newModel->initialize())
	{
		return NULL;
	}
	return newModel;
}

Filename& ModelTemplate::modelFile()
{
	return mModelFile;
}

std::list<Filename>& ModelTemplate::animFiles()
{
	return mAnimFiles;
}

PandaFramework*& ModelTemplate::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& ModelTemplate::windowFramework()
{
	return mWindowFramework;
}

void ModelTemplate::reset()
{
	mModelFile = Filename("");
	mAnimFiles.clear();
}

//TypedObject semantics: hardcoded
TypeHandle ModelTemplate::_type_handle;
