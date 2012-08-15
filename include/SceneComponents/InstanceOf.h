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
 * \file /Ely/include/SceneComponents/InstanceOf.h
 *
 * \date 20/mag/2012 (09:42:38)
 * \author marco
 */

#ifndef INSTANCEOF_H_
#define INSTANCEOF_H_

#include <nodePath.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"
#include "SceneComponents/InstanceOfTemplate.h"
#include "ObjectModel/ObjectTemplateManager.h"

class InstanceOfTemplate;

/**
 * \brief Component representing an instance of another object.
 *
 * XML Param(s):
 * \li \c "instance_of"  	|single|no default
 * \li \c "scale_x"  		|single|"1.0"
 * \li \c "scale_y"  		|single|"1.0"
 * \li \c "scale_z"  		|single|"1.0"
 */
class InstanceOf: public Component
{
public:
	InstanceOf();
	InstanceOf(InstanceOfTemplate* tmpl);
	virtual ~InstanceOf();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

private:
	///The template used to construct this component.
	InstanceOfTemplate* mTmpl;
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
		register_type(_type_handle, "InstanceOf", Component::get_class_type());
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

#endif /* INSTANCEOF_H_ */
