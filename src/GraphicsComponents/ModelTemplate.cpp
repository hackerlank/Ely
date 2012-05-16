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

ModelTemplate::ModelTemplate()
{
	// TODO Auto-generated constructor stub

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
	newModel->preSetup();
	return newModel;
}

void ModelTemplate::setModelFile(const std::list<std::string>& modelFile)
{
	mModelFile = modelFile;
}

std::string& ModelTemplate::getModelFile() const
{
	return mModelFile;
}

void ModelTemplate::setAnimFiles(const std::string& animFile)
{
	mAnimFiles = animFile;
}

std::list<std::string>& ModelTemplate::getAnimFiles() const
{
	return mAnimFiles;
}

