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
 * \file /Ely/include/GraphicsComponents/ModelTemplate.h
 *
 * \date 16/mag/2012 (16:26:41)
 * \author marco
 */

#ifndef MODELTEMPLATE_H_
#define MODELTEMPLATE_H_

#include <string>
#include <list>
#include <filename.h>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <lvecBase3.h>
#include <typedObject.h>

#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Component.h"
#include "GraphicsComponents/Model.h"
#include "Utilities/Tools.h"

class ModelTemplate: public ComponentTemplate
{
public:
	ModelTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~ModelTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(ComponentId& compId);

	virtual void reset();

	/**
	 * \brief Gets/sets the model file name.
	 * @return A reference to the model file name.
	 */
	Filename& modelFile();

	/**
	 * \brief Gets/sets the list of the animations file names.
	 * @return A reference to the list of the animations file names.
	 */
	std::list<Filename>& animFiles();

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
	///The name of the model file containing the static model.
	Filename mModelFile;
	///The name of the file containing the animations.
	std::list<Filename> mAnimFiles;

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
