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
 * \file /Ely/training/opensteer/common.h
 *
 * \date 17/nov/2013 (11:11:37)
 * \author consultit
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <nodePath.h>
#include <OpenSteer/Vec3.h>

namespace ely
{
//LVecBase3f-OpenSteer::Vec3 conversion functions
inline OpenSteer::Vec3 LVecBase3fToOpenSteerVec3(const LVecBase3f& v)
{
	return OpenSteer::Vec3(v.get_x(), v.get_z(), -v.get_y());
}
inline LVecBase3f OpenSteerVec3ToLVecBase3f(const OpenSteer::Vec3& v)
{
	return LVecBase3f(v.x, -v.z, v.y);
}

template<class Super>
class ActorMixin: public Super
{
public:

	void update(const float currentTime, const float elapsedTime)
	{
		Super::update(currentTime, elapsedTime);
		//update actor
		LPoint3f pos = OpenSteerVec3ToLVecBase3f(Super::position());
		mActor.set_pos(pos);
		mActor.heads_up(pos - OpenSteerVec3ToLVecBase3f(Super::forward()),
				OpenSteerVec3ToLVecBase3f(Super::up()));
	}

	void setActor(NodePath actor)
	{
		mActor = actor;
	}

protected:
	NodePath mActor;

};
}

#endif /* COMMON_H_ */
