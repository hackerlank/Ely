// Filename: config_bullet.cxx
// Created by:  enn0x (23Jan10)
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

#include "Utilities/Tools.h"

#include "Support/BulletLocal/config_bullet.h"

#include "Support/BulletLocal/bulletBaseCharacterControllerNode.h"
#include "Support/BulletLocal/bulletBodyNode.h"
#include "Support/BulletLocal/bulletBoxShape.h"
#include "Support/BulletLocal/bulletCapsuleShape.h"
#include "Support/BulletLocal/bulletCharacterControllerNode.h"
#include "Support/BulletLocal/bulletConeShape.h"
#include "Support/BulletLocal/bulletConeTwistConstraint.h"
#include "Support/BulletLocal/bulletContactCallbackData.h"
#include "Support/BulletLocal/bulletConstraint.h"
#include "Support/BulletLocal/bulletConvexHullShape.h"
#include "Support/BulletLocal/bulletConvexPointCloudShape.h"
#include "Support/BulletLocal/bulletCylinderShape.h"
#include "Support/BulletLocal/bulletMinkowskiSumShape.h"
#include "Support/BulletLocal/bulletDebugNode.h"
#include "Support/BulletLocal/bulletFilterCallbackData.h"
#include "Support/BulletLocal/bulletGenericConstraint.h"
#include "Support/BulletLocal/bulletGhostNode.h"
#include "Support/BulletLocal/bulletHeightfieldShape.h"
#include "Support/BulletLocal/bulletHingeConstraint.h"
#include "Support/BulletLocal/bulletMultiSphereShape.h"
#include "Support/BulletLocal/bulletPlaneShape.h"
#include "Support/BulletLocal/bulletRigidBodyNode.h"
#include "Support/BulletLocal/bulletShape.h"
#include "Support/BulletLocal/bulletSliderConstraint.h"
#include "Support/BulletLocal/bulletSphereShape.h"
#include "Support/BulletLocal/bulletSphericalConstraint.h"
#include "Support/BulletLocal/bulletSoftBodyNode.h"
#include "Support/BulletLocal/bulletSoftBodyShape.h"
#include "Support/BulletLocal/bulletTickCallbackData.h"
#include "Support/BulletLocal/bulletTriangleMesh.h"
#include "Support/BulletLocal/bulletTriangleMeshShape.h"
#include "Support/BulletLocal/bulletVehicle.h"
#include "Support/BulletLocal/bulletWorld.h"

#include "Support/BulletLocal/bulletContactCallbacks.h"

#include <dconfig.h>
#include <pandaSystem.h>

namespace ely
{
extern ContactAddedCallback gContactAddedCallback;
extern ContactProcessedCallback gContactProcessedCallback;
extern ContactDestroyedCallback gContactDestroyedCallback;

Configure(config_bullet);

ConfigureFn(config_bullet)
{
	init_libbullet();
}

ConfigVariableInt bullet_max_objects("bullet-max-objects", 1024,
		PRC_DESC("Specifies the maximum number of individual objects within a "
				"bullet physics world. Default value is 1024."));

ConfigVariableInt bullet_gc_lifetime("bullet-gc-lifetime", 256,
		PRC_DESC(
				"Specifies the lifetime of data clean up be the soft body world "
						"info garbage collector. Default value is 256."));

ConfigVariableEnum<BulletWorld::BroadphaseAlgorithm> bullet_broadphase_algorithm(
		"bullet-broadphase-algorithm", BulletWorld::BA_dynamic_aabb_tree,
		PRC_DESC("Specifies the broadphase algorithm to be used by the physics "
				"engine. Default value is 'aabb' (dynamic aabb tree)."));

ConfigVariableEnum<BulletWorld::FilterAlgorithm> bullet_filter_algorithm(
		"bullet-filter-algorithm", BulletWorld::FA_mask,
		PRC_DESC("Specifies the algorithm to be used by the physics engine for "
				"collision filtering. Default value is 'mask'."));

ConfigVariableDouble bullet_sap_extents("bullet-sap-extents", 1000.0,
		PRC_DESC(
				"Specifies the world extent in all directions. The config variable "
						"is only used if bullet-broadphase-algorithm is set to 'sap' "
						"(sweep and prune). Default value is 1000.0."));

ConfigVariableBool bullet_enable_contact_events("bullet-enable-contact-events",
		false,
		PRC_DESC("Specifies if events should be send when new contacts are "
				"created or existing contacts get remove. Warning: enabling "
				"contact events might create more load on the event queue "
				"then you might want! Default value is FALSE."));

ConfigVariableInt bullet_solver_iterations("bullet-solver-iterations", 10,
		PRC_DESC("Specifies the number of iterations for the Bullet contact "
				"solver. This is the native Bullet property "
				"btContactSolverInfo::m_numIterations. Default value is 10."));

ConfigVariableBool bullet_additional_damping("bullet-additional-damping", false,
		PRC_DESC(
				"Enables additional damping on eachrigid body, in order to reduce "
						"jitter. Default value is FALSE. Additional damping is an "
						"experimental feature of the Bullet physics engine. Use with "
						"care."));

ConfigVariableDouble bullet_additional_damping_linear_factor(
		"bullet-additional-damping-linear-factor", 0.005,
		PRC_DESC("Only used when bullet-additional-damping is set to TRUE. "
				"Default value is 0.005"));

ConfigVariableDouble bullet_additional_damping_angular_factor(
		"bullet-additional-damping-angular-factor", 0.01,
		PRC_DESC("Only used when bullet-additional-damping is set to TRUE. "
				"Default value is 0.01"));

ConfigVariableDouble bullet_additional_damping_linear_threshold(
		"bullet-additional-damping-linear-threshold", 0.01,
		PRC_DESC("Only used when bullet-additional-damping is set to TRUE. "
				"Default value is 0.01"));

ConfigVariableDouble bullet_additional_damping_angular_threshold(
		"bullet-additional-damping-angular-threshold", 0.01,
		PRC_DESC("Only used when bullet-additional-damping is set to TRUE. "
				"Default value is 0.01."));

////////////////////////////////////////////////////////////////////
//     Function: init_libbullet
//  Description: Initializes the library. This must be called at
//               least once before any of the functions or classes in
//               this library can be used. Normally it will be
//               called by the static initializers and need not be
//               called explicitly, but special cases exist.
////////////////////////////////////////////////////////////////////
void init_libbullet()
{

	static bool initialized = false;
	if (initialized)
	{
		return;
	}
	initialized = true;

	// Initialize types
	BulletBaseCharacterControllerNode::init_type();
	BulletBodyNode::init_type();
	BulletBoxShape::init_type();
	BulletCapsuleShape::init_type();
	BulletCharacterControllerNode::init_type();
	BulletConeShape::init_type();
	BulletConeTwistConstraint::init_type();
	BulletContactCallbackData::init_type();
	BulletConstraint::init_type();
	BulletConvexHullShape::init_type();
	BulletConvexPointCloudShape::init_type();
	BulletCylinderShape::init_type();
	BulletMinkowskiSumShape::init_type();
	BulletDebugNode::init_type();
	BulletFilterCallbackData::init_type();
	BulletGenericConstraint::init_type();
	BulletGhostNode::init_type();
	BulletHeightfieldShape::init_type();
	BulletHingeConstraint::init_type();
	BulletMultiSphereShape::init_type();
	BulletPlaneShape::init_type();
	BulletRigidBodyNode::init_type();
	BulletShape::init_type();
	BulletSliderConstraint::init_type();
	BulletSphereShape::init_type();
	BulletSphericalConstraint::init_type();
	BulletSoftBodyNode::init_type();
	BulletSoftBodyShape::init_type();
	BulletTickCallbackData::init_type();
	BulletTriangleMesh::init_type();
	BulletTriangleMeshShape::init_type();
	BulletVehicle::init_type();
	BulletWorld::init_type();

	// Custom contact callbacks
	gContactAddedCallback = contact_added_callback;
	gContactProcessedCallback = contact_processed_callback;
	gContactDestroyedCallback = contact_destroyed_callback;

	// Initialize notification category
	PRINT_DEBUG("initialize module");

}

}
