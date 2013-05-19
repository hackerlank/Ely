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
 * \date 28/apr/2013 (12:50:35)
 * \author consultit
 */

#include "Support/Raycaster.h"

Raycaster::Raycaster(PandaFramework* app, WindowFramework* window,
		SMARTPTR(BulletWorld)world, int N) :
		mApp(app), mWindow(window), mWorld(world), m_N(N)
{
	//get render, camera node paths
	mRender = window->get_render();
	mCamera = window->get_camera_group();
	mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
	if(m_N <= 0)
	{
		m_N = 3;
	}
	//allocated room for vectors
	mCallback.resize(m_N);
	mData.resize(m_N);
	mBitMask.resize(m_N);
	mHitBodyData.resize(m_N);
	mHitKey.resize(m_N);
	//reset callback data
	for (int i = 0; i < m_N; ++i)
	{
		mHitBodyData[i].clear();
	}
}

Raycaster::~Raycaster()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mApp)
	{
		// remove event callbacks for picking body
		for (int i = 0; i < m_N; ++i)
		{
			if (mHitBodyData[i])
			{
				mApp->get_event_handler().remove_hooks(mHitKey[i]);
			}
		}
	}
}

void Raycaster::setHitCallback(int index, void (*callback)(Raycaster*, void*),
		void* data,	const std::string& hitKey, BitMask32 bitMask)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (not ((index >= 0) and (index < m_N)))
	{
		return;
	}
	mCallback[index] = callback;
	mData[index] = data;
	// setup event callback for picking body
	mHitKey[index] = hitKey;
	mBitMask[index] = bitMask;
	//first remove old hooks
	if (mHitBodyData[index])
	{
		mApp->get_event_handler().remove_hooks(mHitKey[index]);
	}
	mHitBodyData[index] = new EventCallbackInterface<Raycaster>::EventCallbackData(this,
			&Raycaster::hitBody);
	ostringstream idx;
	idx << index;
	mApp->define_key(mHitKey[index], "HitBody_" + idx.str(),
			&EventCallbackInterface<Raycaster>::eventCallbackFunction,
			reinterpret_cast<void*>(mHitBodyData[index].p()));
}

const PandaNode* Raycaster::getHitNode()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mHitNode;
}

LPoint3f Raycaster::getHitPos()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mHitPos;
}

LPoint3f Raycaster::getFromPos()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mFromPos;
}

LPoint3f Raycaster::getToPos()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mToPos;
}

LVector3f Raycaster::getHitNormal()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mHitNormal;
}

float Raycaster::getHitFraction()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mHitFraction;
}

void Raycaster::hitBody(const Event* event)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//get OnPickKey index (if any)
	std::string eventName = event->get_name();
	int idx = 0;
	while(idx < m_N)
	{
		if (eventName == mHitKey[idx])
		{
			break;
		}
		++idx;
	}

	// handle body picking
	if (mCallback[idx] and (idx < m_N))
	{
		//get the mouse watcher
		SMARTPTR(MouseWatcher)mwatcher =
		DCAST(MouseWatcher, mWindow->get_mouse().node());
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
				//cast a ray to detect a body
				BulletClosestHitRayResult result = mWorld->ray_test_closest(pFrom,
						pTo, mBitMask[idx]);
				//
				if (result.has_hit())
				{
					//possible hit objects:
					//- BulletRigidBodyNode
					//- BulletCharacterControllerNode
					//- BulletVehicle
					//- BulletConstraint
					//- BulletSoftBodyNode
					//- BulletGhostNode

					mHitNode = const_cast<PandaNode*>(result.get_node());
					mHitPos = result.get_hit_pos();
					mHitNormal = result.get_hit_normal();
					mHitFraction = result.get_hit_fraction();
					mFromPos = result.get_from_pos();
					mToPos = result.get_to_pos();
					mCallback[idx](this, mData[idx]);
				}
			}
		}
	}
}

ReMutex& Raycaster::getMutex()
{
	return mMutex;
}





