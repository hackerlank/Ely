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
 * \file /Ely/include/Game.h
 *
 * \date 07/mag/2012 (18:07:26)
 * \author marco
 */

#ifndef GAME_H_
#define GAME_H_

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <auto_bind.h>

#include <cardMaker.h>
#include <ambientLight.h>
#include <directionalLight.h>
#include <pointLight.h>
#include <spotlight.h>

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <utility>

class FuncInterval;

/**
 * \brief Application
 */
class Game: public PandaFramework
{
public:
	Game(int argc, char* argv[]);
	virtual ~Game();

	/**
	 * Put here your own code, such as the loading of your models
	 */
	virtual void setup();

	// Generic Task Function interface
	typedef AsyncTask::DoneStatus (Game::*ApplicationTaskPtr)(
			GenericAsyncTask* task);
	typedef std::pair<Game*, ApplicationTaskPtr> ApplicationTaskData;
	static AsyncTask::DoneStatus applicationTask(GenericAsyncTask* task,
			void * data);

protected:

	// Effective Tasks composed by a pair of an object and a member function
//	boost::shared_ptr<ApplicationTaskData> mUpdateTask;
//	AsyncTask::DoneStatus update(GenericAsyncTask* task = NULL);

	WindowFramework * mWindow;
	NodePath mRender;
	NodePath mCamera;
	PT(ClockObject) mGlobalClock;

	//
	NodePath mPanda;
	AnimControlCollection mPandaAnims;
};

#endif /* GAME_H_ */
