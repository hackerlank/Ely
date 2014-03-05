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
 * \file /Ely/ely/initializations/defs/ely.cpp
 *
 * \date 11/gen/2014 (11:22:20)
 * \author consultit
 */

#include "../common_configs.h"
#include "Game/GamePhysicsManager.h"
#include "Support/Raycaster.h"
#include "Ely.h"
#include <rocketRegion.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
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

//locals
namespace
{
const int CALLBACKSNUM = 5;
std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");
void LoadFonts(const char* directory);
void showMainMenu(const Event* e, void* data);

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
}  // namespace
//globals
Rocket::Core::Context *gRocketContext;
Rocket::Core::ElementDocument *gMainMenu = NULL;

void elyPreObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//create the global ray caster
	new Raycaster(pandaFramework, windowFramework, GamePhysicsManager::GetSingleton().bulletWorld(), CALLBACKSNUM);

	//initialize libRocket: one region for the main window
	LoadFonts((baseDir + "data/misc/").c_str());

	SMARTPTR(RocketRegion)region = RocketRegion::make("elyRocket", windowFramework->get_graphics_window());
	region->set_active(true);

	SMARTPTR(RocketInputHandler)inputHandler = new RocketInputHandler();
	windowFramework->get_mouse().attach_new_node(inputHandler);
	region->set_input_handler(inputHandler);

	gRocketContext = region->get_context();

	Rocket::Controls::Initialise();

	//show rocket-main event handler
	EventHandler::get_global_event_handler()->add_hook("m", showMainMenu,
	reinterpret_cast<void*>(&pandaFramework));

#ifdef ELY_DEBUG
	Rocket::Debugger::Initialise(gRocketContext);
	Rocket::Debugger::SetVisible(true);
#endif
}

void elyPostObjects_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
}

namespace
{
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

void showMainMenu(const Event* e, void* data)
{
	PandaFramework* framework = reinterpret_cast<PandaFramework*>(data);
	//register EventListenerInstancer
	eventListenerInstancer = new MainEventListenerInstancer(framework);
	Rocket::Core::Factory::RegisterEventListenerInstancer(
			eventListenerInstancer);
	eventListenerInstancer->RemoveReference();

	// Load and show the main document.
	gMainMenu = gRocketContext->LoadDocument(
			(baseDir + "data/misc/rocket-main.rml").c_str());
	if (gMainMenu != NULL)
	{
		gMainMenu->GetElementById("title")->SetInnerRML(gMainMenu->GetTitle());
		gMainMenu->Show();
		gMainMenu->RemoveReference();
	}
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

	if (mValue == "main::body::load_logo")
	{

	}
	else if (mValue == "main::button::start_game")
	{
		//close (i.e. unload) the main document.
		gMainMenu->Close();
	}
	else if (mValue == "main::button::options")
	{
//		gMainMenu->Hide();
//		// Load and show the options document.
//		optionMenu = gRocketContext->LoadDocument(
//				(baseDir + "data/misc/rocket-options.rml").c_str());
//		if (optionMenu != NULL)
//		{
//			optionMenu->GetElementById("title")->SetInnerRML(
//					optionMenu->GetTitle());
//			///update controls
//			//graphics
//			switch (graphicsQuality)
//			{
//			case 0:
//				optionMenu->GetElementById("default_graphics")->SetAttribute(
//						"checked", true);
//				break;
//			case 1:
//				optionMenu->GetElementById("good_graphics")->SetAttribute(
//						"checked", true);
//				break;
//			case 2:
//				optionMenu->GetElementById("bad_graphics")->SetAttribute(
//						"checked", true);
//				break;
//			default:
//				break;
//			}
//			//audio
//			Rocket::Core::Element* audioElem;
//			audioElem = optionMenu->GetElementById("reverb_audio");
//			audioReverb ?
//					audioElem->SetAttribute("checked", true) :
//					audioElem->RemoveAttribute("checked");
//			audioElem = optionMenu->GetElementById("3d_audio");
//			audio3d ?
//					audioElem->SetAttribute("checked", true) :
//					audioElem->RemoveAttribute("checked");
//			//
//			optionMenu->Show();
//			optionMenu->RemoveReference();
//		}
	}
	else if (mValue == "main::button::exit")
	{
		//close (i.e. unload) the main document.
		gMainMenu->Close();
		//set PandaFramework exit flag
		mFramework->set_exit_flag();
	}
//	else if (mValue == "options::body::load_logo")
//	{
//
//	}
//	else if (mValue == "options::form::submit_options")
//	{
//		Rocket::Core::String paramValue;
//		//check if ok or cancel
//		paramValue = event.GetParameter<Rocket::Core::String>("submit",
//				"cancel");
//		if (paramValue == "ok")
//		{
//			//get controls values
//			//graphics
//			paramValue = event.GetParameter<Rocket::Core::String>("graphics",
//					"default");
//			if (paramValue == "good")
//			{
//				graphicsQuality = 1;
//			}
//			else if (paramValue == "bad")
//			{
//				graphicsQuality = 2;
//			}
//			else
//			{
//				//default
//				graphicsQuality = 0;
//			}
//			//audio
//			paramValue = event.GetParameter<Rocket::Core::String>("reverb",
//					"NONE");
//			paramValue == "true" ? audioReverb = true : audioReverb = false;
//			paramValue = event.GetParameter<Rocket::Core::String>("3d", "NONE");
//			paramValue == "true" ? audio3d = true : audio3d = false;
//		}
//		//close (i.e. unload) the options document.
//		optionMenu->Close();
//		//return to main document.
//		gMainMenu->Show();
//	}
//	else if (mValue == "options::input::bad_graphics")
//	{
//		// This event is sent from the 'onchange' of the bad graphics
//		//radio button. It shows or hides the bad graphics warning message.
//		Rocket::Core::ElementDocument* options_body =
//				event.GetTargetElement()->GetOwnerDocument();
//		if (options_body == NULL)
//			return;
//
//		Rocket::Core::Element* bad_warning = options_body->GetElementById(
//				"bad_warning");
//		if (bad_warning)
//		{
//			// The 'value' parameter of an onchange event is set to
//			//the value the control would send if it was submitted;
//			//so, the empty string if it is clear or to the 'value'
//			//attribute of the control if it is set.
//			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
//			{
//				bad_warning->SetProperty("display", "none");
//			}
//			else
//			{
//				bad_warning->SetProperty("display", "block");
//			}
//		}
//	}
	else
	{
	}
}

}

void elyGameInit()
{
}

void elyGameEnd()
{
	//delete the global ray caster
	delete Raycaster::GetSingletonPtr();
}
