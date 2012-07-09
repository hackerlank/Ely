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
 * \file /Ely/include/PhysicsComponents/RigidBodyTemplate.h
 *
 * \date 07/lug/2012 (15:56:19)
 * \author marco
 */

#ifndef RIGIDBODYTEMPLATE_H_
#define RIGIDBODYTEMPLATE_H_

#include <pandaFramework.h>
#include <windowFramework.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"
#include "PhysicsComponents/RigidBody.h"
#include "Game/GamePhysicsManager.h"
#include "Utilities/Tools.h"

class RigidBodyTemplate: public ComponentTemplate
{
public:
	RigidBodyTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~RigidBodyTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(const ComponentId& compId);

	virtual void setParameters(ParameterTable& parameterTable);
	virtual void resetParameters();
	virtual std::string& parameter(const std::string& paramName);

	/**
	 * \brief Gets the singleton GamePhysicsManager.
	 * @return A reference to the GamePhysicsManager.
	 */
	GamePhysicsManager* gamePhysicsMgr();

	/**
	 * \brief Gets/sets the PandaFramework.
	 * @return A reference to the GameManager.
	 */
	PandaFramework*& pandaFramework();

	/**
	 * \brief Gets/sets the WindowFramework.
	 * @return A reference to the WindowFramework.
	 */
	WindowFramework*& windowFramework();

private:
	///The PandaFramework.
	PandaFramework* mPandaFramework;
	///The WindowFramework.
	WindowFramework* mWindowFramework;

	///@{
	/// Physics parameters.
	std::string mBodyType, mShapeType;
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "RigidBodyTemplate",
				ComponentTemplate::get_class_type());
	}

	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}

	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;
};

#endif /* RIGIDBODYTEMPLATE_H_ */
