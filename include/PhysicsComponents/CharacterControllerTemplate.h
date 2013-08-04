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
 * \file /Ely/include/PhysicsComponents/CharacterControllerTemplate.h
 *
 * \date 30/ott/2012 (17:01:33)
 * \author consultit
 */

#ifndef CHARACTERCONTROLLERTEMPLATE_H_
#define CHARACTERCONTROLLERTEMPLATE_H_

#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"

namespace ely
{
class CharacterControllerTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	CharacterControllerTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~CharacterControllerTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType familyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "CharacterControllerTemplate",
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
}  // namespace ely

#endif /* CHARACTERCONTROLLERTEMPLATE_H_ */
