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
 * \file /Ely/include/GraphicsComponents/Model.h
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <list>
#include <nodePath.h>
#include <animControlCollection.h>
#include <auto_bind.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"

class ModelTemplate;

/**
 * \brief Component representing the model and animations of an object.
 */
class Model: public Component
{
public:
	Model();
	Model(ModelTemplate* tmpl);
	virtual ~Model();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddSetup();

	/**
	 * \brief Get a reference to the node path associated to this model.
	 * @return The node path associated to this model.
	 */
	NodePath& nodePath();
	/**
	 * \brief NodePath conversion function.
	 */
	operator NodePath();

	AnimControlCollection& animations();

private:
	///The template used to construct this component.
	ModelTemplate* mTmpl;
	///The NodePath associated to this model.
	NodePath mNodePath;
	///The list of animations associated with this model.
	AnimControlCollection mAnimations;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Model", Component::get_class_type());
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

#endif /* MODEL_H_ */
