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
 * \file /Ely/include/AIComponents/Steering.h
 *
 * \date 03/dic/2012 (13:39:59)
 * \author marco
 */

#ifndef STEERING_H_
#define STEERING_H_

#include <aiCharacter.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

class SteeringTemplate;

/**
 * \brief Component implementing AI Steering Behaviors and Path Finding.
 *
 * XML Param(s):
 * - "param1"  			|single|no default
 * - "param2"  			|multiple|no default
 */
class Steering: public Component
{
public:
	Steering();
	Steering(SMARTPTR(SteeringTemplate) tmpl);
	virtual ~Steering();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

	/**
	 * \brief Gets/sets the node path associated to this model.
	 * @return The node path associated to this model.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

private:
	///The NodePath associated to this Steering.
	NodePath mNodePath;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Steering", Component::get_class_type());
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

#endif /* STEERING_H_ */
