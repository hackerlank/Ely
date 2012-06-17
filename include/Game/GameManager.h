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
 * \file /Ely/include/Game/GameManager.h
 *
 * \date 07/mag/2012 (18:07:26)
 * \author marco
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include <pandaFramework.h>
#include <windowFramework.h>
#include <nodePath.h>
#include <clockObject.h>
#include <lmatrix.h>
#include <asyncTask.h>
#include <pointerTo.h>
#include <event.h>
#include <trackball.h>
#include <transform2sg.h>
#include <iostream>
#include <string>
#include <map>
#include <cstdio>
#include "tinyxml2.h"

#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"
#include "Game/ComponentTemplateManager.h"
#include "Game/ObjectTemplateManager.h"

class FuncInterval;

/**
 * \brief GameManager
 */
class GameManager: public PandaFramework
{
public:
	/**
	 * \brief Constructor. Initializes the environment.
	 * @param argc from main()
	 * @param argv from main()
	 */
	GameManager(int argc, char* argv[]);

	/**
	 * \brief Destructor
	 */
	virtual ~GameManager();

	/**
	 * \brief Put here your own custom setup game code.
	 */
	virtual void setup();

	/**
	 * \brief Set the Component template manager up.
	 */
	virtual void setupCompTmplMgr();

	/**
	 * \brief Create a Game World loading description from a file (xml).
	 *
	 * @param gameWorldXML The description file.
	 */
	virtual void createGameWorld(const std::string& gameWorldXML);

	/**
	 * \brief Porting of python function direct.showbase.ShowBase.enableMouse.
	 */
	void enable_mouse();
	/**
	 * \brief Porting of python function direct.showbase.ShowBase.disableMouse.
	 */
	void disable_mouse();

protected:

	/// Objects in the game.
	typedef std::map<ObjectId, PT(Object)> ObjectTable;
	ObjectTable mObjects;

	/// Common members
	WindowFramework * mWindow;
	NodePath mRender;
	NodePath mCamera;
	PT(ClockObject) mGlobalClock;

	/// NodePaths for enable_mouse/disable_mouse.
	NodePath mTrackBall, mMouse2cam;

	/// 1nd task.
	PT(TaskInterface<GameManager>::TaskData) m1stTask;
	AsyncTask::DoneStatus firstTask(GenericAsyncTask* task);
	/// 2nd task
	PT(TaskInterface<GameManager>::TaskData) m2ndTask;
	AsyncTask::DoneStatus secondTask(GenericAsyncTask* task);

	static void toggleActor1Control(const Event* event, void* data);

};

#endif /* GAMEMANAGER_H_ */
