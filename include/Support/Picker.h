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
 * \file /Ely/include/Support/Picker.h
 *
 * \date 26/dic/2012 (10:58:27)
 * \author consultit
 */

#ifndef PICKER_H_
#define PICKER_H_

#include "Utilities/Tools.h"
#include <pandaFramework.h>
#include <lens.h>
#include "Support/BulletLocal/bulletWorld.h"
#include "Support/BulletLocal/bulletSphericalConstraint.h"
#include "Support/BulletLocal/bulletGenericConstraint.h"

namespace ely
{

/**
 * \brief A class for picking (physics) objects.
 *
 * \note Bullet based.
 */
class Picker: public Singleton<Picker>
{
public:
	Picker(PandaFramework* app, WindowFramework* window,
			const std::string& pickKeyOn, const std::string& pickKeyOff,
			bool csIspherical = true, float cfm = 0.5, float erp = 0.5);
	virtual ~Picker();

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
	SMARTPTR(Lens)mCamLens;
	///Bullet world.
	SMARTPTR(BulletWorld) mWorld;
	/**
	 * \name Picking logic data.
	 */
	///@{
	SMARTPTR(BulletConstraint) mCsPick;
	bool mCsIsSpherical;
	float mCfm, mErp;
	float mPivotCamDist;
	///@}

	/**
	 * \name Pick body event callback data.
	 */
	///@{
	SMARTPTR(EventCallbackInterface<Picker>::EventCallbackData) mPickBodyData;
	void pickBody(const Event* event);
	SMARTPTR(BulletRigidBodyNode) mPickedBody;
	std::string mPickKeyOn, mPickKeyOff;
	///@}
	/**
	 * \name Move picked body task data.
	 */
	///@{
	SMARTPTR(TaskInterface<Picker>::TaskData) mMovePickedData;
	AsyncTask::DoneStatus movePicked(GenericAsyncTask* task);
	SMARTPTR(AsyncTask) mMovePickedTask;
	///@}

#ifdef ELY_THREAD
	///The mutex associated with this picker.
	ReMutex mMutex;
#endif
};

///inline definitions

#ifdef ELY_THREAD
inline ReMutex& Picker::getMutex()
{
	return mMutex;
}
#endif

} // namespace ely

#endif /* PICKER_H_ */
