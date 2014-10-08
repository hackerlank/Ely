// Filename: bulletSoftBodyNode.h
// Created by:  enn0x (27Dec10)
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

#ifndef __BULLET_SOFT_BODY_NODE_H__
#define __BULLET_SOFT_BODY_NODE_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bullet_utils.h"
#include "bulletBodyNode.h"
#include "bulletRigidBodyNode.h"

#include <collideMask.h>
#include <geom.h>
#include <geomNode.h>
#include <geomVertexFormat.h>
#include <boundingBox.h>
#include <nurbsCurveEvaluator.h>
#include <nurbsSurfaceEvaluator.h>
#include <pta_LVecBase3.h>

namespace ely
{
class BulletSoftBodyConfig;
class BulletSoftBodyControl;
class BulletSoftBodyMaterial;
class BulletSoftBodyWorldInfo;

////////////////////////////////////////////////////////////////////
//       Class : BulletSoftBodyNodeElement
// Description : 
////////////////////////////////////////////////////////////////////
class BulletSoftBodyNodeElement
{

public:
	inline ~BulletSoftBodyNodeElement();
	inline static BulletSoftBodyNodeElement empty();

	inline LPoint3 get_pos() const;
	inline LVector3 get_velocity() const;
	inline LVector3 get_normal() const;
	inline PN_stdfloat get_inv_mass() const;
	inline PN_stdfloat get_area() const;
	inline int is_attached() const;

public:
	BulletSoftBodyNodeElement(btSoftBody::Node &node);

private:
	btSoftBody::Node &_node;
};

////////////////////////////////////////////////////////////////////
//       Class : BulletSoftBodyNode
// Description : 
////////////////////////////////////////////////////////////////////
class BulletSoftBodyNode: public BulletBodyNode
{

public:
	BulletSoftBodyNode(btSoftBody *body, const char *name = "softbody");

public:
	inline ~BulletSoftBodyNode();

	BulletSoftBodyConfig get_cfg();
	BulletSoftBodyWorldInfo get_world_info();

	void generate_bending_constraints(int distance,
			BulletSoftBodyMaterial *material = NULL);
	void randomize_constraints();

	// Mass, volume, density
	void set_volume_mass(PN_stdfloat mass);
	void set_volume_density(PN_stdfloat density);
	void set_total_mass(PN_stdfloat mass, bool fromfaces = false);
	void set_total_density(PN_stdfloat density);
	void set_mass(int node, PN_stdfloat mass);

	PN_stdfloat get_mass(int node) const;
	PN_stdfloat get_total_mass() const;
	PN_stdfloat get_volume() const;

	// Force
	void add_force(const LVector3 &force);
	void add_force(const LVector3 &force, int node);

	void set_velocity(const LVector3 &velocity);
	void add_velocity(const LVector3 &velocity);
	void add_velocity(const LVector3 &velocity, int node);

	void set_wind_velocity(const LVector3 &velocity);
	LVector3 get_wind_velocity() const;

	void set_pose(bool bvolume, bool bframe);

	BoundingBox get_aabb() const;

	// Cluster
	void generate_clusters(int k, int maxiterations = 8192);
	void release_cluster(int index);
	void release_clusters();
	int get_num_clusters() const;
	LVecBase3 cluster_com(int cluster) const;

	// Rendering
	void link_geom(Geom *geom);
	void unlink_geom();

	void link_curve(NurbsCurveEvaluator *curve);
	void unlink_curve();

	void link_surface(NurbsSurfaceEvaluator *surface);
	void unlink_surface();

	// Anchors
	void append_anchor(int node, BulletRigidBodyNode *body,
			bool disable = false);
	void append_anchor(int node, BulletRigidBodyNode *body,
			const LVector3 &pivot, bool disable = false);

	// Links
	void append_linear_joint(BulletBodyNode *body, int cluster,
			PN_stdfloat erp = 1.0, PN_stdfloat cfm = 1.0, PN_stdfloat split =
					1.0);

	void append_linear_joint(BulletBodyNode *body, const LPoint3 &pos,
			PN_stdfloat erp = 1.0, PN_stdfloat cfm = 1.0, PN_stdfloat split =
					1.0);

	void append_angular_joint(BulletBodyNode *body, const LVector3 &axis,
			PN_stdfloat erp = 1.0, PN_stdfloat cfm = 1.0, PN_stdfloat split =
					1.0, BulletSoftBodyControl *control = NULL);

	// Materials
	int get_num_materials() const;
	BulletSoftBodyMaterial get_material(int idx) const;
	;

	BulletSoftBodyMaterial append_material();

	// Nodes
	int get_num_nodes() const;
	BulletSoftBodyNodeElement get_node(int idx) const;
	;

	int get_closest_node_index(LVecBase3 point, bool local);

	// Factory
	static PT(BulletSoftBodyNode)make_rope(
			BulletSoftBodyWorldInfo &info,
			const LPoint3 &from,
			const LPoint3 &to,
			int res,
			int fixeds);

	static PT(BulletSoftBodyNode) make_patch(
			BulletSoftBodyWorldInfo &info,
			const LPoint3 &corner00,
			const LPoint3 &corner10,
			const LPoint3 &corner01,
			const LPoint3 &corner11,
			int resx,
			int resy,
			int fixeds,
			bool gendiags);

	static PT(BulletSoftBodyNode) make_ellipsoid(
			BulletSoftBodyWorldInfo &info,
			const LPoint3 &center,
			const LVecBase3 &radius,
			int res);

	static PT(BulletSoftBodyNode) make_tri_mesh(
			BulletSoftBodyWorldInfo &info,
			const Geom *geom,
			bool randomizeConstraints=true);

	static PT(BulletSoftBodyNode) make_tri_mesh(
			BulletSoftBodyWorldInfo &info,
			PTA_LVecBase3 points,
			PTA_int indices,
			bool randomizeConstraints=true);

	static PT(BulletSoftBodyNode) make_tet_mesh(
			BulletSoftBodyWorldInfo &info,
			PTA_LVecBase3 points,
			PTA_int indices,
			bool tetralinks=true);

	static PT(BulletSoftBodyNode) make_tet_mesh(
			BulletSoftBodyWorldInfo &info,
			const char *ele,
			const char *face,
			const char *node);

public:
	virtual btCollisionObject *get_object() const;

	void sync_p2b();
	void sync_b2p();

protected:
	virtual void transform_changed();

private:
	btSoftBody *_soft;

	CPT(TransformState) _sync;
	bool _sync_disable;

	PT(Geom) _geom;
	PT(NurbsCurveEvaluator) _curve;
	PT(NurbsSurfaceEvaluator) _surface;

	static int get_point_index(LVecBase3 p, PTA_LVecBase3 points);
	static int next_line(const char *buffer);

////////////////////////////////////////////////////////////////////
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		BulletBodyNode::init_type();
		register_type(_type_handle, "BulletSoftBodyNode",
				BulletBodyNode::get_class_type());
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
//     Function: BulletSoftBodyNode::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletSoftBodyNode::~BulletSoftBodyNode()
{

	delete _soft;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletSoftBodyNodeElement::~BulletSoftBodyNodeElement()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::empty
//       Access: Published
//  Description: Named constructor intended to be used for asserts
//               with have to return a concrete value.
////////////////////////////////////////////////////////////////////
inline BulletSoftBodyNodeElement BulletSoftBodyNodeElement::empty()
{

	static btSoftBody::Node node;

	return BulletSoftBodyNodeElement(node);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::get_pos
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LPoint3 BulletSoftBodyNodeElement::get_pos() const
{

	return btVector3_to_LPoint3(_node.m_x);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::get_normal
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletSoftBodyNodeElement::get_normal() const
{

	return btVector3_to_LVector3(_node.m_n);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::get_velocity
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline LVector3 BulletSoftBodyNodeElement::get_velocity() const
{

	return btVector3_to_LVector3(_node.m_v);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::get_inv_mass
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyNodeElement::get_inv_mass() const
{

	return (PN_stdfloat) _node.m_im;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::get_area
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyNodeElement::get_area() const
{

	return (PN_stdfloat) _node.m_area;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyNodeElement::is_attached
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletSoftBodyNodeElement::is_attached() const
{

	return (PN_stdfloat) _node.m_battach;
}
}
#endif // __BULLET_SOFT_BODY_NODE_H__

