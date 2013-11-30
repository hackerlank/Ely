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
 * \file /Ely/include/AIComponents/OpenSteer/common.h
 *
 * \date 30/nov/2013 (11:11:37)
 * \author consultit
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <algorithm>
#include <nodePath.h>
#include <animControlCollection.h>
#include <OpenSteer/Vec3.h>
#include <OpenSteer/Color.h>
#include <OpenSteer/AbstractVehicle.h>

extern bool gToggleDrawGrid;
extern OpenSteer::AbstractVehicle* selectedVehicle;

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
//LVecBase4f-OpenSteer::Color conversion functions
inline OpenSteer::Color LVecBase4fToOpenSteerColor(const LVecBase4f& c)
{
	return OpenSteer::Color(c.get_x(), c.get_y(), c.get_z(), c.get_w());
}
inline LVecBase4f OpenSteerColorToLVecBase4f(const OpenSteer::Color& c)
{
	return LVecBase4f(c.r(), c.g(), c.b(), c.a());
}

template<typename Super, typename P1 = int, typename P2 = int,
		typename P3 = int, typename P4 = int, typename P5 = int>
class ActorMixin: public Super
{
public:
	ActorMixin()
	{
	}

	ActorMixin(P1 p1) :
			Super(p1)
	{
	}

	ActorMixin(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) :
			Super(p1, p2, p3, p4, p5)
	{
	}

	void update(const float currentTime, const float elapsedTime)
	{
		Super::update(currentTime, elapsedTime);
		//update actor
		LPoint3f pos = OpenSteerVec3ToLVecBase3f(Super::position());
		mActor.set_pos(pos);
		mActor.heads_up(pos - OpenSteerVec3ToLVecBase3f(Super::forward()),
				OpenSteerVec3ToLVecBase3f(Super::up()));

		//update anim if any
		if (mAnims.get_num_anims() > 0)
		{
			//get relative speed
			float relSpeed = Super::relativeSpeed();
			if (relSpeed >= 0.1)
			{
				mAnims.get_anim(0)->set_play_rate(relSpeed * mAnimRateFactor);
				if (not mAnims.get_anim(0)->is_playing())
				{
					mAnims.get_anim(0)->loop(true);
				}
			}
			else
			{
				if (mAnims.get_anim(0)->is_playing())
				{
					mAnims.get_anim(0)->stop();
				}
			}
		}
	}

	void setActor(NodePath actor)
	{
		mActor = actor;
		// set size of bounding sphere
		LPoint3f minP, maxP;
		actor.calc_tight_bounds(minP, maxP);
		Super::setRadius((maxP - minP).length() / 2.0);
	}

	void setAnimRateFactor(float animRateFactor)
	{
		mAnimRateFactor = animRateFactor;
	}

	AnimControlCollection& getAnims()
	{
		return mAnims;
	}

protected:
	NodePath mActor;
	AnimControlCollection mAnims;
	float mAnimRateFactor;
};

}

namespace OpenSteer
{
void gridUtility(const Vec3& gridTarget, const float size = 50,
		const int subsquares = 50);
}

#endif /* COMMON_H_ */
