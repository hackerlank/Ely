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
 * \author consultit
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "Utilities/Tools.h"
#include <pandaFramework.h>
#include <windowFramework.h>
#include <clockObject.h>

namespace ely
{
class FuncInterval;

/**
 * \brief GameManager.
 *
 * It runs in the main thread.
 */
class GameManager: public PandaFramework, public Singleton<GameManager>
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
	 * \brief Put here game initialization code.
	 */
	virtual void initialize();

	/**
	 * \brief Set the Component template manager up.
	 */
	virtual void setupCompTmplMgr();

	/**
	 * \brief Create a Game World loading description from a file (xml).
	 *
	 * Objects (inside object sets)  may have a creation priority and
	 * components (inside objects) may have a initialization priority:
	 * the higher is priority the earlier is creation/initialization
	 * (default priority = 0).
	 * @param gameWorldXML The description file.
	 */
	///@{
	virtual void createGameWorld(const std::string& gameWorldXML);
	virtual void createGameWorldWithoutParamTables(const std::string& gameWorldXML);
	///@}

	/**
	 * \brief Porting of python function direct.showbase.ShowBase.enableMouse.
	 */
	void enable_mouse();
	/**
	 * \brief Porting of python function direct.showbase.ShowBase.disableMouse.
	 */
	void disable_mouse();

	/**
	 * \brief Gets/sets the PandaFramework.
	 * @return A reference to the PandaFramework.
	 */
	PandaFramework* const pandaFramework() const;

	/**
	 * \brief Gets/sets the WindowFramework.
	 * @return A reference to the WindowFramework.
	 */
	WindowFramework* const windowFramework() const;

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();

protected:

	/// Struct that can be ordered based on a priority field.
	template<typename T> class Orderable
	{
	private:
		T* mPtr;
		int mPrio;
	public:
		Orderable()
		{
		}
		Orderable(T* ptr, int prio) :
				mPtr(ptr), mPrio(prio)
		{
		}
		int getPrio() const
		{
			return mPrio;
		}
		void setPrio(int prio)
		{
			mPrio = prio;
		}
		T* getPtr() const
		{
			return mPtr;
		}
		void setPtr(T* ptr)
		{
			mPtr = ptr;
		}
		bool operator <(const Orderable& other) const
		{
			return mPrio < other.mPrio;
		}
	};

	/// Common members
	WindowFramework * mWindow;
	NodePath mRender;
	SMARTPTR(ClockObject) mGlobalClock;

	/// NodePaths for enable_mouse/disable_mouse.
	NodePath mTrackBall, mMouse2cam;

	/**
	 * \brief Put here custom game object management code.
	 */
	virtual void GamePlay();

	/// 1nd task.
	SMARTPTR(TaskInterface<GameManager>::TaskData) m1stTask;
	AsyncTask::DoneStatus firstTask(GenericAsyncTask* task);
	/// 2nd task
	SMARTPTR(TaskInterface<GameManager>::TaskData) m2ndTask;
	AsyncTask::DoneStatus secondTask(GenericAsyncTask* task);

#ifdef ELY_DEBUG
	bool mPhysicsDebugEnabled;
	SMARTPTR(EventCallbackInterface<GameManager>::EventCallbackData) mPhysicsDebugData;
	void togglePhysicsDebug(const Event* event);
#endif

	///The (reentrant) mutex associated with this manager.
	ReMutex mMutex;
};

} // namespace ely

#endif /* GAMEMANAGER_H_ */
