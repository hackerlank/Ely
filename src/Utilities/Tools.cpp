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
 * \file /Ely/src/Utilities/Tools.cpp
 *
 * \date 19/mag/2012 (12:39:59)
 * \author marco
 */

#include "Utilities/Tools.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplate.h"
#include "Utilities/ComponentSuite.h"
///Distributed
#include "Support/Distributed/DistributedObjectBase.h"

std::vector<std::string> parseCompoundString(const std::string& compoundString,
		char separator)
{
	std::vector<std::string> substrings;
	size_t startPos, endPos, i;
	for (i = 0, startPos = 0, endPos = compoundString.find_first_of(separator);
			(endPos != std::string::npos)
					or (not compoundString.substr(startPos).empty()); endPos =
					compoundString.find_first_of(separator, startPos))
	{
		std::string substring;
		if (endPos != std::string::npos)
		{
			substring = compoundString.substr(startPos, endPos - startPos);
			startPos = endPos + 1;
		}
		else
		{
			substring = compoundString.substr(startPos,
					compoundString.size() - startPos);
			startPos = compoundString.size();
		}
		substrings.push_back(substring);
	}
	return substrings;
}

std::string replaceCharacter(const std::string& source, int character,
		int replacement)
{
	int len = source.size() + 1;
	char* dest = new char[len];
	strncpy(dest, source.c_str(), len);
	//replace hyphens
	char* pch;
	pch = strchr(dest, character);
	while (pch != NULL)
	{
		*pch = replacement;
		pch = strchr(pch + 1, character);
	}
	std::string outStr(dest);
	delete[] dest;
	return outStr;
}

void initTypedObjects()
{
	Component::init_type();
	ComponentTemplate::init_type();
	Object::init_type();
	ObjectTemplate::init_type();
	//
	Steering::init_type();
	SteeringTemplate::init_type();
	Listener::init_type();
	ListenerTemplate::init_type();
	Sound3d::init_type();
	Sound3dTemplate::init_type();
	Activity::init_type();
	ActivityTemplate::init_type();
	Chaser::init_type();
	ChaserTemplate::init_type();
	Driver::init_type();
	DriverTemplate::init_type();
	CharacterController::init_type();
	CharacterControllerTemplate::init_type();
	RigidBody::init_type();
	RigidBodyTemplate::init_type();
	InstanceOf::init_type();
	InstanceOfTemplate::init_type();
	Model::init_type();
	ModelTemplate::init_type();
	NodePathWrapper::init_type();
	NodePathWrapperTemplate::init_type();
	Terrain::init_type();
	TerrainTemplate::init_type();
	GeoMipTerrainRef::init_type();
	//
	///Distributed
	DistributedObjectBase::init_type();
}
