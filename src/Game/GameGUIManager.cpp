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
 * \file /Ely/src/Game/GameGUIManager.cpp
 *
 * \date 04/apr/2015 (17:24:46)
 * \author consultit
 */

#include "Game/GameGUIManager.h"
#include <rocketRegion.h>
#include <Rocket/Debugger.h>
#include "Game/GameManager.h"

namespace ely
{

GameGUIManager::GameGUIManager()
{
	//gui (libRocket) variables initialization
	gGuiRocketContext = NULL;
	gGuiMainMenu = NULL;
	gGuiExitMenu = NULL;
	gGuiMainMenuPath.clear();
	gGuiExitMenuPath.clear();
	//registered by subsystems to add their element (tags) to main menu
	gGuiAddElementsFunctions.clear();
	//registered by subsystems to handle their events
	gGuiEventHandlers.clear();
	//registered by some subsystems that need to preset themselves before main/exit menus are shown
	gGuiPresetFunctions.clear();
	//registered by some subsystems that need to commit their changes after main/exit menus are closed
	gGuiCommitFunctions.clear();
	gGuiFontPaths.clear();
	gGuiMainPresetsCommits = false;
	mEventListenerInstancer = NULL;
}

GameGUIManager::~GameGUIManager()
{
}

void GameGUIManager::guiSetup()
{
	RETURN_ON_COND(gGuiMainMenuPath.empty() or gGuiExitMenuPath.empty(),)

	///libRocket initialization
	//load fonts
	std::vector<std::string>::const_iterator iterPath;
	for (iterPath = gGuiFontPaths.begin(); iterPath != gGuiFontPaths.end();
			++iterPath)
	{
		Rocket::Core::FontDatabase::LoadFontFace(
				Rocket::Core::String(iterPath->c_str()));
	}

	//initialize one region for all documents
	SMARTPTR(RocketRegion)region = RocketRegion::make("elyRocket",
			GameManager::GetSingletonPtr()->windowFramework()->get_graphics_window());
	region->set_active(true);

	//set input handler
	SMARTPTR(RocketInputHandler)inputHandler = new RocketInputHandler();
	GameManager::GetSingletonPtr()->windowFramework()->get_mouse().attach_new_node(
			inputHandler);
	region->set_input_handler(inputHandler);

	//set global context variable
	gGuiRocketContext = region->get_context();

	//initialize controls
	Rocket::Controls::Initialise();

	//register the main EventListenerInstancer: used by all the application documents
	mEventListenerInstancer = new MainEventListenerInstancer(
			GameManager::GetSingletonPtr()->pandaFramework());
	Rocket::Core::Factory::RegisterEventListenerInstancer(
			mEventListenerInstancer);
	mEventListenerInstancer->RemoveReference();

#ifdef ELY_DEBUG
	Rocket::Debugger::Initialise(gGuiRocketContext);
	Rocket::Debugger::SetVisible(true);
#endif
}

void GameGUIManager::showMainMenu()
{
	RETURN_ON_COND(gGuiMainMenuPath.empty() or gGuiExitMenuPath.empty(),)
	//return if already shown or we are asking to exit
	RETURN_ON_COND(
			gGuiRocketContext->GetDocument("main_menu")
					or gGuiRocketContext->GetDocument("exit_menu"),)

	//call all registered preset functions
	std::vector<void (*)()>::iterator iter;
	for (iter = gGuiPresetFunctions.begin();
			iter != gGuiPresetFunctions.end(); ++iter)
	{
		(*iter)();
	}
	//presets & commits are executing by main menu
	gGuiMainPresetsCommits = true;

	// Load and show the main document.
	gGuiMainMenu = gGuiRocketContext->LoadDocument(gGuiMainMenuPath.c_str());
	if (gGuiMainMenu != NULL)
	{
		gGuiMainMenu->GetElementById("title")->SetInnerRML(
				gGuiMainMenu->GetTitle());
		//call all registered add elements functions
		std::vector<void (*)(Rocket::Core::ElementDocument *)>::iterator iter;
		for (iter = gGuiAddElementsFunctions.begin();
				iter != gGuiAddElementsFunctions.end(); ++iter)
		{
			(*iter)(gGuiMainMenu);
		}
		gGuiMainMenu->Show();
		gGuiMainMenu->RemoveReference();
	}
}

void GameGUIManager::showExitMenu()
{
	RETURN_ON_COND(gGuiMainMenuPath.empty() or gGuiExitMenuPath.empty(),)
	//return if we are already asking to exit
	RETURN_ON_COND(gGuiRocketContext->GetDocument("exit_menu"),)

	//if presets & commits are not being executed by main menu
	if (not gGuiMainPresetsCommits)
	{
		//call all registered preset functions
		std::vector<void (*)()>::iterator iter;
		for (iter = gGuiPresetFunctions.begin();
				iter != gGuiPresetFunctions.end(); ++iter)
		{
			(*iter)();
		}
	}

	// Load and show the exit menu modal document.
	gGuiExitMenu = gGuiRocketContext->LoadDocument(
			gGuiExitMenuPath.c_str());
	if (gGuiExitMenu != NULL)
	{
		gGuiExitMenu->GetElementById("title")->SetInnerRML(
				gGuiExitMenu->GetTitle());
		//
		gGuiExitMenu->Show(Rocket::Core::ElementDocument::MODAL);
		gGuiExitMenu->RemoveReference();
	}
}

///MainEventListener stuff
MainEventListener::MainEventListener(const Rocket::Core::String& value) :
		EventListener(value)
{
}

MainEventListener::~MainEventListener()
{
}

void MainEventListener::ProcessEvent(Rocket::Core::Event& event)
{
#ifdef ELY_DEBUG
	Rocket::Core::Event::EventPhase phase = event.GetPhase();
	Rocket::Core::String phaseStr;
	switch (phase)
	{
	case Rocket::Core::Event::PHASE_BUBBLE:
		phaseStr = "PHASE_BUBBLE";
		break;
	case Rocket::Core::Event::PHASE_CAPTURE:
		phaseStr = "PHASE_CAPTURE";
		break;
	case Rocket::Core::Event::PHASE_TARGET:
		phaseStr = "PHASE_TARGET";
		break;
	default:
		phaseStr = "PHASE_UNKNOWN";
		break;
	}
	PRINT_DEBUG("Event type: " << event.GetType().CString());
	PRINT_DEBUG("Event value: " << mValue.CString());
	PRINT_DEBUG("Event phase: " << phaseStr.CString());
	PRINT_DEBUG(
			"Event target element tag: " << event.GetTargetElement()->GetTagName().CString());
	PRINT_DEBUG(
			"Event current element tag: " << event.GetTargetElement()->GetTagName().CString());
	int pos = 0;
	Rocket::Core::String key, valueAsStr;
	Rocket::Core::Variant* value;
	while (event.GetParameters()->Iterate(pos, key, value))
	{
		valueAsStr = value->Get<Rocket::Core::String>();
		PRINT_DEBUG(
				"Event parameter: pos = " << pos << " key = \"" << key.CString() << "\"" << " value = " << valueAsStr.CString());
	}
	PRINT_DEBUG("");
#endif

	if (mValue == "MAIN::ENTER_GAME")
	{
		//call all registered commit functions
		std::vector<void (*)()>::iterator iter;
		for (iter =
				GameGUIManager::GetSingletonPtr()->gGuiCommitFunctions.begin();
				iter
						!= GameGUIManager::GetSingletonPtr()->gGuiCommitFunctions.end();
				++iter)
		{
			(*iter)();
		}
		//presets & commits are not being executed by main menu any more
		GameGUIManager::GetSingletonPtr()->gGuiMainPresetsCommits = false;
		//close (i.e. unload) the main document and set as closed..
		GameGUIManager::GetSingletonPtr()->gGuiMainMenu->Close();
	}
	else if (mValue == "MAIN::EXIT_GAME")
	{
		GameGUIManager::GetSingletonPtr()->showExitMenu();
	}
	else if (mValue == "EXIT::SUBMIT_EXIT")
	{
		Rocket::Core::String paramValue;
		//check if ok or cancel
		paramValue = event.GetParameter<Rocket::Core::String>("submit",
				"cancel");
		//close (i.e. unload) the exit menu and set as closed..
		GameGUIManager::GetSingletonPtr()->gGuiExitMenu->Close();
		if (paramValue == "ok")
		{
			//user wants to exit: unload all documents
			GameGUIManager::GetSingletonPtr()->gGuiRocketContext->UnloadAllDocuments();
			//set PandaFramework exit flag
			GameManager::GetSingletonPtr()->pandaFramework()->set_exit_flag();
		}
		//if presets & commits are not being executed by main menu
		if (not GameGUIManager::GetSingletonPtr()->gGuiMainPresetsCommits)
		{
			//call all registered commit functions
			std::vector<void (*)()>::iterator iter;
			for (iter =
					GameGUIManager::GetSingletonPtr()->gGuiCommitFunctions.begin();
					iter
							!= GameGUIManager::GetSingletonPtr()->gGuiCommitFunctions.end();
					++iter)
			{
				(*iter)();
			}
		}
	}
	else
	{
		//check if it is a registered event name
		if (GameGUIManager::GetSingletonPtr()->gGuiEventHandlers.find(mValue)
				!= GameGUIManager::GetSingletonPtr()->gGuiEventHandlers.end())
		{
			//call the registered event handler
			GameGUIManager::GetSingletonPtr()->gGuiEventHandlers[mValue](mValue,
					event);
		}
	}
}

} // namespace ely
