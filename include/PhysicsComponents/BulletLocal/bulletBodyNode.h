// Filename: bulletBodyNode.h
// Created by:  enn0x (19Nov10)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#ifndef __BULLET_BODY_NODE_H__
#define __BULLET_BODY_NODE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"

#include <pandaNode.h>
#include <collideMask.h>
#include <collisionNode.h>
#include <transformState.h>

namespace ely
{
class BulletShape;

////////////////////////////////////////////////////////////////////
//       Class : BulletBodyNode
// Description : 
////////////////////////////////////////////////////////////////////
class BulletBodyNode: public PandaNode
{

public:
	BulletBodyNode(const char *name);
	inline ~BulletBodyNode();

	// Shapes
	void add_shape(BulletShape *shape, const TransformState *xform =
			TransformState::make_identity());
	void remove_shape(BulletShape *shape);

	inline int get_num_shapes() const;
	inline BulletShape *get_shape(int idx) const;MAKE_SEQ(get_shapes, get_num_shapes, get_shape)
	;

	LPoint3 get_shape_pos(int idx) const;
	LMatrix4 get_shape_mat(int idx) const;

	void add_shapes_from_collision_solids(CollisionNode *cnode);

	// Static and kinematic
	inline bool is_static() const;
	inline bool is_kinematic() const;

	inline void set_static(bool value);
	inline void set_kinematic(bool value);

	// Contacts
	inline void set_into_collide_mask(CollideMask mask);

	inline void notify_collisions(bool value);
	inline bool notifies_collisions() const;

	inline void set_collision_response(bool value);
	inline bool get_collision_response() const;

	bool check_collision_with(PandaNode *node);

	bool has_contact_response() const;

	PN_stdfloat get_contact_processing_threshold() const;
	void set_contact_processing_threshold(PN_stdfloat threshold);

	// Deactivation
	bool is_active() const;
	void set_active(bool active, bool force = false);

	void set_deactivation_time(PN_stdfloat dt);
	PN_stdfloat get_deactivation_time() const;

	void set_deactivation_enabled(const bool enabled, const bool force = false);
	bool is_deactivation_enabled() const;

	// Debug Visualistion
	inline void set_debug_enabled(const bool enabled);
	inline bool is_debug_enabled() const;

	// Friction and Restitution
	inline PN_stdfloat get_restitution() const;
	inline void set_restitution(PN_stdfloat restitution);

	inline PN_stdfloat get_friction() const;
	inline void set_friction(PN_stdfloat friction);

#if BT_BULLET_VERSION >= 281
	inline PN_stdfloat get_rolling_friction() const;
	inline void set_rolling_friction(PN_stdfloat friction);
#endif

	inline bool has_anisotropic_friction() const;
	void set_anisotropic_friction(const LVecBase3 &friction);
	LVecBase3 get_anisotropic_friction() const;

	// CCD
	PN_stdfloat get_ccd_swept_sphere_radius() const;
	PN_stdfloat get_ccd_motion_threshold() const;
	void set_ccd_swept_sphere_radius(PN_stdfloat radius);
	void set_ccd_motion_threshold(PN_stdfloat threshold);

	// Special
	void set_transform_dirty();

public:
	virtual btCollisionObject *get_object() const = 0;

	virtual CollideMask get_legal_collide_mask() const;

	virtual bool safe_to_flatten() const;
	virtual bool safe_to_transform() const;
	virtual bool safe_to_modify_transform() const;
	virtual bool safe_to_combine() const;
	virtual bool safe_to_combine_children() const;
	virtual bool safe_to_flatten_below() const;

	virtual void output(ostream &out) const;

protected:
	inline void set_collision_flag(int flag, bool value);
	inline bool get_collision_flag(int flag) const;

	btCollisionShape *_shape;

	typedef PTA(PT(BulletShape))BulletShapes;
	BulletShapes _shapes;

private:
	virtual void shape_changed();

	static bool is_identity(btTransform &trans);

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		PandaNode::init_type();
		register_type(_type_handle, "BulletBodyNode",
				PandaNode::get_class_type());
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

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletBodyNode::~BulletBodyNode()
{

	if (_shape
			&& (_shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE
					|| _shape->getShapeType() == EMPTY_SHAPE_PROXYTYPE))
	{
		delete _shape;
	}
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_into_collide_mask
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_into_collide_mask(CollideMask mask)
{

	PandaNode::set_into_collide_mask(mask);

	/*
	 TODO: we would need a handle to the BulletWorld first
	 possible, but has to be set/cleared upon attach/remove to world

	 if (!_world) return;

	 btBroadphaseProxy* proxy = get_object()->getBroadphaseHandle();
	 if (proxy) {
	 btBroadphaseInterface *broadphase = _world->get_broadphase();
	 btDispatcher *dispatcher = _world->get_dispatcher();

	 broadphase->getOverlappingPairCache()->cleanProxyFromPairs(proxy, dispatcher);
	 }
	 */
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::notify_collisions
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::notify_collisions(bool value)
{

	set_collision_flag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK, value);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::notifies_collisions
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletBodyNode::notifies_collisions() const
{

	return get_collision_flag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_collision_response
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_collision_response(bool value)
{

	set_collision_flag(btCollisionObject::CF_NO_CONTACT_RESPONSE, !value);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::get_collision_response
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletBodyNode::get_collision_response() const
{

	return !get_collision_flag(btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_collision_flag
//       Access: Protected
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_collision_flag(int flag, bool value)
{

	int flags = get_object()->getCollisionFlags();

	if (value == true)
	{
		flags |= flag;
	}
	else
	{
		flags &= ~(flag);
	}

	get_object()->setCollisionFlags(flags);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::get_collision_flag
//       Access: Protected
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletBodyNode::get_collision_flag(int flag) const
{

	return (get_object()->getCollisionFlags() & flag) ? true : false;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::is_static
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletBodyNode::is_static() const
{

	return get_object()->isStaticObject();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::is_kinematic
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletBodyNode::is_kinematic() const
{

	return get_object()->isKinematicObject();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_static
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_static(bool value)
{

	set_collision_flag(btCollisionObject::CF_STATIC_OBJECT, value);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_kinematic
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_kinematic(bool value)
{

	set_collision_flag(btCollisionObject::CF_KINEMATIC_OBJECT, value);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::get_restitution
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletBodyNode::get_restitution() const
{

	return get_object()->getRestitution();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_restitution
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_restitution(PN_stdfloat restitution)
{

	return get_object()->setRestitution(restitution);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::get_friction
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletBodyNode::get_friction() const
{

	return get_object()->getFriction();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_friction
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_friction(PN_stdfloat friction)
{

	return get_object()->setFriction(friction);
}

#if BT_BULLET_VERSION >= 281
////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::get_rolling_friction
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletBodyNode::get_rolling_friction() const
{

	return get_object()->getRollingFriction();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_rolling_friction
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_rolling_friction(PN_stdfloat friction)
{

	return get_object()->setRollingFriction(friction);
}
#endif

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::has_anisotropic_friction
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline bool BulletBodyNode::has_anisotropic_friction() const
{

	return get_object()->hasAnisotropicFriction();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::set_debug_enabled
//       Access: Published
//  Description: Enables or disables the debug visualisation for
//               this collision object. By default the debug
//               visualisation is enabled.
////////////////////////////////////////////////////////////////////
inline void BulletBodyNode::set_debug_enabled(const bool enabled)
{

	set_collision_flag(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT,
			!enabled);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::is_debug_enabled
//       Access: Published
//  Description: Returns TRUE if the debug visualisation is enabled
//               for this collision object, and FALSE if the debug
//               visualisation is disabled.
////////////////////////////////////////////////////////////////////
inline bool BulletBodyNode::is_debug_enabled() const
{

	return !get_collision_flag(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::get_shape
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletShape *BulletBodyNode::get_shape(int idx) const
{

	nassertr(idx >= 0 && idx < (int ) _shapes.size(), NULL);
	return _shapes[idx];
}

////////////////////////////////////////////////////////////////////
//     Function: BulletBodyNode::get_num_shapes
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletBodyNode::get_num_shapes() const
{

	return _shapes.size();
}

}
#endif // __BULLET_BODY_NODE_H__

