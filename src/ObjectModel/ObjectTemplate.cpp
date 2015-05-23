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
 * \author consultit
 */

#include "ObjectModel/ObjectTemplate.h"

namespace ely
{

ObjectTemplate::ObjectTemplate(const ObjectType& name,
		ObjectTemplateManager* objectTmplMgr, PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		mName(name), mObjectTmplMgr(objectTmplMgr), mPandaFramework(
				pandaFramework), mWindowFramework(windowFramework)
{
	if (not objectTmplMgr)
	{
		throw GameException(
				"ObjectTemplate::ObjectTemplate: invalid ObjectTemplateManager");

	}
	if (not pandaFramework or not windowFramework)
	{
		throw GameException(
				"ObjectTemplate::ObjectTemplate: invalid PandaFramework or WindowFramework");
	}
	//reset parameters
	setParametersDefaults();
}

ObjectTemplate::~ObjectTemplate()
{
}

void ObjectTemplate::clearComponentTemplates()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mComponentTemplates.clear();
}

void ObjectTemplate::addComponentTemplate(SMARTPTR(ComponentTemplate)componentTmpl)
{
	//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		if (not componentTmpl)
		{
			throw GameException(
					"ObjectTemplate::addComponentTemplate: NULL component template");
		}
		mComponentTemplates.push_back(componentTmpl);
	}

SMARTPTR(ComponentTemplate)ObjectTemplate::getComponentTemplate(
		const ComponentType& componentType) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	ComponentTemplateList::const_iterator it;
	it = find_if(mComponentTemplates.begin(), mComponentTemplates.end(),
			idIsEqualTo(componentType));
	RETURN_ON_COND(it == mComponentTemplates.end(), NULL)

	return *it;
}

void ObjectTemplate::setParameters(const ParameterTable& parameterTable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	RETURN_ON_COND(parameterTable.empty(),)

	ParameterTableConstIter iter;
	std::pair<ParameterTableIter, ParameterTableIter> iterRange;
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

void ObjectTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("is_steady", "false"));
	mParameterTable.insert(ParameterNameValue("store_params", "false"));
	mParameterTable.insert(ParameterNameValue("pos", "0.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("rot", "0.0,0.0,0.0"));
	mParameterTable.insert(ParameterNameValue("init_func", ""));
}

std::string ObjectTemplate::parameter(const std::string& paramName) const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::string strPtr;
	ParameterTableConstIter iter;
	iter = mParameterTable.find(paramName);
	//return a reference to a parameter value only if it exists
	if (iter != mParameterTable.end())
	{
		strPtr = iter->second;
	}
	//
	return strPtr;
}

std::list<std::string> ObjectTemplate::parameterList(
		const std::string& paramName)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<std::string> strList;
	ParameterTableIter iter;
	std::pair<ParameterTableIter, ParameterTableIter> iterRange =
			mParameterTable.equal_range(paramName);
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

void ObjectTemplate::addComponentParameter(const std::string& parameterName,
		const std::string& parameterValue, ComponentType compType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//any parameter value is a "compound" one, i.e. could have the form:
	// "value1:value2:...:valueN"
	std::vector<std::string> values = parseCompoundString(parameterValue, ':');
	std::vector<std::string>::const_iterator iterValue;
	for (iterValue = values.begin(); iterValue != values.end(); ++iterValue)
	{
		mComponentParameterTables[compType].insert(
				ParameterTable::value_type(parameterName, *iterValue));
	}
}

bool ObjectTemplate::isComponentParameterValue(const std::string& name,
		const std::string& value, ComponentType compType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	bool result;
	//
	std::pair<ParameterTableConstIter, ParameterTableConstIter> iterRange =
			mComponentParameterTables[compType].equal_range(name);
	if (iterRange.first != iterRange.second)
	{
		result = (iterRange.second
				!= find(iterRange.first, iterRange.second,
						ParameterTable::value_type(name, value)));
	}
	else
	{
		result = false;
	}
	//
	return result;
}

std::list<std::string> ObjectTemplate::componentParameterValues(
		const std::string& paramName, ComponentType compType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<std::string> strList;
	ParameterTableConstIter iter;
	std::pair<ParameterTableConstIter, ParameterTableConstIter> iterRange;
	iterRange = mComponentParameterTables[compType].equal_range(paramName);
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
TypeHandle ObjectTemplate::_type_handle;

} //namespace ely
