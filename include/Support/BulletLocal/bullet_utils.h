// Filename: bullet_utils.h
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

#ifndef __BULLET_UTILS_H__
#define __BULLET_UTILS_H__

#include <pandabase.h>

#include "bullet_includes.h"

#include <luse.h>
#include <pointerTo.h>
#include <pandaNode.h>
#include <nodePath.h>

namespace ely
{
// Conversion from Panda3D to Bullet
btVector3 LVecBase3_to_btVector3(const LVecBase3 &v);
btMatrix3x3 LMatrix3_to_btMatrix3x3(const LMatrix3 &m);
btTransform LMatrix4_to_btTrans(const LMatrix4 &m);
btQuaternion LQuaternion_to_btQuat(const LQuaternion &q);

// Conversion from Bullet to Panda3D
LVecBase3 btVector3_to_LVecBase3(const btVector3 &v);
LVector3 btVector3_to_LVector3(const btVector3 &v);
LPoint3 btVector3_to_LPoint3(const btVector3 &p);
LMatrix3 btMatrix3x3_to_LMatrix3(const btMatrix3x3 &m);
LMatrix4 btTrans_to_LMatrix4(const btTransform &tf);
LQuaternion btQuat_to_LQuaternion(const btQuaternion &q);

CPT(TransformState)btTrans_to_TransformState(
		const btTransform &tf,
		const LVecBase3 &scale=LVecBase3(1.0f, 1.0f, 1.0f));

btTransform TransformState_to_btTrans(
CPT(TransformState)ts);

// UpAxis
enum BulletUpAxis
{
	X_up = 0, Y_up = 1, Z_up = 2,
};

BulletUpAxis get_default_up_axis();
void get_node_transform(btTransform &trans, PandaNode *node);
int get_bullet_version();

}
#endif // __BULLET_UTILS_H__
