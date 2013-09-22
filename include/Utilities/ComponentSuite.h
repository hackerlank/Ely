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
 * \date 17/mag/2012 (18:22:24)
 * \author consultit
 */

#ifndef COMPONENTSUITE_H_
#define COMPONENTSUITE_H_

/// WARNING: include this file only in source files and not into headers.

#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"

/// List of all components and component templates
#include "AIComponents/Steering.h"
#include "AIComponents/SteeringTemplate.h"
#include "AIComponents/CrowdAgent.h"
#include "AIComponents/CrowdAgentTemplate.h"
#include "AIComponents/NavMesh.h"
#include "AIComponents/NavMeshTemplate.h"
#include "AudioComponents/Listener.h"
#include "AudioComponents/ListenerTemplate.h"
#include "AudioComponents/Sound3d.h"
#include "AudioComponents/Sound3dTemplate.h"
#include "BehaviorComponents/Activity.h"
#include "BehaviorComponents/ActivityTemplate.h"
#include "ControlComponents/Chaser.h"
#include "ControlComponents/ChaserTemplate.h"
#include "ControlComponents/Driver.h"
#include "ControlComponents/DriverTemplate.h"
#include "PhysicsComponents/CharacterController.h"
#include "PhysicsComponents/CharacterControllerTemplate.h"
#include "PhysicsComponents/RigidBody.h"
#include "PhysicsComponents/RigidBodyTemplate.h"
#include "PhysicsComponents/Vehicle.h"
#include "SceneComponents/InstanceOf.h"
#include "SceneComponents/InstanceOfTemplate.h"
#include "SceneComponents/Model.h"
#include "SceneComponents/ModelTemplate.h"
#include "SceneComponents/NodePathWrapper.h"
#include "SceneComponents/NodePathWrapperTemplate.h"
#include "SceneComponents/Terrain.h"
#include "SceneComponents/TerrainTemplate.h"

#endif /* COMPONENTSUITE_H_ */
