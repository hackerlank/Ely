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
 * \file /Ely/include/InputComponents/ControlByEventTemplate.h
 *
 * \date 31/mag/2012 (16:44:06)
 * \author marco
 */

#ifndef CONTROLBYEVENTTEMPLATE_H_
#define CONTROLBYEVENTTEMPLATE_H_

#include <string>
#include <typedObject.h>
#include <pandaFramework.h>
#include <windowFramework.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"
#include "InputComponents/ControlByEvent.h"
#include "Game/GameInputManager.h"
#include "Utilities/Tools.h"

class ControlByEventTemplate: public ComponentTemplate
{
public:
	ControlByEventTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ControlByEventTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(const ComponentId& compId);

	virtual void setParametersDefaults();

	/**
	 * \brief Gets the singleton GameInputManager.
	 * @return A reference to the GameInputManager.
	 */
	GameInputManager* gameInputMgr();

	/**
	 * \brief Gets/sets the PandaFramework.
	 * @return A reference to the PandaFramework.
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

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "ControlByEventTemplate",
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

#endif /* CONTROLBYEVENTTEMPLATE_H_ */
