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
 * \file /Ely/elygame/initializations/defs/SoftObject_init.cpp
 *
 * \date 2013-10-20 
 * \author consultit
 */

#include "../common_configs.h"
#include "PhysicsComponents/SoftBody.h"
#include <bulletSoftBodyMaterial.h>
#include <bulletSoftBodyConfig.h>
#include <bulletShape.h>

///Soft objects related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION cover1_initialization;
INITIALIZATION softBall1_initialization;
INITIALIZATION softCube1_initialization;
INITIALIZATION softModel1_initialization;

#ifdef __cplusplus
}
#endif

///cover1
void cover1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//get SoftBody
	SMARTPTR(SoftBody) softBody =
			DCAST(SoftBody, object->getComponent(ComponentFamilyType("Physics")));
	if(softBody)
	{
		HOLD_REMUTEX(softBody->getMutex())

		BulletSoftBodyNode& bulletSoftBodyNode = *softBody;
		BulletSoftBodyMaterial material = bulletSoftBodyNode.append_material();
    	material.setLinearStiffness(0.4);
    	bulletSoftBodyNode.generate_bending_constraints(2, &material);
    	bulletSoftBodyNode.get_shape(0)->set_margin(0.5);
	}
}

///softBall1
void softBall1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//get SoftBody
	SMARTPTR(SoftBody) softBody =
			DCAST(SoftBody, object->getComponent(ComponentFamilyType("Physics")));
	if(softBody)
	{
		HOLD_REMUTEX(softBody->getMutex())

		BulletSoftBodyNode& bulletSoftBodyNode = *softBody;
		bulletSoftBodyNode.set_name("softBall1-Ellipsoid");
		bulletSoftBodyNode.get_material(0).setLinearStiffness(0.1);
		bulletSoftBodyNode.get_cfg().set_dynamic_friction_coefficient(1.0);
		bulletSoftBodyNode.get_cfg().set_damping_coefficient(0.001);
		bulletSoftBodyNode.get_cfg().set_pressure_coefficient(1500.0);
		bulletSoftBodyNode.set_pose(true, false);
	}
}

///softCube1
void softCube1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//get SoftBody
	SMARTPTR(SoftBody) softBody =
			DCAST(SoftBody, object->getComponent(ComponentFamilyType("Physics")));
	if(softBody)
	{
		HOLD_REMUTEX(softBody->getMutex())

		BulletSoftBodyNode& bulletSoftBodyNode = *softBody;
		bulletSoftBodyNode.set_name("softCube1-Tetramesh");
		bulletSoftBodyNode.get_shape(0)->set_margin(0.01);
		bulletSoftBodyNode.get_material(0).setLinearStiffness(0.1);
		bulletSoftBodyNode.get_cfg().set_positions_solver_iterations(1);
		bulletSoftBodyNode.get_cfg().clear_all_collision_flags();
		bulletSoftBodyNode.get_cfg().set_collision_flag(BulletSoftBodyConfig::CF_cluster_soft_soft, true);
		bulletSoftBodyNode.get_cfg().set_collision_flag(BulletSoftBodyConfig::CF_cluster_rigid_soft, true);
		bulletSoftBodyNode.generate_clusters(6);
	}
}

///softModel1
void softModel1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//get SoftBody
	SMARTPTR(SoftBody) softBody =
			DCAST(SoftBody, object->getComponent(ComponentFamilyType("Physics")));
	if(softBody)
	{
		HOLD_REMUTEX(softBody->getMutex())

		BulletSoftBodyNode& bulletSoftBodyNode = *softBody;
		bulletSoftBodyNode.set_name("softModel1-Trimesh");
		bulletSoftBodyNode.generate_bending_constraints(2);
		bulletSoftBodyNode.get_cfg().set_positions_solver_iterations(2);
		bulletSoftBodyNode.get_cfg().set_collision_flag(BulletSoftBodyConfig::CF_vertex_face_soft_soft,true);
		bulletSoftBodyNode.randomize_constraints();
	}
}

///init/end
void SoftObject_initInit()
{
}

void SoftObject_initEnd()
{
}
