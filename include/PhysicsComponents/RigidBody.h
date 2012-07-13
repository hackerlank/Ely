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

#include <pointerTo.h>
#include <nodePath.h>
#include <bulletShape.h>
#include <bulletRigidBodyNode.h>
#include <bitMask.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

class RigidBodyTemplate;

/**
 * \brief Component representing a single rigid body attached to an object.
 *
 * It, by default, constructs a rigid body with a single collision shape
 * of type:
 * \li \c Sphere (default)
 * \li \c Box
 * \li \c Cylinder
 * \li \c Capsule
 * \li \c Cone
 * as specified by a component template parameter, and wrapping as tight as
 * possible, the geometry of the object (specified by the model component).
 * More complex shapes, can be set through the bullet rigid body node.
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
		STATIC,  //!< STATIC: mass == 0.0, no driven
		KINEMATIC//!< KINEMATIC: mass == 0.0, user driven
	};

	/**
	 * \brief Switches the actual component's type.
	 * @param bodyType The new component's type.
	 */
	void switchType(BodyType bodyType);

private:
	///The template used to construct this component.
	RigidBodyTemplate* mTmpl;
	///The root of the scene (e.g. render)
	NodePath mSceneRoot;
	///The Rigid Body Node of this component.
	PT(BulletRigidBodyNode) mRigidBodyNode;
	///@{
	///Physics parameters.
	float mBodyMass, mCcdMotionThreshold, mCcdSweptSphereRadius;
	BodyType mBodyType;
	BitMask32 mCollideMask;
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
