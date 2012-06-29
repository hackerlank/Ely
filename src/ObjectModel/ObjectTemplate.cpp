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
 * \file /Ely/src/ObjectModel/ObjectTemplate.cpp
 *
 * \date 12/mag/2012 (19:34:28)
 * \author marco
 */

#include "ObjectModel/ObjectTemplate.h"

ObjectTemplate::ObjectTemplate(const ObjectType& name,
		ObjectTemplateManager* objectTmplMgr, PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		mName(name)
{
	if (not objectTmplMgr or not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ObjectTemplate::ObjectTemplate: invalid ObjectTemplateManager or "
						"PandaFramework or WindowFramework");

	}
	mObjectTmplMgr = objectTmplMgr;
	mPandaFramework = pandaFramework;
	mWindowFramework = windowFramework;
	//reset parameters
	resetParameters();
}

ObjectTemplate::~ObjectTemplate()
{
	// TODO Auto-generated destructor stub
}

ObjectType& ObjectTemplate::name()
{
	return mName;
}

void ObjectTemplate::clearComponentTemplates()
{
	mComponentTemplates.clear();
}

ObjectTemplate::ComponentTemplateList& ObjectTemplate::getComponentTemplates()
{
	return mComponentTemplates;
}

void ObjectTemplate::addComponentTemplate(ComponentTemplate* componentTmpl)
{
	if (not componentTmpl)
	{
		throw GameException(
				"ObjectTemplate::addComponentTemplate: NULL component template");
	}
	mComponentTemplates.push_back(componentTmpl);
}

ComponentTemplate* ObjectTemplate::getComponentTemplate(
		const ComponentType& componentId)
{
	ComponentTemplateList::iterator it;
	it = find_if(mComponentTemplates.begin(), mComponentTemplates.end(),
			idIsEqualTo(componentId));
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	return *it;
}

ObjectTemplateManager*& ObjectTemplate::objectTmplMgr()
{
	return mObjectTmplMgr;
}

PandaFramework*& ObjectTemplate::pandaFramework()
{
	return mPandaFramework;
}

WindowFramework*& ObjectTemplate::windowFramework()
{
	return mWindowFramework;
}

void ObjectTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTable::iterator iter;
	CASEITER(parameterTable,iter,"parent",mParent)
	CASEITER(parameterTable,iter,"is_static",mIsStatic)
	CASEITER(parameterTable,iter,"pos_x",mPosX)
	CASEITER(parameterTable,iter,"pos_y",mPosY)
	CASEITER(parameterTable,iter,"pos_z",mPosZ)
	CASEITER(parameterTable,iter,"rot_h",mRotH)
	CASEITER(parameterTable,iter,"rot_p",mRotP)
	CASEITER(parameterTable,iter,"rot_r",mRotR)
	CASEITER(parameterTable,iter,"scale_x",mScaleX)
	CASEITER(parameterTable,iter,"scale_y",mScaleY)
	CASEITER(parameterTable,iter,"scale_z",mScaleZ)
}

void ObjectTemplate::resetParameters()
{
	//set component parameters to their default values
	mParent = std::string("");
	mIsStatic = std::string("false");
	mPosX = std::string("0.0");
	mPosY = std::string("0.0");
	mPosZ = std::string("0.0");
	mRotH = std::string("0.0");
	mRotP = std::string("0.0");
	mRotR = std::string("0.0");
	mScaleX = std::string("1.0");
	mScaleY = std::string("1.0");
	mScaleZ = std::string("1.0");
}

std::string& ObjectTemplate::parameter(const std::string& paramName)
{
	std::string* strPtr = &mUnknown;
	CASE(paramName, strPtr, "parent", mParent)
	CASE(paramName, strPtr, "is_static", mIsStatic)
	CASE(paramName, strPtr, "pos_x", mPosX)
	CASE(paramName, strPtr, "pos_y", mPosY)
	CASE(paramName, strPtr, "pos_z", mPosZ)
	CASE(paramName, strPtr, "rot_h", mRotH)
	CASE(paramName, strPtr, "rot_p", mRotP)
	CASE(paramName, strPtr, "rot_r", mRotR)
	CASE(paramName, strPtr, "scale_x", mScaleX)
	CASE(paramName, strPtr, "scale_y", mScaleY)
	CASE(paramName, strPtr, "scale_z", mScaleZ)
	//
	return *strPtr;
}

std::list<std::string>& ObjectTemplate::parameterList(const std::string& paramName)
{
	std::list<std::string>* strListPtr = &mUnknownList;
	//
	return *strListPtr;
}

//TypedObject semantics: hardcoded
TypeHandle ObjectTemplate::_type_handle;
