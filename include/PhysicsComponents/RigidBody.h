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
#include <bulletSphereShape.h>
#include <bulletBoxShape.h>
#include <bulletPlaneShape.h>
#include <bulletCylinderShape.h>
#include <bulletCapsuleShape.h>
#include <bulletConeShape.h>
#include <bulletRigidBodyNode.h>
#include <bullet_utils.h>
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
 * Collision shapes are:
 * \li \c "sphere"
 * \li \c "plane"
 * \li \c "box"
 * \li \c "cylinder"
 * \li \c "capsule"
 * \li \c "cone"
 *
 * In case of "sphere", "box", "cylinder", "capsule", "cone", if any of
 * the relevant parameters is missing, the shape is automatically
 * constructed by guessing them through calculation of a tight bounding volume
 * of object geometry (supposedly specified by the model component).
 * For "plane" shape, in case of missing parameters, the default is
 * a plane with normal = (0,0,1) and d = 0.
 *
 * XML Param(s):
 * \li \c "body_type"  |single|"dynamic" ("static","kinematic")
 * \li \c "body_mass"  |single|"1.0"
 * \li \c "body_friction"  |single|"0.8"
 * \li \c "body_restitution"  |single|"0.1"
 * \li \c "shape_type"  |single|"sphere"
 * \li \c "collide_mask"  |single|"all_on"
 * \li \c "shape_radius"  |single|no default (sphere,cylinder,capsule,cone)
 * \li \c "shape_norm_x"  |single|no default (plane)
 * \li \c "shape_norm_y"  |single|no default (plane)
 * \li \c "shape_norm_z"  |single|no default (plane)
 * \li \c "shape_d"  |single|no default (plane)
 * \li \c "shape_half_x"  |single|no default (box)
 * \li \c "shape_half_y"  |single|no default (box)
 * \li \c "shape_half_z"  |single|no default (box)
 * \li \c "shape_height"  |single|no default (cylinder,capsule,cone)
 * \li \c "shape_up"  |single|no default (cylinder,capsule,cone)
 * \li \c "ccd_motion_threshold"  |single|no default
 * \li \c "ccd_swept_sphere_radius"  |single|no default
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
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

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

	/**
	 * \brief Gets/sets the node path of this rigid body.
	 * @return The node path of this rigid body.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

private:
	///The template used to construct this component.
	RigidBodyTemplate* mTmpl;
	///The NodePath associated to this rigid body.
	NodePath mNodePath;
	///The NodePath associated to this rigid body.
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
	 * \brief Sets physical parameters of a bullet rigid body node (helper function).
	 * @param rigidBodyNode The bullet rigid body node.
	 */
	void setPhysicalParameters();
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
