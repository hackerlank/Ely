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
 * \file /Ely/training/recastnavigation/Raycaster.cpp
 *
 * \date 29/mar/2013 (18:04:03)
 * \author marco
 */

#include "Raycaster.h"

Raycaster::Raycaster(PandaFramework* app, WindowFramework* window,
		SMARTPTR(BulletWorld)world,	const std::string& pickKeyOn,
		const std::string& pickKeyOff, BitMask32 bitMask) :
		mApp(app), mWindow(window), mWorld(world),
		mCallback(NULL), mBitMask(bitMask)
{
	//get render, camera node paths
	mRender = window->get_render();
	mCamera = window->get_camera_group();
	mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
	// setup event callback for picking body
	mPickKeyOn = pickKeyOn;
	mPickKeyOff = pickKeyOff;
	mPickBodyData = new EventCallbackInterface<Raycaster>::EventCallbackData(this,
			&Raycaster::hitBody);
	mApp->define_key(mPickKeyOn, "pickBody",
			&EventCallbackInterface<Raycaster>::eventCallbackFunction,
			reinterpret_cast<void*>(mPickBodyData.p()));
	mApp->define_key(mPickKeyOff, "pickBodyUp",
			&EventCallbackInterface<Raycaster>::eventCallbackFunction,
			reinterpret_cast<void*>(mPickBodyData.p()));
	//
}

Raycaster::~Raycaster()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mApp)
	{
		// remove event callback for picking body
		mApp->get_event_handler().remove_hooks_with(
				reinterpret_cast<void*>(mPickBodyData.p()));
	}
}

void Raycaster::hitBody(const Event* event)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	// handle body picking
	if (mCallback and event->get_name() == mPickKeyOn)
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
						pTo, mBitMask);
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

					mHitNode = result.get_node()->get_name();
					mHitPos = result.get_hit_pos();
					mHitNormal = result.get_hit_normal();
					mHitFraction = result.get_hit_fraction();
					mFromPos = result.get_from_pos();
					mToPos = result.get_to_pos();
					mCallback(this, mData);
				}
			}
		}
	}
}

ReMutex& Raycaster::getMutex()
{
	return mMutex;
}





