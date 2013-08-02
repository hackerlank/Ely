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
 * \author consultit
 */

#ifndef NODEPATHWRAPPER_H_
#define NODEPATHWRAPPER_H_

#include <nodePath.h>
#include "ObjectModel/Component.h"

namespace ely
{
class NodePathWrapperTemplate;

/**
 * \brief Component wrapping a predefined NodePath (e.g. render, camera etc...).
 *
 * On error default NodePath is render.
 *
 * XML Param(s):
 * - "nodepath"  		|single|no default
 */
class NodePathWrapper: public Component
{
protected:
	friend class Object;
	friend class NodePathWrapperTemplate;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

public:
	NodePathWrapper();
	NodePathWrapper(SMARTPTR(NodePathWrapperTemplate)tmpl);
	virtual ~NodePathWrapper();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	/**
	 * \brief Gets/sets the node path of this node path wrapper.
	 * @return The node path of this this node path wrapper.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

private:
	///The NodePath associated to this node path wrapper.
	NodePath mNodePath;

	///The wrapped NodePath.
	std::string mWrappedNodePath;

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
}  // namespace ely

#endif /* NODEPATHWRAPPER_H_ */
