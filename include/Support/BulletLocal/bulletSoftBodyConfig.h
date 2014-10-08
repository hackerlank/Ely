// Filename: bulletSoftBodyConfig.h
// Created by:  enn0x (12Apr10)
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

#ifndef __BULLET_SOFT_BODY_CONFIG_H__
#define __BULLET_SOFT_BODY_CONFIG_H__

#include <pandabase.h>

#include "Utilities/Tools.h"
#include "bullet_includes.h"

#include <numeric_types.h>
#include <pnotify.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletSoftBodyConfig
// Description : 
////////////////////////////////////////////////////////////////////
class BulletSoftBodyConfig
{

public:
	inline ~BulletSoftBodyConfig();

	enum CollisionFlag
	{
		CF_rigid_vs_soft_mask = 0x000f, // RVSmask: Rigid versus soft mask
		CF_sdf_rigid_soft = 0x0001, // SDF_RS:  SDF based rigid vs soft
		CF_cluster_rigid_soft = 0x0002, // CL_RS:   Cluster vs convex rigid vs soft
		CF_soft_vs_soft_mask = 0x0030, // SVSmask: Soft versus soft mask
		CF_vertex_face_soft_soft = 0x0010, // VF_SS:   Vertex vs face soft vs soft handling
		CF_cluster_soft_soft = 0x0020, // CL_SS:   Cluster vs cluster soft vs soft handling
		CF_cluster_self = 0x0040, // CL_SELF: Cluster soft body self collision
	};

	enum AeroModel
	{
		AM_vertex_point, // V_Point:    Vertex normals are oriented toward velocity
		AM_vertex_two_sided, // V_TwoSided: Vertex normals are fliped to match velocity
		AM_vertex_one_sided, // V_OneSided: Vertex normals are taken as it is
		AM_face_two_sided, // F_TwoSided: Face normals are fliped to match velocity
		AM_face_one_sided,   // F_OneSided: Face normals are taken as it is
	};

	void clear_all_collision_flags();
	void set_collision_flag(CollisionFlag flag, bool value);
	bool get_collision_flag(CollisionFlag flag) const;

	void set_aero_model(AeroModel value);
	AeroModel get_aero_model() const;

	inline void set_velocities_correction_factor(PN_stdfloat value);
	inline void set_damping_coefficient(PN_stdfloat value);
	inline void set_drag_coefficient(PN_stdfloat value);
	inline void set_lift_coefficient(PN_stdfloat value);
	inline void set_pressure_coefficient(PN_stdfloat value);
	inline void set_volume_conversation_coefficient(PN_stdfloat value);
	inline void set_dynamic_friction_coefficient(PN_stdfloat value);
	inline void set_pose_matching_coefficient(PN_stdfloat value);
	inline void set_rigid_contacts_hardness(PN_stdfloat value);
	inline void set_kinetic_contacts_hardness(PN_stdfloat value);
	inline void set_soft_contacts_hardness(PN_stdfloat value);
	inline void set_anchors_hardness(PN_stdfloat value);
	inline void set_soft_vs_rigid_hardness(PN_stdfloat value);
	inline void set_soft_vs_kinetic_hardness(PN_stdfloat value);
	inline void set_soft_vs_soft_hardness(PN_stdfloat value);
	inline void set_soft_vs_rigid_impulse_split(PN_stdfloat value);
	inline void set_soft_vs_kinetic_impulse_split(PN_stdfloat value);
	inline void set_soft_vs_soft_impulse_split(PN_stdfloat value);
	inline void set_maxvolume(PN_stdfloat value);
	inline void set_timescale(PN_stdfloat value);
	inline void set_positions_solver_iterations(int value);
	inline void set_velocities_solver_iterations(int value);
	inline void set_drift_solver_iterations(int value);
	inline void set_cluster_solver_iterations(int value);

	inline PN_stdfloat get_velocities_correction_factor() const;
	inline PN_stdfloat get_damping_coefficient() const;
	inline PN_stdfloat get_drag_coefficient() const;
	inline PN_stdfloat get_lift_coefficient() const;
	inline PN_stdfloat get_pressure_coefficient() const;
	inline PN_stdfloat get_volume_conversation_coefficient() const;
	inline PN_stdfloat get_dynamic_friction_coefficient() const;
	inline PN_stdfloat get_pose_matching_coefficient() const;
	inline PN_stdfloat get_rigid_contacts_hardness() const;
	inline PN_stdfloat get_kinetic_contacts_hardness() const;
	inline PN_stdfloat get_soft_contacts_hardness() const;
	inline PN_stdfloat get_anchors_hardness() const;
	inline PN_stdfloat get_soft_vs_rigid_hardness() const;
	inline PN_stdfloat get_soft_vs_kinetic_hardness() const;
	inline PN_stdfloat get_soft_vs_soft_hardness() const;
	inline PN_stdfloat get_soft_vs_rigid_impulse_split() const;
	inline PN_stdfloat get_soft_vs_kinetic_impulse_split() const;
	inline PN_stdfloat get_soft_vs_soft_impulse_split() const;
	inline PN_stdfloat get_maxvolume() const;
	inline PN_stdfloat get_timescale() const;
	inline int get_positions_solver_iterations() const;
	inline int get_velocities_solver_iterations() const;
	inline int get_drift_solver_iterations() const;
	inline int get_cluster_solver_iterations() const;

public:
	BulletSoftBodyConfig(btSoftBody::Config &cfg);

private:
	btSoftBody::Config &_cfg;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletSoftBodyConfig::~BulletSoftBodyConfig()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_velocities_correction_factor
//       Access: Published
//  Description: Getter for property kVCF.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_velocities_correction_factor() const
{

	return (PN_stdfloat) _cfg.kVCF;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_velocities_correction_factor
//       Access: Published
//  Description: Setter for property kVCF.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_velocities_correction_factor(
		PN_stdfloat value)
{

	_cfg.kVCF = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_damping_coefficient
//       Access: Published
//  Description: Getter for property kDP.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_damping_coefficient() const
{

	return (PN_stdfloat) _cfg.kDP;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_damping_coefficient
//       Access: Published
//  Description: Setter for property kDP.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_damping_coefficient(PN_stdfloat value)
{

	_cfg.kDP = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_drag_coefficient
//       Access: Published
//  Description: Getter for property kDG.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_drag_coefficient() const
{

	return (PN_stdfloat) _cfg.kDG;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_drag_coefficient
//       Access: Published
//  Description: Setter for property kDG.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_drag_coefficient(PN_stdfloat value)
{

	_cfg.kDG = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_lift_coefficient
//       Access: Published
//  Description: Getter for property kLF.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_lift_coefficient() const
{

	return (PN_stdfloat) _cfg.kLF;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_lift_coefficient
//       Access: Published
//  Description: Setter for property kLF.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_lift_coefficient(PN_stdfloat value)
{

	_cfg.kLF = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_pressure_coefficient
//       Access: Published
//  Description: Getter for property kPR.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_pressure_coefficient() const
{

	return (PN_stdfloat) _cfg.kPR;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_pressure_coefficient
//       Access: Published
//  Description: Setter for property kPR.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_pressure_coefficient(PN_stdfloat value)
{

	_cfg.kPR = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_volume_conversation_coefficient
//       Access: Published
//  Description: Getter for property kVC.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_volume_conversation_coefficient() const
{

	return (PN_stdfloat) _cfg.kVC;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_volume_conversation_coefficient
//       Access: Published
//  Description: Setter for property kVC.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_volume_conversation_coefficient(
		PN_stdfloat value)
{

	_cfg.kVC = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_dynamic_friction_coefficient
//       Access: Published
//  Description: Getter for property kDF.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_dynamic_friction_coefficient() const
{

	return (PN_stdfloat) _cfg.kDF;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_dynamic_friction_coefficient
//       Access: Published
//  Description: Setter for property kDF.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_dynamic_friction_coefficient(
		PN_stdfloat value)
{

	_cfg.kDF = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_pose_matching_coefficient
//       Access: Published
//  Description: Getter for property kMT.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_pose_matching_coefficient() const
{

	return (PN_stdfloat) _cfg.kMT;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_pose_matching_coefficient
//       Access: Published
//  Description: Setter for property kMT.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_pose_matching_coefficient(
		PN_stdfloat value)
{

	_cfg.kMT = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_rigid_contacts_hardness
//       Access: Published
//  Description: Getter for property kCHR.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_rigid_contacts_hardness() const
{

	return (PN_stdfloat) _cfg.kCHR;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_rigid_contacts_hardness
//       Access: Published
//  Description: Setter for property kCHR.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_rigid_contacts_hardness(PN_stdfloat value)
{

	_cfg.kCHR = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_kinetic_contacts_hardness
//       Access: Published
//  Description: Getter for property kKHR.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_kinetic_contacts_hardness() const
{

	return (PN_stdfloat) _cfg.kKHR;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_kinetic_contacts_hardness
//       Access: Published
//  Description: Setter for property kKHR.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_kinetic_contacts_hardness(
		PN_stdfloat value)
{

	_cfg.kKHR = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_soft_contacts_hardness
//       Access: Published
//  Description: Getter for property kSHR.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_soft_contacts_hardness() const
{

	return (PN_stdfloat) _cfg.kSHR;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_soft_contacts_hardness
//       Access: Published
//  Description: Setter for property kSHR.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_soft_contacts_hardness(PN_stdfloat value)
{

	_cfg.kSHR = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_anchors_hardness
//       Access: Published
//  Description: Getter for property kAHR.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_anchors_hardness() const
{

	return (PN_stdfloat) _cfg.kAHR;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_anchors_hardness
//       Access: Published
//  Description: Setter for property kAHR.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_anchors_hardness(PN_stdfloat value)
{

	_cfg.kAHR = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_soft_vs_rigid_hardness
//       Access: Published
//  Description: Getter for property kSRHR_CL.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_soft_vs_rigid_hardness() const
{

	return (PN_stdfloat) _cfg.kSRHR_CL;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_soft_vs_rigid_hardness
//       Access: Published
//  Description: Setter for property kSRHR_CL.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_soft_vs_rigid_hardness(PN_stdfloat value)
{

	_cfg.kSRHR_CL = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_soft_vs_kinetic_hardness
//       Access: Published
//  Description: Getter for property kSKHR_CL.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_soft_vs_kinetic_hardness() const
{

	return (PN_stdfloat) _cfg.kSKHR_CL;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_soft_vs_kinetic_hardness
//       Access: Published
//  Description: Setter for property kSKHR_CL.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_soft_vs_kinetic_hardness(
		PN_stdfloat value)
{

	_cfg.kSKHR_CL = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_soft_vs_soft_hardness
//       Access: Published
//  Description: Getter for property kSSHR_CL.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_soft_vs_soft_hardness() const
{

	return (PN_stdfloat) _cfg.kSSHR_CL;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_soft_vs_soft_hardness
//       Access: Published
//  Description: Setter for property kSSHR_CL.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_soft_vs_soft_hardness(PN_stdfloat value)
{

	_cfg.kSSHR_CL = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_soft_vs_rigid_impulse_split
//       Access: Published
//  Description: Getter for property kSR_SPLT_CL.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_soft_vs_rigid_impulse_split() const
{

	return (PN_stdfloat) _cfg.kSR_SPLT_CL;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_soft_vs_rigid_impulse_split
//       Access: Published
//  Description: Setter for property kSR_SPLT_CL.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_soft_vs_rigid_impulse_split(
		PN_stdfloat value)
{

	_cfg.kSR_SPLT_CL = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_soft_vs_kinetic_impulse_split
//       Access: Published
//  Description: Getter for property kSK_SPLT_CL.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_soft_vs_kinetic_impulse_split() const
{

	return (PN_stdfloat) _cfg.kSK_SPLT_CL;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_soft_vs_kinetic_impulse_split
//       Access: Published
//  Description: Setter for property kSK_SPLT_CL.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_soft_vs_kinetic_impulse_split(
		PN_stdfloat value)
{

	_cfg.kSK_SPLT_CL = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_soft_vs_soft_impulse_split
//       Access: Published
//  Description: Getter for property kSS_SPLT_CL.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_soft_vs_soft_impulse_split() const
{

	return (PN_stdfloat) _cfg.kSS_SPLT_CL;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_soft_vs_soft_impulse_split
//       Access: Published
//  Description: Setter for property kSS_SPLT_CL.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_soft_vs_soft_impulse_split(
		PN_stdfloat value)
{

	_cfg.kSS_SPLT_CL = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_maxvolume
//       Access: Published
//  Description: Getter for property maxvolume.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_maxvolume() const
{

	return (PN_stdfloat) _cfg.maxvolume;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_maxvolume
//       Access: Published
//  Description: Setter for property maxvolume.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_maxvolume(PN_stdfloat value)
{

	_cfg.maxvolume = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_timescale
//       Access: Published
//  Description: Getter for property timescale.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyConfig::get_timescale() const
{

	return (PN_stdfloat) _cfg.timescale;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_timescale
//       Access: Published
//  Description: Setter for property timescale.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_timescale(PN_stdfloat value)
{

	_cfg.timescale = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_positions_solver_iterations
//       Access: Published
//  Description: Getter for property piterations.
////////////////////////////////////////////////////////////////////
inline int BulletSoftBodyConfig::get_positions_solver_iterations() const
{

	return _cfg.piterations;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_positions_solver_iterations
//       Access: Published
//  Description: Setter for property piterations.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_positions_solver_iterations(int value)
{

	RETURN_ON_COND(not(value > 0),)
	_cfg.piterations = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_velocities_solver_iterations
//       Access: Published
//  Description: Getter for property viterations.
////////////////////////////////////////////////////////////////////
inline int BulletSoftBodyConfig::get_velocities_solver_iterations() const
{

	return _cfg.viterations;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_velocities_solver_iterations
//       Access: Published
//  Description: Setter for property viterations.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_velocities_solver_iterations(int value)
{

	RETURN_ON_COND(not (value > 0),)
	_cfg.viterations = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_drift_solver_iterations
//       Access: Published
//  Description: Getter for property diterations.
////////////////////////////////////////////////////////////////////
inline int BulletSoftBodyConfig::get_drift_solver_iterations() const
{

	return _cfg.diterations;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_drift_solver_iterations
//       Access: Published
//  Description: Setter for property diterations.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_drift_solver_iterations(int value)
{

	RETURN_ON_COND(not (value > 0),)
	_cfg.diterations = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::get_cluster_solver_iterations
//       Access: Published
//  Description: Getter for property citerations.
////////////////////////////////////////////////////////////////////
inline int BulletSoftBodyConfig::get_cluster_solver_iterations() const
{

	return _cfg.citerations;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyConfig::set_cluster_solver_iterations
//       Access: Published
//  Description: Setter for property citerations.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyConfig::set_cluster_solver_iterations(int value)
{

	RETURN_ON_COND(not (value > 0),)
	_cfg.citerations = value;
}
}
#endif // __BULLET_SOFT_BODY_CONFIG_H__
