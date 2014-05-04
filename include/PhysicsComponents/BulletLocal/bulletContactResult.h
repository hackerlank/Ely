// Filename: bulletContactResult.h
// Created by:  enn0x (08Mar10)
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

#ifndef __BULLET_CONTACT_RESULT_H__
#define __BULLET_CONTACT_RESULT_H__

#include <pandabase.h>

#include "bullet_includes.h"
#include "bulletManifoldPoint.h"

#include <pvector.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletContact
// Description : 
////////////////////////////////////////////////////////////////////
struct BulletContact
{

public:
	BulletContact();
	BulletContact(const BulletContact &other);

public:
	inline BulletManifoldPoint &get_manifold_point();
	inline PandaNode *get_node0() const;
	inline PandaNode *get_node1() const;
	inline const int get_idx0() const;
	inline const int get_idx1() const;
	inline const int get_part_id0() const;
	inline const int get_part_id1() const;

private:
	static btManifoldPoint _empty;

	BulletManifoldPoint _mp;

	PT(PandaNode)_node0;
	PT(PandaNode) _node1;

	int _part_id0;
	int _part_id1;
	int _idx0;
	int _idx1;

	friend struct BulletContactResult;
};

////////////////////////////////////////////////////////////////////
//       Class : BulletContactResult
// Description : 
////////////////////////////////////////////////////////////////////
struct BulletContactResult: public btCollisionWorld::ContactResultCallback
{

public:
	inline int get_num_contacts() const;
	inline BulletContact &get_contact(int idx);MAKE_SEQ(get_contacts, get_num_contacts, get_contact)
	;

public:
#if BT_BULLET_VERSION >= 281
	virtual bool needsCollision(btBroadphaseProxy *proxy0) const;

	virtual btScalar addSingleResult(btManifoldPoint &mp,
			const btCollisionObjectWrapper *wrap0, int part_id0, int idx0,
			const btCollisionObjectWrapper *wrap1, int part_id1, int idx1);
#else
	virtual btScalar addSingleResult(btManifoldPoint &mp,
			const btCollisionObject *obj0, int part_id0, int idx0,
			const btCollisionObject *obj1, int part_id1, int idx1);
#endif

protected:
	BulletContactResult();

#if BT_BULLET_VERSION >= 281
	void use_filter(btOverlapFilterCallback *cb, btBroadphaseProxy *proxy);
#endif

private:
	static BulletContact _empty;

	btAlignedObjectArray<BulletContact> _contacts;

#if BT_BULLET_VERSION >= 281
	bool _filter_set;
	btOverlapFilterCallback *_filter_cb;
	btBroadphaseProxy *_filter_proxy;
#endif

	friend class BulletWorld;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletContact::get_node0
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletContact::get_node0() const
{

	return _node0;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContact::get_node1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline PandaNode *BulletContact::get_node1() const
{

	return _node1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContact::get_manifold_point
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletManifoldPoint &BulletContact::get_manifold_point()
{

	return _mp;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContact::get_idx0
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline const int BulletContact::get_idx0() const
{

	return _idx0;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContact::get_idx1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline const int BulletContact::get_idx1() const
{

	return _idx1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContact::get_part_id0
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline const int BulletContact::get_part_id0() const
{

	return _part_id0;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContact::get_part_id1
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline const int BulletContact::get_part_id1() const
{

	return _part_id1;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactResult::get_num_contacts
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline int BulletContactResult::get_num_contacts() const
{

	return _contacts.size();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletContactResult::get_contact
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletContact &BulletContactResult::get_contact(int idx)
{

	nassertr(idx >= 0 && idx < (int ) _contacts.size(), _empty);
	return _contacts[idx];
}

}
#endif // __BULLET_CONTACT_RESULT_H__
