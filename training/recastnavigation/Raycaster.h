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
 * \file /Ely/training/recastnavigation/Raycaster.h
 *
 * \date 29/mar/2013 (18:03:35)
 * \author marco
 */


#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include "Utilities/Tools.h"
#include <iostream>
#include <string>
#include <vector>
#include <pandaFramework.h>
#include <bulletWorld.h>
#include <bulletTriangleMesh.h>
#include <bulletTriangleMeshShape.h>
#include <bulletSphericalConstraint.h>
#include <bulletClosestHitRayResult.h>
#include <bulletRigidBodyNode.h>
#include <mouseWatcher.h>

//Raycaster
class Raycaster: public Singleton<Raycaster>
{
public:
	Raycaster(PandaFramework* app, WindowFramework* window, SMARTPTR(BulletWorld)world,
	const std::string& pickKeyOn, const std::string& pickKeyOff);
	virtual ~Raycaster();

	void setHitCallback(void (*callback)(Raycaster*, void*), void* data)
	{
		mCallback = callback;
		mData = data;
	}
	std::string getHitNode(){return mHitNode;}
	LPoint3f getHitPos(){return mHitPos;}
	LPoint3f getFromPos(){return mFromPos;}
	LPoint3f getToPos(){return mToPos;}
	LVector3f getHitNormal(){return mHitNormal;}
	float getHitFraction(){return mHitFraction;}

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();

private:
	///Panda framework.
	PandaFramework* mApp;
	///Window framework.
	WindowFramework* mWindow;
	///Render, camera node paths.
	NodePath mRender, mCamera;
	///Camera lens reference.
	SMARTPTR(Lens) mCamLens;
	///Bullet world.
	SMARTPTR(BulletWorld) mWorld;
	///Hit results.
	std::string mHitNode;
	LPoint3f mHitPos, mFromPos, mToPos;
	LVector3f mHitNormal;
	float mHitFraction;
	///Hit callback.
	void (*mCallback)(Raycaster*, void*);
	void* mData;
	/**
	 * \name Hit body event callback data.
	 */
	///@{
	SMARTPTR(EventCallbackInterface<Raycaster>::EventCallbackData) mPickBodyData;
	void hitBody(const Event* event);
	std::string mPickKeyOn, mPickKeyOff;
	///@}
	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* RAYCASTER_H_ */
