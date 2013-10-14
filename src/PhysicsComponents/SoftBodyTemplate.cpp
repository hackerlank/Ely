/*
 * SoftBodyTemplate.cpp
 *
 *  Created on: 08/ott/2013
 *      Author: marco
 */

#include "PhysicsComponents/SoftBodyTemplate.h"
#include "PhysicsComponents/SoftBody.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

SoftBodyTemplate::SoftBodyTemplate(PandaFramework* pandaFramework,
		WindowFramework* windowFramework) :
		ComponentTemplate(pandaFramework, windowFramework)
{
	CHECK_EXISTENCE_DEBUG(pandaFramework,
			"SoftBodyTemplate::SoftBodyTemplate: invalid PandaFramework")
	CHECK_EXISTENCE_DEBUG(windowFramework,
			"SoftBodyTemplate::SoftBodyTemplate: invalid WindowFramework")
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"SoftBodyTemplate::SoftBodyTemplate: invalid GamePhysicsManager")
	//
	setParametersDefaults();
}

SoftBodyTemplate::~SoftBodyTemplate()
{
	// TODO Auto-generated destructor stub
}

ComponentType SoftBodyTemplate::componentType() const
{
	return ComponentType("SoftBody");
}

ComponentFamilyType SoftBodyTemplate::familyType() const
{
	return ComponentFamilyType("Physics");
}

SMARTPTR(Component)SoftBodyTemplate::makeComponent(const ComponentId& compId)
{
	SMARTPTR(SoftBody) newSoftBody = new SoftBody(this);
	newSoftBody->setComponentId(compId);
	if (not newSoftBody->initialize())
	{
		return NULL;
	}
	return newSoftBody.p();
}

void SoftBodyTemplate::setParametersDefaults()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//mParameterTable must be the first cleared
	mParameterTable.clear();
	//sets the (mandatory) parameters to their default values.
	mParameterTable.insert(ParameterNameValue("", ""));
	mParameterTable.insert(ParameterNameValue("", ""));
	mParameterTable.insert(ParameterNameValue("", ""));
}

//TypedObject semantics: hardcoded
TypeHandle SoftBodyTemplate::_type_handle;

} /* namespace ely */