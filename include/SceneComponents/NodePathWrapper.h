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
 * \file /Ely/include/SceneComponents/NodePathWrapper.h
 *
 * \date 28/giu/2012 (20:16:04)
 * \author marco
 */

#ifndef NODEPATHWRAPPER_H_
#define NODEPATHWRAPPER_H_

#include <string>
#include <nodePath.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "SceneComponents/NodePathWrapperTemplate.h"

class NodePathWrapperTemplate;

/**
 * \brief Component wrapping a predefined NodePath (e.g. render, camera etc...).
 */
class NodePathWrapper: public Component
{
public:
	NodePathWrapper();
	NodePathWrapper(NodePathWrapperTemplate* tmpl);
	virtual ~NodePathWrapper();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

	/**
	 * \brief Gets a reference to the node path associated to this model.
	 * @return The node path associated to this model.
	 */
	NodePath& nodePath();
	/**
	 * \brief NodePath conversion function.
	 */
	operator NodePath();

private:
	///The template used to construct this component.
	NodePathWrapperTemplate* mTmpl;
	///The NodePath associated to this model.
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
		register_type(_type_handle, "NodePathWrapper", Component::get_class_type());
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

#endif /* NODEPATHWRAPPER_H_ */