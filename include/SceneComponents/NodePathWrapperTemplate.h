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
 * \file /Ely/include/SceneComponents/NodePathWrapperTemplate.h
 *
 * \date 28/giu/2012 (20:15:15)
 * \author marco
 */

#ifndef NODEPATHWRAPPERTEMPLATE_H_
#define NODEPATHWRAPPERTEMPLATE_H_

#include <string>
#include <pandaFramework.h>
#include <windowFramework.h>

#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Component.h"
#include "SceneComponents/NodePathWrapper.h"
#include "Utilities/Tools.h"

class NodePathWrapperTemplate: public ComponentTemplate
{
public:
	NodePathWrapperTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~NodePathWrapperTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(const ComponentId& compId);

	virtual void setParameters(ParameterTable& parameterTable);
	virtual void resetParameters();
	virtual std::string& parameter(const std::string& paramName);

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
	///@{
	/// Set of allowed Parameters.
	///The wrapped NodePath.
	std::string mNodePath;
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
		register_type(_type_handle, "NodePathWrapperTemplate",
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

#endif /* NODEPATHWRAPPERTEMPLATE_H_ */