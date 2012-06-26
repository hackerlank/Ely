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
	resetParameters();
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

void ModelTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTable::iterator iter;
	pair<ParameterTable::iterator, ParameterTable::iterator> iterRange;
	CASEITER(parameterTable,iter,"model_file",mModelFile)
	CASEITER(parameterTable,iter,"is_static",mIsStatic)
	CASERANGE(parameterTable,iter,iterRange,"anim_files",mAnimFiles)
}

void ModelTemplate::resetParameters()
{
	//set component parameters to their default values
	mModelFile = std::string("");
	mAnimFiles.clear();
	mIsStatic = std::string("false");
}

std::string& ModelTemplate::parameter(const std::string& paramName)
{
	std::string* strPtr = &mUnknown;
	CASE(paramName,strPtr,"is_static",mIsStatic)
	CASE(paramName,strPtr,"model_file",mModelFile)
	//
	return *strPtr;
}

std::list<std::string>&  ModelTemplate::parameterList(const std::string& paramName)
{
	std::list<std::string>* strListPtr = &mUnknownList;
	CASE(paramName,strListPtr,"anim_files",mAnimFiles)
	//
	return *strListPtr;
}

//TypedObject semantics: hardcoded
TypeHandle ModelTemplate::_type_handle;
