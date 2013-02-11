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
 * \author marco
 */

#ifndef PICKER_H_
#define PICKER_H_

#include "Utilities/Tools.h"

#include <string>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <nodePath.h>
#include <graphicsWindow.h>
#include <mouseWatcher.h>
#include <lpoint3.h>
#include <lpoint2.h>
#include <lvector3.h>
#include <camera.h>
#include <lens.h>
#include <bitMask.h>
#include <bulletWorld.h>
#include <bulletRigidBodyNode.h>
#include <bulletSphericalConstraint.h>
#include <bulletClosestHitRayResult.h>
#include "Game/GamePhysicsManager.h"
#include "ObjectModel/Component.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "ObjectModel/ObjectTemplate.h"
#include "ObjectModel/Object.h"
#include "SceneComponents/NodePathWrapper.h"
#include "PhysicsComponents/RigidBody.h"

/**
 * \brief A class for picking (physics) objects.
 *
 * \note Bullet based.
 */
class Picker: public Singleton<Picker>
{
public:
	Picker(PandaFramework* app, WindowFramework* window,
			const std::string& pickKeyOn, const std::string& pickKeyOff);
	virtual ~Picker();

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
	///Picking logic data.
	SMARTPTR(BulletSphericalConstraint) mCsPick;
	LPoint3f mPivotPos;
	SMARTPTR(RigidBody) mPickingBody;
	SMARTPTR(BulletRigidBodyNode) mPickingBodyNode;
	ObjectType mPickingBodyType;
	ObjectId mPickingBodyId;
	bool mPickingBodyAttached;

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

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

#endif /* PICKER_H_ */
