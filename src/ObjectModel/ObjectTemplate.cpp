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
		ObjectTemplateManager* objectTmplMgr) :
		mName(name), mObjectTmplMgr(objectTmplMgr)
{
	if (not objectTmplMgr)
	{
		throw GameException(
				"ObjectTemplate::ObjectTemplate: invalid ObjectTemplateManager");

	}
	//reset parameters
	setParametersDefaults();
}

ObjectTemplate::~ObjectTemplate()
{
	// TODO Auto-generated destructor stub
}

const ObjectType& ObjectTemplate::name() const
{
	return mName;
}

void ObjectTemplate::clearComponentTemplates()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mComponentTemplates.clear();
}

ObjectTemplate::ComponentTemplateList& ObjectTemplate::getComponentTemplates()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mComponentTemplates;
}

void ObjectTemplate::addComponentTemplate(ComponentTemplate* componentTmpl)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not componentTmpl)
	{
		throw GameException(
				"ObjectTemplate::addComponentTemplate: NULL component template");
	}
	mComponentTemplates.push_back(componentTmpl);
}

ComponentTemplate* ObjectTemplate::getComponentTemplate(
		const ComponentType& componentType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	ComponentTemplateList::iterator it;
	it = find_if(mComponentTemplates.begin(), mComponentTemplates.end(),
			idIsEqualTo(componentType));
	if (it == mComponentTemplates.end())
	{
		return NULL;
	}
	return *it;
}

ObjectTemplateManager* const ObjectTemplate::objectTmplMgr() const
{
	return mObjectTmplMgr;
}

void ObjectTemplate::setParameters(ParameterTable& parameterTable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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

void ObjectTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("is_static", "false"));
	mParameterTable.insert(ParameterNameValue("pos_x", "0.0"));
	mParameterTable.insert(ParameterNameValue("pos_y", "0.0"));
	mParameterTable.insert(ParameterNameValue("pos_z", "0.0"));
	mParameterTable.insert(ParameterNameValue("rot_h", "0.0"));
	mParameterTable.insert(ParameterNameValue("rot_p", "0.0"));
	mParameterTable.insert(ParameterNameValue("rot_r", "0.0"));
}

std::string ObjectTemplate::parameter(const std::string& paramName)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	std::string strPtr;
	ParameterTable::iterator iter;
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
	HOLDMUTEX(mMutex)

	std::list<std::string> strList;
	ParameterTableIter iter;
	pair<ParameterTableIter, ParameterTableIter> iterRange;
	iterRange = mParameterTable.equal_range(paramName);
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

ReMutex& ComponentTemplate::getMutex()
{
	return mMutex;
}

//TypedObject semantics: hardcoded
TypeHandle ObjectTemplate::_type_handle;
