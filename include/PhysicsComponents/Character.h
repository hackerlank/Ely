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
 * \file /Ely/include/PhysicsComponents/Character.h
 *
 * \date 30/ott/2012 17:03:49
 * \author marco
 */

#ifndef CHARACTER_H_
#define CHARACTER_H_

#include <string>
#include <pointerTo.h>
#include <nodePath.h>
#include <bulletCharacterControllerNode.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Utilities/Tools.h"

class CharacterTemplate;

/**
 * \brief Component representing a character controller attached to an object.
 *
 * It constructs a character controller with the single specified collision
 * shape_type along with relevant parameters.\n
 *
 * XML Param(s):
 * - "XXXX"  				|single|no default
 */
class Character: public Component
{
public:
	Character();
	Character(SMARTPTR(CharacterTemplate) tmpl);
	virtual ~Character();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \brief Gets/sets the node path of this character controller.
	 * @return The node path of this character controller.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

private:
	///The NodePath associated to this character controller.
	NodePath mNodePath;
	///The NodePath associated to this character controller.
	SMARTPTR(BulletCharacterControllerNode) mCharacter;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Character", Component::get_class_type());
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

#endif /* CHARACTER_H_ */
