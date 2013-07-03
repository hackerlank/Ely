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
 * \file /Ely/src/SceneComponents/ModelTemplate.cpp
 *
 * \date 16/mag/2012 (16:26:41)
 * \author consultit
 */

#include "SceneComponents/ModelTemplate.h"
#include "SceneComponents/Model.h"
#include "Game/GameSceneManager.h"

namespace ely
{

ModelTemplate::ModelTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECKEXISTENCE(pandaFramework,
			"ModelTemplate::ModelTemplate: invalid PandaFramework")
	CHECKEXISTENCE(windowFramework,
			"ModelTemplate::ModelTemplate: invalid WindowFramework")
	CHECKEXISTENCE(GameSceneManager::GetSingletonPtr(),
			"ModelTemplate::ModelTemplate: invalid GameSceneManager")
	//
	setParametersDefaults();
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
	return ComponentFamilyType("Scene");
}

SMARTPTR(Component)ModelTemplate::makeComponent(const ComponentId& compId)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(Model) newModel = new Model(this);
	newModel->setComponentId(compId);
	if (not newModel->initialize())
	{
		return NULL;
	}
	return newModel.p();
}

void ModelTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("from_file", "true"));
	mParameterTable.insert(ParameterNameValue("scale_x", "1.0"));
	mParameterTable.insert(ParameterNameValue("scale_y", "1.0"));
	mParameterTable.insert(ParameterNameValue("scale_z", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle ModelTemplate::_type_handle;

} // namespace ely
