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
 * \file /Ely/include/PhysicsComponents/CharacterController.h
 *
 * \date 30/ott/2012 17:03:49
 * \author marco
 */

#ifndef CHARACTERCONTROLLER_H_
#define CHARACTERCONTROLLER_H_

#include <string>
#include <cstdlib>
#include <cmath>
#include <pointerTo.h>
#include <nodePath.h>
#include <lvecBase3.h>
#include <lvector3.h>
#include <lpoint3.h>
#include <bulletShape.h>
#include <bulletSphereShape.h>
#include <bulletBoxShape.h>
#include <bulletCylinderShape.h>
#include <bulletCapsuleShape.h>
#include <bulletConeShape.h>
#include <bulletCharacterControllerNode.h>
#include <bullet_utils.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Utilities/Tools.h"

class CharacterControllerTemplate;

/**
 * \brief Component representing a character controller attached to an object.
 *
 * It constructs a character controller with the single specified collision
 * shape_type along with relevant parameters.\n
 *
 * XML Param(s):
 * - "collide_mask"  			|single|"all_on"
 * - "shape_type"  				|single|"sphere"
 * - "use_shape_of"				|single|no default
 * - "shape_radius"  			|single|no default (sphere,cylinder,capsule,cone)
 * - "shape_height"  			|single|no default (cylinder,capsule,cone)
 * - "shape_up"  				|single|no default (cylinder,capsule,cone)
 * - "shape_half_x"  			|single|no default (box)
 * - "shape_half_y"  			|single|no default (box)
 * - "shape_half_z"  			|single|no default (box)
 */
class CharacterController: public Component
{
public:
	CharacterController();
	CharacterController(SMARTPTR(CharacterControllerTemplate) tmpl);
	virtual ~CharacterController();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \brief The shape type.
	 */
	enum ShapeType
	{
		SPHERE, //!< SPHERE (radius)
		BOX, //!< BOX (half_x, half_y, half_z)
		CYLINDER, //!< CYLINDER (radius, height, up)
		CAPSULE, //!< CAPSULE (radius, height, up)
		CONE, //!< CONE (radius, height, up)
	};

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
	SMARTPTR(BulletCharacterControllerNode) mCharacterNode;

	///Geometric functions and parameters.
	BitMask32 mCollideMask;
	///@{
	/**
	 * \brief Create a shape given its type.
	 * @param shapeType The shape type.
	 * @return The created shape.
	 */
	SMARTPTR(BulletShape) createShape(ShapeType shapeType);
	ShapeType mShapeType;
	/**
	 * \brief Calculates geometric characteristics of a GeomNode.
	 *
	 * It takes a NodePath, (supposedly) referring to a GeomNode, and
	 * calculates a tight bounding box surrounding it, hence sets the
	 * related dimensions into mModelDims, mModelCenter, mModelRadius
	 * member variables.
	 * @param modelNP The GeomNode node path.
	 */
	void getBoundingDimensions(NodePath modelNP);
	LVector3 mModelDims;
	float mModelRadius;
	//any model has a local frame and the tight bounding box is computed
	//wrt it; so mModelDeltaCenter represents a transform (translation) to
	//be applied to the model node path so that the middle point of the
	//bounding box will overlap the frame center of the parent's node path .
	LVector3 mModelDeltaCenter;
	bool mAutomaticShaping;
	float mDim1, mDim2, mDim3, mDim4;
	BulletUpAxis mUpAxis;
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "CharacterController", Component::get_class_type());
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

#endif /* CHARACTERCONTROLLER_H_ */
