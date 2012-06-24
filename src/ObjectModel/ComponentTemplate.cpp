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
 * \file /Ely/src/ObjectModel/ComponentTemplate.cpp
 *
 * \date 11/mag/2012 (13:09:41)
 * \author marco
 */

#include "ObjectModel/ComponentTemplate.h"

ComponentTemplate::ComponentTemplate()
{
}

void ComponentTemplate::setParameters(ParameterTable& parameterTable)
{
}

void ComponentTemplate::resetParameters()
{
}

std::string& ComponentTemplate::parameter(const std::string& name)
{
	std::string* strPtr = &mUnknown;
	//
	return *strPtr;
}

std::list<std::string>& ComponentTemplate::parameterList(const std::string& name)
{
	std::list<std::string>* strListPtr = &mUnknownList;
	//
	return *strListPtr;
}

//TypedObject semantics: hardcoded
TypeHandle ComponentTemplate::_type_handle;


