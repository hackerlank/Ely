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
 * \author marco
 */

#ifndef COMPONENTSUITE_H_
#define COMPONENTSUITE_H_

/// WARNING: include this file only in source files and not into headers.

#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"

/// List of all components and component templates
#include "SceneComponents/Model.h"
#include "SceneComponents/ModelTemplate.h"
#include "SceneComponents/InstanceOf.h"
#include "SceneComponents/InstanceOfTemplate.h"
#include "SceneComponents/NodePathWrapper.h"
#include "SceneComponents/NodePathWrapperTemplate.h"
#include "InputComponents/ControlByEvent.h"
#include "InputComponents/ControlByEventTemplate.h"
#include "AudioComponents/Sound3d.h"
#include "AudioComponents/Sound3dTemplate.h"
#include "AudioComponents/Listener.h"
#include "AudioComponents/ListenerTemplate.h"

#endif /* COMPONENTSUITE_H_ */
