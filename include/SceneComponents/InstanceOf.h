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
 * \author consultit
 */

#ifndef INSTANCEOF_H_
#define INSTANCEOF_H_

#include "Utilities/Tools.h"

#include <nodePath.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "SceneComponents/Model.h"

namespace ely
{
class InstanceOfTemplate;

/**
 * \brief Component representing an instance of another object.
 *
 * XML Param(s):
 * - "instance_of"  	|single|no default
 * - "scale_x"  		|single|"1.0"
 * - "scale_y"  		|single|"1.0"
 * - "scale_z"  		|single|"1.0"
 */
class InstanceOf: public Component
{
public:
	InstanceOf();
	InstanceOf(SMARTPTR(InstanceOfTemplate)tmpl);
	virtual ~InstanceOf();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

	/**
	 * \brief Gets/sets the node path associated to this instance of.
	 * @return The node path associated to this instance of.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \brief Gets a reference to the instanced object.
	 * @return The reference to the instanced object.
	 */
	///@{
	SMARTPTR(Object) getInstancedObject() const;
	///@}

private:
	///The NodePath associated to this instance of.
	NodePath mNodePath;
	///The instanced object.
	SMARTPTR(Object) mInstancedObject;
	/**
	 * \name Main parameters.
	 */
	///@{
	///Instance of object id.
	ObjectId mInstanceOfId;
	///@}

	///Scaling  (default: (1.0,1.0,1.0)).
	float mScaleX, mScaleY, mScaleZ;

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
}  // namespace ely

#endif /* INSTANCEOF_H_ */
