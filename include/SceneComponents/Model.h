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
 * \file /Ely/include/SceneComponents/Model.h
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <list>
#include <cstdlib>
#include <nodePath.h>
#include <filename.h>
#include <animControlCollection.h>
#include <auto_bind.h>
#include <cardMaker.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

class ModelTemplate;

/**
 * \brief Component representing the model and animations of an object.
 *
 * XML Param(s):
 * \li \c "from_file"  			|single|"true"
 * \li \c "scale_x"  			|single|"1.0"
 * \li \c "scale_y"  			|single|"1.0"
 * \li \c "scale_z"  			|single|"1.0"
 * \li \c "model_file"  		|single|no default
 * \li \c "anim_files"  		|multiple|no default
 * \li \c "model_type"  		|single|no default
 * \li \c "model_card_left"  	|single|no default
 * \li \c "model_card_right"  	|single|no default
 * \li \c "model_card_bottom"  	|single|no default
 * \li \c "model_card_top"  	|single|no default
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
	virtual void onAddToObjectSetup();

	/**
	 * \brief Gets/sets the node path associated to this model.
	 * @return The node path associated to this model.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \brief Gets a reference to AnimControlCollection of this model.
	 *
	 * Thread safe.
	 * @return A reference to AnimControlCollection of this model.
	 */
	AnimControlCollection animations() const;

private:
	///The NodePath associated to this model.
	NodePath mNodePath;
	///The list of animations associated with this model.
	AnimControlCollection mAnimations;
	///Flag indicating if component is set up from a file or programmatically
	bool mFromFile;

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
