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
 * \file /Ely/elygame/initializations/defs/Game_init.cpp
 *
 * \date 11/gen/2014 (11:22:20)
 * \author consultit
 */

#include "../common_configs.h"
#include "Game/GamePhysicsManager.h"
#include "Support/Raycaster.h"
#include "Ely.h"
#include "Game_init.h"
#include "Support/LibRocketLocal/rocketRegion.h"
#include <Rocket/Debugger.h>

///ely related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION elyPreObjects_initialization;
INITIALIZATION elyPostObjects_initialization;

#ifdef __cplusplus
}
#endif

//globals
Rocket::Core::Context *gRocketContext;
Rocket::Core::ElementDocument *gRocketMainMenu;
//registered by subsystems to add their element (tags) to main menu
std::vector<void (*)(Rocket::Core::ElementDocument *)> gRocketAddElementsFunctions;
//registered by subsystems to handle their events
std::map<Rocket::Core::String,
		void (*)(const Rocket::Core::String&, Rocket::Core::Event&)> gRocketEventHandlers;
//registered by some subsystems that need to preset themselves before main/exit menus are shown
std::vector<void (*)()> gRocketPresetFunctions;
//registered by some subsystems that need to commit their changes after main/exit menus are closed
std::vector<void (*)()> gRocketCommitFunctions;
std::string rocketBaseDir(ELY_DATADIR);
//common text writing
void writeText(NodePath& textNode, const std::string& text, float scale, const LVecBase4& color,
		const LVector3f& location)
{
	textNode = NodePath(new TextNode("CommonTextNode"));
	textNode.reparent_to(
			ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(
					ObjectId("render2d"))->getNodePath());
	textNode.set_bin("fixed", 50);
	textNode.set_depth_write(false);
	textNode.set_depth_test(false);
	textNode.set_billboard_point_eye();
	DCAST(TextNode, textNode.node())->set_text(text);
	textNode.set_scale(scale);
	textNode.set_color(color);
	textNode.set_pos(location);
}

//locals
namespace
{
const int CALLBACKSNUM = 5;
Rocket::Core::ElementDocument *exitMenu;
bool mainPresetsCommits = false;

void showExitMenu(const Event* e);

class MainEventListener: public EventListener
{
public:
	MainEventListener(const Rocket::Core::String& value,
			PandaFramework* framework) :
			EventListener(value, framework)
	{
	}
	virtual ~MainEventListener()
	{
	}
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
		return new MainEventListener(value, mFramework);
	}
};
MainEventListenerInstancer* eventListenerInstancer = NULL;

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

	if (mValue == "main::body::load_logo")
	{

	}
	else if (mValue == "main::button::start_game")
	{
		//call all registered commit functions
		std::vector<void (*)()>::iterator iter;
		for (iter = gRocketCommitFunctions.begin();
				iter != gRocketCommitFunctions.end(); ++iter)
		{
			(*iter)();
		}
		//presets & commits are not being executed by main menu any more
		mainPresetsCommits = false;
		//close (i.e. unload) the main document and set as closed..
		gRocketMainMenu->Close();
	}
	else if (mValue == "main::button::exit")
	{
		showExitMenu(NULL);
	}
	else if (mValue == "exit::form::submit_exit")
	{
		Rocket::Core::String paramValue;
		//check if ok or cancel
		paramValue = event.GetParameter<Rocket::Core::String>("submit",
				"cancel");
		//close (i.e. unload) the exit menu and set as closed..
		exitMenu->Close();
		if (paramValue == "ok")
		{
			//user wants to exit: unload all documents
			gRocketContext->UnloadAllDocuments();
			//set PandaFramework exit flag
			mFramework->set_exit_flag();
		}
		//if presets & commits are not being executed by main menu
		if (not mainPresetsCommits)
		{
			//call all registered commit functions
			std::vector<void (*)()>::iterator iter;
			for (iter = gRocketCommitFunctions.begin();
					iter != gRocketCommitFunctions.end(); ++iter)
			{
				(*iter)();
			}
		}
	}
	else
	{
		//check if it is a registered event name
		if (gRocketEventHandlers.find(mValue) != gRocketEventHandlers.end())
		{
			//call the registered event handler
			gRocketEventHandlers[mValue](mValue, event);
		}
	}
}

void showMainMenu(const Event* e)
{
	//return if already shown or we are asking to exit
	RETURN_ON_COND(gRocketContext->GetDocument("main_menu") or
			gRocketContext->GetDocument("exit_menu"),)

	//call all registered preset functions
	std::vector<void (*)()>::iterator iter;
	for (iter = gRocketPresetFunctions.begin();
			iter != gRocketPresetFunctions.end(); ++iter)
	{
		(*iter)();
	}
	//presets & commits are executing by main menu
	mainPresetsCommits = true;

	// Load and show the main document.
	gRocketMainMenu = gRocketContext->LoadDocument(
			(rocketBaseDir + "misc/ely-main-menu.rml").c_str());
	if (gRocketMainMenu != NULL)
	{
		gRocketMainMenu->GetElementById("title")->SetInnerRML(
				gRocketMainMenu->GetTitle());
		//call all registered add elements functions
		std::vector<void (*)(Rocket::Core::ElementDocument *)>::iterator iter;
		for (iter = gRocketAddElementsFunctions.begin();
				iter != gRocketAddElementsFunctions.end(); ++iter)
		{
			(*iter)(gRocketMainMenu);
		}
		gRocketMainMenu->Show();
		gRocketMainMenu->RemoveReference();
	}
}

void showExitMenu(const Event* e)
{
	//return if we are already asking to exit
	RETURN_ON_COND(gRocketContext->GetDocument("exit_menu"),)

	//if presets & commits are not being executed by main menu
	if (not mainPresetsCommits)
	{
		//call all registered preset functions
		std::vector<void (*)()>::iterator iter;
		for (iter = gRocketPresetFunctions.begin();
				iter != gRocketPresetFunctions.end(); ++iter)
		{
			(*iter)();
		}
	}

	// Load and show the exit menu modal document.
	exitMenu = gRocketContext->LoadDocument(
			(rocketBaseDir + "misc/ely-exit-menu.rml").c_str());
	if (exitMenu != NULL)
	{
		exitMenu->GetElementById("title")->SetInnerRML(
				exitMenu->GetTitle());
		//
		exitMenu->Show(Rocket::Core::ElementDocument::MODAL);
		exitMenu->RemoveReference();
	}
}

// Loads libRocket fonts from the given path.
void LoadFonts(const char* directory)
{
	Rocket::Core::String font_names[4];
	font_names[0] = "Delicious-Roman.otf";
	font_names[1] = "Delicious-Italic.otf";
	font_names[2] = "Delicious-Bold.otf";
	font_names[3] = "Delicious-BoldItalic.otf";

	for (unsigned int i = 0;
			i < sizeof(font_names) / sizeof(Rocket::Core::String); i++)
	{
		Rocket::Core::FontDatabase::LoadFontFace(
				Rocket::Core::String(directory) + font_names[i]);
	}
}

}  // namespace

void elyPreObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//create the global ray caster
	new Raycaster(pandaFramework, windowFramework, GamePhysicsManager::GetSingleton().bulletWorld(), CALLBACKSNUM);

	///libRocket initialization
	//load fonts
	LoadFonts((rocketBaseDir + "misc/").c_str());

	//initialize one region for all documents
	SMARTPTR(RocketRegion)region = RocketRegion::make("elyRocket", windowFramework->get_graphics_window());
	region->set_active(true);

	//set input handler
	SMARTPTR(RocketInputHandler)inputHandler = new RocketInputHandler();
	windowFramework->get_mouse().attach_new_node(inputHandler);
	region->set_input_handler(inputHandler);

	//set global context variable
	gRocketContext = region->get_context();

	//initialize controls
	Rocket::Controls::Initialise();

	//register the main EventListenerInstancer: used by all the application documents
	eventListenerInstancer = new MainEventListenerInstancer(pandaFramework);
	Rocket::Core::Factory::RegisterEventListenerInstancer(
			eventListenerInstancer);
	eventListenerInstancer->RemoveReference();

#ifdef ELY_DEBUG
	Rocket::Debugger::Initialise(gRocketContext);
	Rocket::Debugger::SetVisible(true);
#endif
}

void elyPostObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	///libRocket
	//add show main menu event handler
	EventHandler::get_global_event_handler()->add_hook("m", &showMainMenu);
	//handle "close request" and "esc" events
	windowFramework->get_graphics_window()->set_close_request_event("close_request_event");
	EventHandler::get_global_event_handler()->add_hook("close_request_event", &showExitMenu);
	EventHandler::get_global_event_handler()->add_hook("escape", &showExitMenu);
}

void Game_initInit()
{
}

void Game_initEnd()
{
	//delete the global ray caster
	delete Raycaster::GetSingletonPtr();
}
