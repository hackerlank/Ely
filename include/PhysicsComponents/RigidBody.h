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
 * \file /Ely/include/PhysicsComponents/RigidBody.h
 *
 * \date 07/lug/2012 (15:58:35)
 * \author marco
 */

#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_

#include <string>
#include <cstdlib>
#include <stdint.h>
#include <cmath>
#include <algorithm>
#include <pointerTo.h>
#include <nodePath.h>
#include <bulletShape.h>
#include <bulletRigidBodyNode.h>
#include <bitMask.h>
#include <lvector3.h>
#include <lpoint3.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

class RigidBodyTemplate;

/**
 * \brief Component representing a single rigid body attached to an object.
 *
 * It constructs a rigid body with the specified collision shape_type along
 * with relevant parameters.
 * If any of the relevant parameter is missing, the shape is automatically
 * constructed striving to wrap it as tight as possible around the
 * object geometry (specified by the model component).
 *
 * XML Param(s):
 * \li \c "body_type"  |single|required|"dynamic" ("static","kinematic")
 * \li \c "body_mass"  |single|required|"1.0"
 * \li \c "body_friction"  |single|required|"0.8"
 * \li \c "body_restitution"  |single|required|"0.1"
 * \li \c "shape_type"  |single|required|"sphere"
 * \li \c "collide_mask"  |single|required|"all_on"
 * \li \c "shape_radius"  |single|optional |no default
 * \li \c "shape_norm_x"  |single|optional |no default
 * \li \c "shape_norm_y"  |single|optional |no default
 * \li \c "shape_norm_z"  |single|optional |no default
 * \li \c "shape_d"  |single|optional |no default
 * \li \c "shape_half_x"  |single|optional |no default
 * \li \c "shape_half_y"  |single|optional |no default
 * \li \c "shape_half_z"  |single|optional |no default
 * \li \c "shape_height"  |single|optional |no default
 * \li \c "shape_up"  |single|optional |no default
 * \li \c "ccd_motion_threshold"  |single|optional |no default
 * \li \c "ccd_swept_sphere_radius"  |single|optional |no default
 */
class RigidBody: public Component
{
public:
	RigidBody();
	RigidBody(RigidBodyTemplate* tmpl);
	virtual ~RigidBody();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToSceneSetup();

	/**
	 * \brief Gets a reference to the bullet rigid body node.
	 * @return The bullet rigid body node.
	 */
	BulletRigidBodyNode* rigidBodyNode();

	/**
	 * \brief The actual component's type.
	 *
	 * It may change during the component's lifetime.
	 */
	enum BodyType
	{
		DYNAMIC, //!< DYNAMIC: mass != 0.0, physics driven (default)
		STATIC, //!< STATIC: mass == 0.0, no driven
		KINEMATIC //!< KINEMATIC: mass == 0.0, user driven
	};

	/**
	 * \brief The shape type.
	 */
	enum ShapeType
	{
		SPHERE, //!< SPHERE (radius)
		PLANE, //!< PLANE (norm_x, norm_y, norm_z, d)
		BOX, //!< BOX (half_x, half_y, half_z)
		CYLINDER, //!< CYLINDER (radius, height, up)
		CAPSULE, //!< CAPSULE (radius, height, up)
		CONE //!< CONE (radius, height, up)
	};

	/**
	 * \brief Switches the actual component's type.
	 * @param bodyType The new component's type.
	 */
	void switchType(BodyType bodyType);

private:
	///The template used to construct this component.
	RigidBodyTemplate* mTmpl;
	///The Rigid Body Node of this component.
	PT(BulletRigidBodyNode) mRigidBodyNode;
	///@{
	///Physical parameters.
	float mBodyMass, mBodyFriction, mBodyRestitution;
	BodyType mBodyType;
	ShapeType mShapeType;
	BitMask32 mCollideMask;
	//ccd stuff
	float mCcdMotionThreshold, mCcdSweptSphereRadius;
	bool mCcdEnabled;
	/**
	 * \brief Sets physical parameters (helper function).
	 */
	void setPhysicalParameters();
	/**
	 * \brief Returns a correction vector position for the model to fit
	 * the collision shape position.
	 * @return The correction vector.
	 */
	LVector3 correctedPosition();
	///@}

	///Geometric functions and parameters.
	///@{
	/**
	 * \brief Create a shape given its type.
	 * @param shapeType The shape type.
	 * @return The created shape.
	 */
	BulletShape* createShape(ShapeType shapeType);
	/**
	 * \brief Calculates geometric characteristics of a GeomNode.
	 *
	 * It takes a NodePath, (supposedly) referring to a GeomNode, and
	 * calculates a tight bounding box surrounding it, hence sets the
	 * related dimensions into mModelDims, mModelCenter, mModelRadius
	 * member variables.
	 * @param modelNP The GeomNode node path.
	 */
	void getDimensions(NodePath modelNP);
	LVector3 mModelDims;
	LPoint3 mModelCenter;
	float mModelRadius;
	bool mAutomaticShaping;
	float mDim1, mDim2, mDim3, mDim4;
	std::string mUpAxis;
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
		register_type(_type_handle, "RigidBody", Component::get_class_type());
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

#endif /* RIGIDBODY_H_ */
