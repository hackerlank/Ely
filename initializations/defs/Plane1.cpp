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
 * \file /Ely/initializations/defs/Plane1.cpp
 *
 * \date 14/lug/2013 (08:47:19)
 * \author consultit
 */

#include "../common_configs.h"
#include <texturePool.h>
#include "SceneComponents/Model.h"

///Plane1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Plane1_initialization;

#ifdef __cplusplus
}
#endif

void Plane1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Plane1
	SMARTPTR(Model) plane1Model = DCAST(Model, object->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(TextureStage) planeTS0 = new TextureStage("planeTS0");
	SMARTPTR(Texture) planeTex = TexturePool::load_texture("maps/envir-ground.jpg");
	plane1Model->getNodePath().set_texture(planeTS0, planeTex, 1);
	plane1Model->getNodePath().set_tex_scale(planeTS0, 100, 100);
}

void Plane1Init()
{
}

void Plane1End()
{
}
