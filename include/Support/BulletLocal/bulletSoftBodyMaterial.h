// Filename: bulletSoftBodyMaterial.h
// Created by:  enn0x (19Mar11)
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

#ifndef __BULLET_SOFT_BODY_MATERIAL_H__
#define __BULLET_SOFT_BODY_MATERIAL_H__

#include <pandabase.h>

#include "bullet_includes.h"

#include <numeric_types.h>

namespace ely
{
////////////////////////////////////////////////////////////////////
//       Class : BulletSoftBodyMaterial
// Description : 
////////////////////////////////////////////////////////////////////
class BulletSoftBodyMaterial
{

public:
	inline ~BulletSoftBodyMaterial();
	inline static BulletSoftBodyMaterial empty();

	inline void setLinearStiffness(PN_stdfloat value);
	inline void setAngularStiffness(PN_stdfloat value);
	inline void setVolumePreservation(PN_stdfloat value);

	inline PN_stdfloat getLinearStiffness() const;
	inline PN_stdfloat getAngularStiffness() const;
	inline PN_stdfloat getVolumePreservation() const;

public:
	BulletSoftBodyMaterial(btSoftBody::Material &material);

	inline btSoftBody::Material &get_material() const;

private:
	btSoftBody::Material &_material;
};

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::Destructor
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
inline BulletSoftBodyMaterial::~BulletSoftBodyMaterial()
{

}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::empty
//       Access: Published
//  Description: Named constructor intended to be used for asserts
//               with have to return a concrete value.
////////////////////////////////////////////////////////////////////
inline BulletSoftBodyMaterial BulletSoftBodyMaterial::empty()
{

	static btSoftBody::Material material;

	return BulletSoftBodyMaterial(material);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyWorldInfo::get_material
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
inline btSoftBody::Material &BulletSoftBodyMaterial::get_material() const
{

	return _material;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::getLinearStiffness
//       Access: Published
//  Description: Getter for the property m_kLST.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyMaterial::getLinearStiffness() const
{

	return (PN_stdfloat) _material.m_kLST;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::setLinearStiffness
//       Access: Published
//  Description: Setter for the property m_kLST.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyMaterial::setLinearStiffness(PN_stdfloat value)
{

	_material.m_kLST = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::getAngularStiffness
//       Access: Published
//  Description: Getter for the property m_kAST.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyMaterial::getAngularStiffness() const
{

	return (PN_stdfloat) _material.m_kAST;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::setAngularStiffness
//       Access: Published
//  Description: Setter for the property m_kAST.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyMaterial::setAngularStiffness(PN_stdfloat value)
{

	_material.m_kAST = (btScalar) value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::getVolumePreservation
//       Access: Published
//  Description: Getter for the property m_kVST.
////////////////////////////////////////////////////////////////////
inline PN_stdfloat BulletSoftBodyMaterial::getVolumePreservation() const
{

	return (PN_stdfloat) _material.m_kVST;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletSoftBodyMaterial::setVolumePreservation
//       Access: Published
//  Description: Setter for the property m_kVST.
////////////////////////////////////////////////////////////////////
inline void BulletSoftBodyMaterial::setVolumePreservation(PN_stdfloat value)
{

	_material.m_kVST = (btScalar) value;
}

}
#endif // __BULLET_SOFT_BODY_MATERIAL_H__
