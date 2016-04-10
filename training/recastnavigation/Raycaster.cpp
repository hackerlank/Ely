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
 * \file /Ely/src/Support/Raycaster.cpp
 *
 * \date 2013-04-28 
 * \author consultit
 */

#include "Raycaster.h"

Raycaster::Raycaster(PandaFramework* app, WindowFramework* window,
SMARTPTR(BulletWorld)world, int N) : mApp(app), mWindow(window), mWorld(world),
		mNumCallbacks(N), mHitResult(BulletClosestHitRayResult::empty())
{
	//get render, camera node paths
	mRender = mWindow->get_render();
	mCamera = mWindow->get_camera_group();
	mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
	if(mNumCallbacks <= 0)
	{
		mNumCallbacks = 3;
	}
	//allocated room for vectors
	mCallback.resize(mNumCallbacks);
	mData.resize(mNumCallbacks);
	mBitMask.resize(mNumCallbacks);
	mHitBodyData.resize(mNumCallbacks);
	mHitKey.resize(mNumCallbacks);
	//reset callback data
	for (int i = 0; i < mNumCallbacks; ++i)
	{
		mHitBodyData[i].clear();
	}
	//reset other members
	mHitNode = NULL;
	mHitPos = mFromPos = mToPos = LPoint3f::zero();
	mHitNormal = LVector3f::zero();
	mHitFraction = 0.0;
}

Raycaster::~Raycaster()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (mApp)
	{
		// remove event callbacks for picking body
		for (int i = 0; i < mNumCallbacks; ++i)
		{
			if (mHitBodyData[i])
			{
				mApp->get_event_handler().remove_hooks_with(
						reinterpret_cast<void*>(mHitBodyData[i].p()));
			}
		}
	}
}

void Raycaster::setHitCallback(int index, void (*callback)(Raycaster*, void*),
		void* data, const std::string& hitKey, BitMask32 bitMask)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (not ((index >= 0) and (index < mNumCallbacks)))
	{
		return;
	}
	//set the callback
	doSetCallback(index, callback, data, hitKey, bitMask);
}

bool Raycaster::setHitCallback(void (*callback)(Raycaster*, void*), void* data,
		const std::string& hitKey, BitMask32 bitMask)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//get the first callback free slot
	int freeIdx = 0;
	while (freeIdx < mNumCallbacks)
	{
		if (mCallback[freeIdx])
		{
			break;
		}
		++freeIdx;
	}
	RETURN_ON_COND(freeIdx >= mNumCallbacks, false)
	//there is a free slot
	doSetCallback(freeIdx, callback, data, hitKey, bitMask);
	return true;
}

void Raycaster::doSetCallback(int index, void (*callback)(Raycaster*, void*),
		void* data, const std::string& hitKey, BitMask32 bitMask)
{
	mCallback[index] = callback;
	mData[index] = data;
	// setup event callback for picking body
	mHitKey[index] = hitKey;
	mBitMask[index] = bitMask;
	//first remove old hooks
	if (mHitBodyData[index])
	{
		mApp->get_event_handler().remove_hooks_with(
				reinterpret_cast<void*>(mHitBodyData[index].p()));
	}
	mHitBodyData[index] =
			new EventCallbackInterface<Raycaster>::EventCallbackData(this,
					&Raycaster::hitBodyCallback);
	mApp->define_key(mHitKey[index],
			"HitBody_"
					+ dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(
							index)).str(),
			&EventCallbackInterface<Raycaster>::eventCallbackFunction,
			reinterpret_cast<void*>(mHitBodyData[index].p()));
}

void Raycaster::hitBodyCallback(const Event* event)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//get OnPickKey index (if any)
	std::string eventName = event->get_name();
	int idx = 0;
	while (idx < mNumCallbacks)
	{
		if (eventName == mHitKey[idx])
		{
			break;
		}
		++idx;
	}

	// handle body picking
	if (mCallback[idx] and (idx < mNumCallbacks))
	{
		doRayCast(mBitMask[idx]);
		if (mHitNode)
		{
			//there was an hit: call the related callback
			mCallback[idx](this, mData[idx]);
		}
	}
}

bool Raycaster::rayCast(const BitMask32& bitMask)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	doRayCast(bitMask);

	//return the hit Object
	return mHitResult.has_hit();
}

inline void Raycaster::doRayCast(const BitMask32& bitMask)
{
	//get the mouse watcher
	SMARTPTR(MouseWatcher)mwatcher = DCAST(MouseWatcher,
			mWindow->get_mouse().node());
	if (mwatcher->has_mouse())
	{
		// Get to and from pos in camera coordinates
		LPoint2f pMouse = mwatcher->get_mouse();
		//
		LPoint3f pFrom, pTo;
		if (mCamLens->extrude(pMouse, pFrom, pTo))
		{
			//Transform to global coordinates
			pFrom = mRender.get_relative_point(mCamera, pFrom);
			pTo = mRender.get_relative_point(mCamera, pTo);

			HOLD_REMUTEX(GamePhysicsManager::GetSingletonPtr()->getMutex())
			{
				//cast a ray to detect a body
				mHitResult = mWorld->ray_test_closest(pFrom, pTo, bitMask);
			}
			//
			if (mHitResult.has_hit())
			{
				//possible hit objects:
				//- BulletRigidBodyNode
				//- BulletCharacterControllerNode
				//- BulletVehicle
				//- BulletConstraint
				//- BulletSoftBodyNode
				//- BulletGhostNode

				mHitNode = const_cast<PandaNode*>(mHitResult.get_node());
				mHitPos = mHitResult.get_hit_pos();
				mHitNormal = mHitResult.get_hit_normal();
				mHitFraction = mHitResult.get_hit_fraction();
				mFromPos = mHitResult.get_from_pos();
				mToPos = mHitResult.get_to_pos();
			}
		}
	}
}


