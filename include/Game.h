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

#include <cIntervalManager.h>
#include <cLerpNodePathInterval.h>
#include <cMetaInterval.h>
#include <randomizer.h>

#include <iostream>

#include "GameObject.h"
#include "Utilitiy.h"

class FuncInterval;

/**
 * \brief Game
 */
class Game: public PandaFramework
{
public:
	/**
	 * \brief Constructor. Initializes the environment.
	 * @param argc from main()
	 * @param argv from main()
	 */
	Game(int argc, char* argv[]);
	/**
	 * \brief Destructor
	 */
	virtual ~Game();

	/**
	 * \brief Put here your own custom setup game code.
	 */
	virtual void setup();

	/**
	 * \brief Generic Task Function interface
	 *
	 *  The effective Tasks are composed by a Pair of
	 *  an Game object and a member function doing the task.
	 */
	typedef AsyncTask::DoneStatus (Game::*GameTaskPtr)(GenericAsyncTask* task);
	typedef Pair<Game*, GameTaskPtr> GameTaskData;
	static AsyncTask::DoneStatus gameTask(GenericAsyncTask* task,
			void * data);

protected:

	/// 1nd task.
	PT(GameTaskData) m1stTask;
	AsyncTask::DoneStatus firstTask(GenericAsyncTask* task = NULL);
	/// 2nd task
	PT(GameTaskData) m2ndTask;
	AsyncTask::DoneStatus secondTask(GenericAsyncTask* task = NULL);

	/// Common members
	WindowFramework * mWindow;
	NodePath mRender;
	NodePath mCamera;
	PT(ClockObject) mGlobalClock;

	/// Specific members
	NodePath mPanda;
	PT(GameObject) mPandaGO;
	AnimControlCollection mPandaAnims;
};

#endif /* GAME_H_ */
