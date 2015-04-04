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
 * \file /Ely/include/Game/GameGUIManager.h
 *
 * \date 04/apr/2015 (17:24:26)
 * \author consultit
 */

#ifndef GAMEGUIMANAGER_H_
#define GAMEGUIMANAGER_H_

#include "Utilities/Tools.h"
#include <pandaFramework.h>
#include <windowFramework.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>

namespace ely
{

/**
 * \brief Abstract class interface for GUI (libRocket) event listeners.
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
	EventListener(const Rocket::Core::String& value) :
			Rocket::Core::ReferenceCountable(0), mValue(value)
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
	EventListenerInstancer(PandaFramework* framework)
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

};

/**
 * \brief Gui (libRocket) event framework class.
 */
class MainEventListener: public EventListener
{
public:
	MainEventListener(const Rocket::Core::String& value);

	virtual ~MainEventListener();

	virtual void ProcessEvent(Rocket::Core::Event& event);

};

class MainEventListenerInstancer: public EventListenerInstancer
{
public:
	MainEventListenerInstancer(PandaFramework* framework) :
			EventListenerInstancer(framework)
	{
	}
	virtual ~MainEventListenerInstancer()
	{
	}
	virtual Rocket::Core::EventListener* InstanceEventListener(
			const Rocket::Core::String& value, Rocket::Core::Element* element)
	{
		return new MainEventListener(value);
	}
};

/**
 * \brief GameGUIManager.
 *
 * To use this libRocket based framework, these documents must be
 * supplied:
 * - "main_menu" that responds, at least, to "MAIN::ENTER_GAME",
 * "MAIN::EXIT_GAME" events
 * - "exit_menu" that responds, at least,  to "EXIT::SUBMIT_EXIT" events
 *
 * Currently these parameters (along with font paths) are passed through a
 * GameConfig component.
 *
 */
class GameGUIManager: public Singleton<GameGUIManager>
{
public:
	/**
	 * \brief Constructor. Initializes the GUI (libRocket) environment.
	 */
	GameGUIManager();

	/**
	 * \brief Destructor
	 */
	virtual ~GameGUIManager();

	/**
	 * \brief Put here game setup code.
	 */
	virtual void guiSetup();

	/**
	 * \brief GUI (libRocket) global related variables and functions.
	 */
	///@{
	Rocket::Core::Context *gGuiRocketContext;
	Rocket::Core::ElementDocument *gGuiMainMenu;
	Rocket::Core::ElementDocument *gGuiExitMenu;
	std::string gGuiMainMenuPath;
	std::string gGuiExitMenuPath;
	//registered by subsystems to add their element (tags) to main menu
	std::vector<void (*)(Rocket::Core::ElementDocument *)> gGuiAddElementsFunctions;
	//registered by subsystems to handle their events
	std::map<Rocket::Core::String,
			void (*)(const Rocket::Core::String&, Rocket::Core::Event&)> gGuiEventHandlers;
	//registered by some subsystems that need to preset themselves before main/exit menus are shown
	std::vector<void (*)()> gGuiPresetFunctions;
	//registered by some subsystems that need to commit their changes after main/exit menus are closed
	std::vector<void (*)()> gGuiCommitFunctions;
	std::vector<std::string> gGuiFontPaths;
	bool gGuiMainPresetsCommits;
	//functions
	void showMainMenu();
	void showExitMenu();
	///@}

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:

	/**
	 * \brief Gui (libRocket) related variables and functions.
	 */
	///@{
	MainEventListenerInstancer* mEventListenerInstancer;
	///@}

#ifdef ELY_THREAD
	///The mutex associated with this manager.
	ReMutex mMutex;
#endif
};

///inline definitions

#ifdef ELY_THREAD
inline ReMutex& GameGUIManager::getMutex()
{
	return mMutex;
}
#endif

} // namespace ely

#endif /* GAMEGUIMANAGER_H_ */
