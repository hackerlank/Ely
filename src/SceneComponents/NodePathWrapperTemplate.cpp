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
 * \file /Ely/src/SceneComponents/NodePathWrapperTemplate.cpp
 *
 * \date 28/giu/2012 (20:15:15)
 * \author consultit
 */

#include "SceneComponents/NodePathWrapperTemplate.h"
#include "SceneComponents/NodePathWrapper.h"
#include "Game/GameSceneManager.h"

namespace ely
{

NodePathWrapperTemplate::NodePathWrapperTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE(pandaFramework,
			"NodePathWrapperTemplate::NodePathWrapperTemplate: invalid PandaFramework")
	CHECK_EXISTENCE(windowFramework,
			"NodePathWrapperTemplate::NodePathWrapperTemplate: invalid WindowFramework")
	CHECK_EXISTENCE(GameSceneManager::GetSingletonPtr(),
			"NodePathWrapperTemplate::NodePathWrapperTemplate: invalid GameSceneManager")
	//
	setParametersDefaults();

}

NodePathWrapperTemplate::~NodePathWrapperTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType NodePathWrapperTemplate::componentType() const
{
	return ComponentType("NodePathWrapper");
}

ComponentFamilyType NodePathWrapperTemplate::familyType() const
{
	return ComponentFamilyType("Scene");
}

SMARTPTR(Component)NodePathWrapperTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(NodePathWrapper) newNodePathWrapper = new NodePathWrapper(this);
	newNodePathWrapper->setComponentId(compId);
	if (not newNodePathWrapper->initialize())
	{
		return NULL;
	}
	return newNodePathWrapper.p();
}

void NodePathWrapperTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values:
	//no mandatory parameters
}

//TypedObject semantics: hardcoded
TypeHandle NodePathWrapperTemplate::_type_handle;

} // namespace ely
