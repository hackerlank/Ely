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
 * \file /Ely/include/AIComponents/SteeringTemplate.h
 *
 * \date 03/dic/2012 (13:52:08)
 * \author consultit
 */

#ifndef STEERINGTEMPLATE_H_
#define STEERINGTEMPLATE_H_

#include "ObjectModel/ComponentTemplate.h"

namespace ely
{
class SteeringTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);
	virtual void setParametersDefaults();

public:
	SteeringTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~SteeringTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

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
		register_type(_type_handle, "SteeringTemplate",
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
} // namespace ely

#endif /* STEERINGTEMPLATE_H_ */
