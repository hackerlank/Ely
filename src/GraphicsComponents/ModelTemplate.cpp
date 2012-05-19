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
#include "GraphicsComponents/Model.h"

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
	mParent = NodePath();
	mInitPosition = LVecBase3(0.0, 0.0, 0.0);
	mInitOrientation = LVecBase3(0.0, 0.0, 0.0);
}

ModelTemplate::~ModelTemplate()
{
	// TODO Auto-generated destructor stub
}

const ComponentId ModelTemplate::componentID() const
{
	return ComponentId("Model");
}

const ComponentFamilyId ModelTemplate::familyID() const
{
	return ComponentFamilyId("Graphics");
}

Component* ModelTemplate::makeComponent()
{
	Model* newModel = new Model(this);
	newModel->initialize();
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

LVecBase3& ModelTemplate::initOrientation()
{
	return mInitOrientation;
}

LVecBase3& ModelTemplate::initPosition()
{
	return mInitPosition;
}

NodePath& ModelTemplate::parent()
{
	return mParent;
}

