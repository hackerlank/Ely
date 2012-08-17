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
 * \file /Ely/include/BehavioralComponents/ActivityTemplate.h
 *
 * \date 17/ago/2012 (09:27:08)
 * \author marco
 */

#ifndef ACTIVITYTEMPLATE_H_
#define ACTIVITYTEMPLATE_H_

#include <string>
#include <utility>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <typedObject.h>
#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Component.h"
#include "BehavioralComponents/Activity.h"
#include "Utilities/Tools.h"

class ActivityTemplate: public ComponentTemplate
{
public:
	ActivityTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ActivityTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(const ComponentId& compId);

	virtual void setParametersDefaults();

	/**
	 * \brief Gets/sets the PandaFramework.
	 * @return A reference to the PandaFramework.
	 */
	PandaFramework* const pandaFramework() const;

	/**
	 * \brief Gets/sets the WindowFramework.
	 * @return A reference to the WindowFramework.
	 */
	WindowFramework* const windowFramework() const;

private:
	///The PandaFramework.
	PandaFramework* const mPandaFramework;
	///The WindowFramework.
	WindowFramework* const mWindowFramework;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "ActivityTemplate",
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

#endif /* ACTIVITYTEMPLATE_H_ */
