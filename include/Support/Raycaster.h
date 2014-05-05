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
 * \author consultit
 */

#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include "Utilities/Tools.h"
#include <pandaFramework.h>
#include "PhysicsComponents/BulletLocal/bulletWorld.h"
#include <mouseWatcher.h>
#include "ObjectModel/Object.h"

namespace ely
{

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
	 * Callbacks are called only if there was an hit.\n
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
	 * \brief Set a callback for ray casting if possible.
	 *
	 * Callbacks are called only if there was an hit.\n
	 *
	 * @param callback The callback.
	 * @param data The data associated with the callback.
	 * @param hitKey The key for ray casting.
	 * @param bitMask The Bullet collision mask of the ray.
	 * @return True if callback has been set, false otherwise.
	 */
	bool setHitCallback(void (*callback)(Raycaster*, void*), void* data,
	const std::string& hitKey, BitMask32 bitMask = BitMask32::all_on());

	/**
	 * \brief Performs a ray cast under the mouse pointer.
	 *
	 * \note when called in multithreading, the Raycaster's mutex should
	 * be locked if detailed hit results are needed, this to avoid intervening
	 * calls between ray casting and hit results reading.\n
	 *
	 * @param bitMask The Bullet collision mask of the ray.
	 * @return The hit Object, NULL otherwise.
	 */
	SMARTPTR(Object) rayCast(const BitMask32& bitMask);

	/**
	 * \name Functions returning the last ray cast hit results.
	 */
	///@{
	const PandaNode* getHitNode();
	SMARTPTR(Object) getHitObject();
	LPoint3f getHitPos();
	LPoint3f getFromPos();
	LPoint3f getToPos();
	LVector3f getHitNormal();
	float getHitFraction();
	///@}

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

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
	PandaNode* mHitNode;
	SMARTPTR(Object) mHitObject;
	LPoint3f mHitPos, mFromPos, mToPos;
	LVector3f mHitNormal;
	float mHitFraction;
	///The max number of callbacks.
	int m_N;
	///Set callback helper.
	void doSetCallback(int index, void (*callback)(Raycaster*, void*),
			void* data, const std::string& hitKey, BitMask32 bitMask);
	///Hit callbacks.
	typedef void (*RaycasterCallback)(Raycaster*, void*);
	std::vector<RaycasterCallback> mCallback;
	std::vector<void*> mData;
	std::vector<BitMask32> mBitMask;
	/**
	 * \name Hit body event callback data.
	 */
	///@{
	std::vector<SMARTPTR(EventCallbackInterface<Raycaster>::EventCallbackData)> mHitBodyData;
	void hitBodyCallback(const Event* event);
	std::vector<std::string> mHitKey;
	///@}
	///Ray cast helper.
	void doRayCast(const BitMask32& bitMask);

#ifdef ELY_THREAD
	///The mutex associated with this ray caster.
	ReMutex mMutex;
#endif
};

///inline definitions

inline SMARTPTR(Object) Raycaster::getHitObject()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHitObject;
}

inline const PandaNode* Raycaster::getHitNode()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHitNode;
}

inline LPoint3f Raycaster::getHitPos()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHitPos;
}

inline LPoint3f Raycaster::getFromPos()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFromPos;
}

inline LPoint3f Raycaster::getToPos()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mToPos;
}

inline LVector3f Raycaster::getHitNormal()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHitNormal;
}

inline float Raycaster::getHitFraction()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mHitFraction;
}

#ifdef ELY_THREAD
inline ReMutex& Raycaster::getMutex()
{
	return mMutex;
}
#endif

} // namespace ely

#endif /* RAYCASTER_H_ */
