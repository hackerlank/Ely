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
 * \file /Ely/include/Support/Raycaster.h
 *
 * \date 28/apr/2013 (12:50:35)
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

/**
 * \brief A singleton class for ray casting.
 *
 * It registers up to N ray casting callbacks, whose signatures are:
 *\code
 * 	void CALLBACK(Raycaster*, void*);
 * \endcode
 *
 * \note Bullet based.
 */
class Raycaster: public Singleton<Raycaster>
{
public:
	/**
	 * \brief The constructor.
	 *
	 * \note N is the max number of callbacks (default to 3 for LMR-mouse clicks).
	 */
	Raycaster(PandaFramework* app, WindowFramework* window,
			SMARTPTR(BulletWorld)world, int N = 3);
	virtual ~Raycaster();

	/**
	 * \brief Set the i-th callback for ray casting.
	 *
	 * @param index The callback index.
	 * @param callback The i-th callback.
	 * @param data The data associated with the i-th callback.
	 * @param hitKey The key for ray casting.
	 * @param bitMask The Bullet collision mask of the ray.
	 */
	void setHitCallback(int index, void (*callback)(Raycaster*, void*), void* data,
			const std::string& hitKey, BitMask32 bitMask = BitMask32::all_on());

	/**
	 * \name Functions returning the last ray cast hit results.
	 */
	///@{
	std::string getHitNode();
	LPoint3f getHitPos();
	LPoint3f getFromPos();
	LPoint3f getToPos();
	LVector3f getHitNormal();
	float getHitFraction();
	///@}

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
	///The max number of callbacks.
	int m_N;
	///Hit callbacks.
	typedef void (*RaycasterCallback)(Raycaster*, void*);
	std::vector<RaycasterCallback> mCallback;
	std::vector<void*>  mData;
	std::vector<BitMask32> mBitMask;
	/**
	 * \name Hit body event callback data.
	 */
	///@{
	std::vector<SMARTPTR(EventCallbackInterface<Raycaster>::EventCallbackData)> mHitBodyData;
	void hitBody(const Event* event);
	std::vector<std::string> mHitKey;
	///@}
	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* RAYCASTER_H_ */
