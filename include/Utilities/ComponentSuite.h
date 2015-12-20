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
 * \file /Ely/include/Utilities/ComponentSuite.h
 *
 * \date 2012-05-17 
 * \author consultit
 */

#ifndef COMPONENTSUITE_H_
#define COMPONENTSUITE_H_

/// WARNING: include this file only in source files and not into headers.

#include "ObjectModel/Component.h"

/// List of all components and component templates
#include "AIComponents/CrowdAgent.h"
#include "AIComponents/NavMesh.h"
#include "AIComponents/SteerPlugIn.h"
#include "AIComponents/SteerVehicle.h"
#include "AudioComponents/Listener.h"
#include "AudioComponents/Sound3d.h"
#include "BehaviorComponents/Activity.h"
#include "CommonComponents/Default.h"
#include "CommonComponents/GameConfig.h"
#include "ControlComponents/Chaser.h"
#include "ControlComponents/Driver.h"
#include "PhysicsComponents/Ghost.h"
#include "PhysicsComponents/RigidBody.h"
#include "PhysicsComponents/SoftBody.h"
#include "PhysicsControlComponents/CharacterController.h"
#include "PhysicsControlComponents/Vehicle.h"
#include "SceneComponents/InstanceOf.h"
#include "SceneComponents/Model.h"
#include "SceneComponents/NodePathWrapper.h"
#include "SceneComponents/Terrain.h"

#endif /* COMPONENTSUITE_H_ */
