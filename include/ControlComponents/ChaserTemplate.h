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
 * \file /Ely/include/ControlComponents/ChaserTemplate.h
 *
 * \date 11/nov/2012 (09:43:16)
 * \author marco
 */

#ifndef CHASERTEMPLATE_H_
#define CHASERTEMPLATE_H_

#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"
#include "ControlComponents/Chaser.h"
#include "Game/GameControlManager.h"
#include "Utilities/Tools.h"

class ChaserTemplate: public ComponentTemplate
{
public:
	ChaserTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ChaserTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual SMARTPTR(Component) makeComponent(const ComponentId& compId);

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
		register_type(_type_handle, "ChaserTemplate",
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

#endif /* CHASERTEMPLATE_H_ */
