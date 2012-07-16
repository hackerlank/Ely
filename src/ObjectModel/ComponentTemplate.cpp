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
	mParameterTable.clear();
}

void ComponentTemplate::setParameters(ParameterTable& parameterTable)
{
	ParameterTableIter iter;
	pair<ParameterTableIter, ParameterTableIter> iterRange;
	//create the parameterTable key set (i.e. the set of parameters
	//that will overwrite those of mParameterTable with the same name)
	std::set<std::string> keySet;
	for (iter = parameterTable.begin(); iter != parameterTable.end(); ++iter)
	{
		keySet.insert(iter->first);
	}
	//erase from mParameterTable the parameters to be overwritten
	std::set<std::string>::iterator keySetIter;
	for (keySetIter = keySet.begin(); keySetIter != keySet.end(); ++keySetIter)
	{
		//find the mParameterTable range of values for
		//the *keySetIter parameter ...
		iterRange = mParameterTable.equal_range(*keySetIter);
		//...and erase it
		mParameterTable.erase(iterRange.first, iterRange.second);
	}
	//now mParameterTable is free from parameters to be overwritten
	//so insert these ones into it from parameterTable
	mParameterTable.insert(parameterTable.begin(), parameterTable.end());
}

std::string ComponentTemplate::parameter(const std::string& name)
{
	std::string strPtr;
	ParameterTable::iterator iter;
	iter = mParameterTable.find(name);
	//return a reference to a parameter value only if it exists
	if (iter != mParameterTable.end())
	{
		strPtr = iter->second;
	}
	//
	return strPtr;
}

std::list<std::string> ComponentTemplate::parameterList(const std::string& name)
{
	std::list<std::string> strList;
	ParameterTableIter iter;
	pair<ParameterTableIter, ParameterTableIter> iterRange;
	iterRange = mParameterTable.equal_range(name);
	if (iterRange.first != iterRange.second)
	{
		for (iter = iterRange.first; iter != iterRange.second; ++iter)
		{
			strList.push_back(iter->second);
		}
	}
	//
	return strList;
}

//TypedObject semantics: hardcoded
TypeHandle ComponentTemplate::_type_handle;

