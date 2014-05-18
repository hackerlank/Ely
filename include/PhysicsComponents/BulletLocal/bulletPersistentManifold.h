// Filename: bulletPersistentManifold.h
// Created by:  enn0x (07Mar10)
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

#ifndef __BULLET_PERSISTENT_MANIFOLD_H__
#define __BULLET_PERSISTENT_MANIFOLD_H__

#include <pandabase.h>

#include "bullet_includes.h"

#include <pandaNode.h>

namespace ely
{
class BulletManifoldPoint;

////////////////////////////////////////////////////////////////////
//       Class : BulletPersistentManifold
// Description : 
////////////////////////////////////////////////////////////////////
class BulletPersistentManifold
{

public:
	inline ~BulletPersistentManifold();

	const PandaNode *get_node0();
	const PandaNode *get_node1();

	int get_num_manifold_points() const;
	BulletManifoldPoint *get_manifold_point(int idx) const;
	;

	PN_stdfloat get_contact_breaking_threshold() const;
	PN_stdfloat get_contact_processing_threshold() const;

	void clear_manifold();

public:
	BulletPersistentManifold(btPersistentManifold *manifold);

private:
	btPersistentManifold *_manifold;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletPersistentManifold::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletPersistentManifold::~BulletPersistentManifold()
{

}
}
#endif // __BULLET_PERSISTENT_MANIFOLD_H__
