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
	mParameterTable.insert(ParameterNameValue("body_type", "dynamic"));
	mParameterTable.insert(ParameterNameValue("body_mass", "1.0"));
	mParameterTable.insert(ParameterNameValue("body_friction", "0.8"));
	mParameterTable.insert(ParameterNameValue("body_restitution", "0.1"));
	mParameterTable.insert(ParameterNameValue("shape_type", "sphere"));
	mParameterTable.insert(ParameterNameValue("shape_size", "medium"));
	mParameterTable.insert(ParameterNameValue("collide_mask", "all_on"));
	mParameterTable.insert(ParameterNameValue("shape_height", "1.0"));
	mParameterTable.insert(ParameterNameValue("shape_up", "z"));
	mParameterTable.insert(ParameterNameValue("shape_scale_w", "1.0"));
	mParameterTable.insert(ParameterNameValue("shape_scale_d", "1.0"));
}

//TypedObject semantics: hardcoded
TypeHandle SoftBodyTemplate::_type_handle;

} /* namespace ely */
