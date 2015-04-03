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
#include <Rocket/Core.h>
#include <Rocket/Controls.h>

namespace ely
{
class FuncInterval;

//libRocket stuff
/**
 * \brief Abstract class interface for libRocket event listeners.
 *
 * A concrete derived class must implement:
 * \code
 * virtual void ProcessEvent(Rocket::Core::Event& event);
 * \endcode
 */
class EventListener: public Rocket::Core::EventListener,
		Rocket::Core::ReferenceCountable
{
public:
	EventListener(const Rocket::Core::String& value, PandaFramework* framework) :
			Rocket::Core::ReferenceCountable(0), mValue(value), mFramework(
					framework)
	{
		PRINT_DEBUG(
				"Creating EventListener: " << mValue.CString() << " " << this);
	}
	virtual ~EventListener()
	{
		PRINT_DEBUG(
				"Destroying EventListener: " << mValue.CString() << " " << this);
	}

	virtual void OnAttach(Rocket::Core::Element* ROCKET_UNUSED(element))
	{
		PRINT_DEBUG(
				"Attaching EventListener: " << mValue.CString() << " " << this);
		AddReference();
	}

	virtual void OnDetach(Rocket::Core::Element* ROCKET_UNUSED(element))
	{
		PRINT_DEBUG(
				"Detaching EventListener: " << mValue.CString() << " " << this);
		RemoveReference();
	}

protected:
	virtual void OnReferenceDeactivate()
	{
		delete this;
	}

	Rocket::Core::String mValue;
	PandaFramework* mFramework;
};

/**
 * \brief Abstract class interface for libRocket event listener instancers.
 *
 * A concrete derived class must implement:
 * \code
 * virtual Rocket::Core::EventListener* InstanceEventListener(
 const Rocket::Core::String& value, Rocket::Core::Element* element);
 * \endcode
 * This interface is designed so that method would create a ely::EventListener
 * derived class.\n
 */
class EventListenerInstancer: public Rocket::Core::EventListenerInstancer
{
public:
	EventListenerInstancer(PandaFramework* framework) :
			mFramework(framework)
	{
		PRINT_DEBUG("Creating EventListenerInstancer " << " " << this);
	}
	virtual ~EventListenerInstancer()
	{
		PRINT_DEBUG("Destroying EventListenerInstancer " << " " << this);
	}

	virtual void Release()
	{
		delete this;
	}

protected:
	PandaFramework* mFramework;
};


/**
 * \brief GameManager.
 *
 * It runs in the main thread.
 * It manages Rocket initialization too.
 *
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
	 * \brief Put here game setup code.
	 */
	virtual void gameSetup();

	/**
	 * \brief Put here game cleanup code.
	 */
	virtual void gameCleanup();

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
	 * Objects' initializations are performed "after" the entire game
	 * world has been created and in particular hierarchies between
	 * all objects have been established.
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
	 * \name Gets/sets game data infos.
	 */
	///@{
	enum GameDataInfo
	{
		DATADIR,       		//!< DATADIR data path
		CONFIGFILE,    		//!< CONFIGFILE xml configuration file path
		CALLBACKS,     		//!< CALLBACKS_LA library path
		TRANSITIONS,   		//!< TRANSITIONS_LA library path
		INITIALIZATIONS,	//!< INITIALIZATIONS_LA library path
		INSTANCEUPDATES		//!< INSTANCEUPDATES_LA library path
	};
	void setDataInfo(GameDataInfo info, const std::string& value);
	std::string getDataInfo(GameDataInfo info);
	///@}

	/**
	 * \brief libRocket globals.
	 */
	///@{
	static Rocket::Core::Context *gRocketContext;
	static Rocket::Core::ElementDocument *gRocketMainMenu;
	//registered by subsystems to add their element (tags) to main menu
	static std::vector<void (*)(Rocket::Core::ElementDocument *)> gRocketAddElementsFunctions;
	//registered by subsystems to handle their events
	static std::map<Rocket::Core::String,
			void (*)(const Rocket::Core::String&, Rocket::Core::Event&)> gRocketEventHandlers;
	//registered by some subsystems that need to preset themselves before main/exit menus are shown
	static std::vector<void (*)()> gRocketPresetFunctions;
	//registered by some subsystems that need to commit their changes after main/exit menus are closed
	static std::vector<void (*)()> gRocketCommitFunctions;
	///@}

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:

	/// Struct that can be ordered based on a priority field.
	template<typename T> class Orderable
	{
	private:
		T* mPtr;
		int mPrio;
	public:
		Orderable() :
				mPtr(NULL), mPrio(int())
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

	///Game data info DB.
	std::map<GameDataInfo, std::string> mInfoDB;

#ifdef ELY_DEBUG
	bool mPhysicsDebugEnabled;
	SMARTPTR(EventCallbackInterface<GameManager>::EventCallbackData) mPhysicsDebugData;
	void togglePhysicsDebug(const Event* event);
#endif

#ifdef ELY_THREAD
	///The mutex associated with this manager.
	ReMutex mMutex;
#endif
};

///inline definitions

inline void GameManager::setDataInfo(GameDataInfo info, const std::string& value)
{
	mInfoDB[info] = value;
}

inline std::string GameManager::getDataInfo(GameDataInfo info)
{
	return mInfoDB[info];
}

#ifdef ELY_THREAD
inline ReMutex& GameManager::getMutex()
{
	return mMutex;
}
#endif

} // namespace ely

#endif /* GAMEMANAGER_H_ */
