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
 * \file /Ely/ely/initializations/defs/SoftObjects.cpp
 *
 * \date 20/ott/2013 (10:01:07)
 * \author consultit
 */

#include "../common_configs.h"
#include "PhysicsComponents/SoftBody.h"
#include <bulletSoftBodyMaterial.h>
#include <bulletShape.h>

///Soft objects related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION cover1_initialization;

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

///init/end
void softObjectsInit()
{
}

void softObjectsEnd()
{
}
