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
 * \file /Ely/include/SceneComponents/ModelTemplate.h
 *
 * \date 16/mag/2012 (16:26:41)
 * \author marco
 */

#ifndef MODELTEMPLATE_H_
#define MODELTEMPLATE_H_

#include <string>
#include <list>
#include <utility>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <typedObject.h>

#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Component.h"
#include "SceneComponents/Model.h"
#include "Utilities/Tools.h"

class ModelTemplate: public ComponentTemplate
{
public:
	ModelTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ModelTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(const ComponentId& compId);

	virtual void setParameters(ParameterTable& parameterTable);
	virtual void resetParameters();
	virtual std::string& parameter(const std::string& paramName);
	virtual std::list<std::string>& parameterList(const std::string& paramName);

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
	///The name of the model file containing the static model.
	std::string mModelFile;
	///The names of the files containing the animations.
	std::list<std::string> mAnimFiles;
	///Static flag: if this object doesn't move in the world.
	std::string mIsStatic;
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
		register_type(_type_handle, "ModelTemplate",
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

#endif /* MODELTEMPLATE_H_ */