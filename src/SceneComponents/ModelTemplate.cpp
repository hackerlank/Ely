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
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"ModelTemplate::ModelTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"ModelTemplate::ModelTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GameSceneManager::GetSingletonPtr(),
			"ModelTemplate::ModelTemplate: invalid GameSceneManager")
	//
	setParametersDefaults();
}

ModelTemplate::~ModelTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType ModelTemplate::componentType() const
{
	return ComponentType("Model");
}

ComponentFamilyType ModelTemplate::familyType() const
{
	return ComponentFamilyType("Scene");
}

SMARTPTR(Component)ModelTemplate::makeComponent(const ComponentId& compId)
{
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
	HOLD_MUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	mParameterTable.insert(ParameterNameValue("from_file", "true"));
	mParameterTable.insert(ParameterNameValue("scale", "1.0,1.0,1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle ModelTemplate::_type_handle;

} // namespace ely
