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
 * \file /Ely/training/manual_example.cpp
 *
 * \date 10/mar/2013 (15:39:11)
 * \author consultit
 */

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <rocketRegion.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include "Utilities/Tools.h"

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");
void LoadFonts(const char* directory);
Rocket::Core::Context *context;
///rocket-doc
void showMainMenu(const Event* e, void* data);

int rocket_main(int argc, char *argv[])
{
	// Load your configuration
	load_prc_file_data("", "model-path " + baseDir + "data/models");
	load_prc_file_data("", "model-path " + baseDir + "data/shaders");
	load_prc_file_data("", "model-path " + baseDir + "data/sounds");
	load_prc_file_data("", "model-path " + baseDir + "data/textures");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "lock-to-one-cpu 0");
	load_prc_file_data("", "support-threads 1");
	load_prc_file_data("", "audio-buffering-seconds 5");
	load_prc_file_data("", "audio-preload-threshold 2000000");
	load_prc_file_data("", "sync-video #t");
	//open a new window framework
	PandaFramework framework;
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	WindowFramework *window = framework.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}
	//setup camera trackball (local coordinate)
	NodePath tballnp = window->get_mouse().find("**/+Trackball");
	PT(Trackball)trackball = DCAST(Trackball, tballnp.node());
	trackball->set_pos(0, 200, 0);
	trackball->set_hpr(0, 15, 0);

	///here is room for your own code
	LoadFonts((baseDir + "data/misc/").c_str());

	PT(RocketRegion)r = RocketRegion::make("pandaRocket", window->get_graphics_window());
	r->set_active(true);

	PT(RocketInputHandler)ih = new RocketInputHandler();
	window->get_mouse().attach_new_node(ih);
	r->set_input_handler(ih);

	context = r->get_context();

	Rocket::Controls::Initialise();

#ifdef ELY_DEBUG
	Rocket::Debugger::Initialise(context);
	Rocket::Debugger::SetVisible(true);
#endif

	//show rocket-main event handler
	EventHandler::get_global_event_handler()->add_hook("m", showMainMenu,
			reinterpret_cast<void*>(&framework));

	//do the main loop, equal to run() in python
	framework.main_loop();

	//close the window framework
	framework.close_framework();
	return (0);
}

/// Loads the default fonts from the given path.
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

///
class EventListener: public Rocket::Core::EventListener,
		Rocket::Core::ReferenceCountable
{
public:
	EventListener(const Rocket::Core::String& value, PandaFramework* framework) :
			Rocket::Core::ReferenceCountable(0), mValue(value), mFramework(
					framework)
	{
		std::cout << "Creating EventListener: " << mValue.CString() << " "
				<< this << std::endl;
	}
	virtual ~EventListener()
	{
		std::cout << "Destroying EventListener: " << mValue.CString() << " "
				<< this << std::endl;
	}

	virtual void ProcessEvent(Rocket::Core::Event& event);

	virtual void OnAttach(Rocket::Core::Element* ROCKET_UNUSED(element))
	{
		std::cout << "Attaching EventListener: " << mValue.CString() << " "
				<< this << std::endl;
		AddReference();
	}

	virtual void OnDetach(Rocket::Core::Element* ROCKET_UNUSED(element))
	{
		std::cout << "Detaching EventListener: " << mValue.CString() << " "
				<< this << std::endl;
		RemoveReference();
	}

protected:
	virtual void OnReferenceDeactivate()
	{
		delete this;
	}

private:
	Rocket::Core::String mValue;
	PandaFramework* mFramework;
};

class EventListenerInstancer: public Rocket::Core::EventListenerInstancer
{
public:
	EventListenerInstancer(PandaFramework* framework) :
			mFramework(framework)
	{
		std::cout << "Creating EventListenerInstancer " << " " << this
				<< std::endl;
	}
	virtual ~EventListenerInstancer()
	{
		std::cout << "Destroying EventListenerInstancer " << " " << this
				<< std::endl;
	}

	virtual Rocket::Core::EventListener* InstanceEventListener(
			const Rocket::Core::String& value, Rocket::Core::Element* element)
	{
		return new EventListener(value, mFramework);
	}

	virtual void Release()
	{
		delete this;
	}

private:
	PandaFramework* mFramework;
};

///
Rocket::Core::ElementDocument *mainMenu = NULL, *optionMenu = NULL;
EventListenerInstancer* eventListenerInstancer = NULL;
//simulate program control variables
//graphics: 0=default,1=good,2=bad
int graphicsQuality = 0;
//audio
bool audioReverb = true, audio3d = false;

void showMainMenu(const Event* e, void* data)
{
	PandaFramework* framework = reinterpret_cast<PandaFramework*>(data);
	//register EventListenerInstancer
	eventListenerInstancer = new EventListenerInstancer(framework);
	Rocket::Core::Factory::RegisterEventListenerInstancer(
			eventListenerInstancer);
	eventListenerInstancer->RemoveReference();

	// Load and show the main document.
	mainMenu = context->LoadDocument(
			(baseDir + "data/misc/rocket-main.rml").c_str());
	if (mainMenu != NULL)
	{
		mainMenu->GetElementById("title")->SetInnerRML(mainMenu->GetTitle());
		mainMenu->Show();
		mainMenu->RemoveReference();
	}
}

void EventListener::ProcessEvent(Rocket::Core::Event& event)
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
	std::cout << "Event type: " << event.GetType().CString() << std::endl;
	std::cout << "Event value: " << mValue.CString() << std::endl;
	std::cout << "Event phase: " << phaseStr.CString() << std::endl;
	std::cout << "Event target element tag: "
			<< event.GetTargetElement()->GetTagName().CString() << std::endl;
	std::cout << "Event current element tag: "
			<< event.GetTargetElement()->GetTagName().CString() << std::endl;
	int pos = 0;
	Rocket::Core::String key, valueAsStr;
	Rocket::Core::Variant* value;
	while (event.GetParameters()->Iterate(pos, key, value))
	{
		valueAsStr = value->Get<Rocket::Core::String>();
		std::cout << "Event parameter: pos = " << pos << " key = \""
				<< key.CString() << "\"" << " value = " << valueAsStr.CString()
				<< std::endl;
	}
	std::cout << std::endl;
#endif

	if (mValue == "main::body::load_logo")
	{

	}
	else if (mValue == "main::button::start_game")
	{
		//close (i.e. unload) the main document.
		mainMenu->Close();
	}
	else if (mValue == "main::button::options")
	{
		mainMenu->Hide();
		// Load and show the options document.
		optionMenu = context->LoadDocument(
				(baseDir + "data/misc/rocket-options.rml").c_str());
		if (optionMenu != NULL)
		{
			optionMenu->GetElementById("title")->SetInnerRML(
					optionMenu->GetTitle());
			///update controls
			//graphics
			switch (graphicsQuality)
			{
			case 0:
				optionMenu->GetElementById("default_graphics")->SetAttribute(
						"checked", true);
				break;
			case 1:
				optionMenu->GetElementById("good_graphics")->SetAttribute(
						"checked", true);
				break;
			case 2:
				optionMenu->GetElementById("bad_graphics")->SetAttribute(
						"checked", true);
				break;
			default:
				break;
			}
			//audio
			Rocket::Core::Element* audioElem;
			audioElem = optionMenu->GetElementById("reverb_audio");
			audioReverb ?
					audioElem->SetAttribute("checked", true) :
					audioElem->RemoveAttribute("checked");
			audioElem = optionMenu->GetElementById("3d_audio");
			audio3d ?
					audioElem->SetAttribute("checked", true) :
					audioElem->RemoveAttribute("checked");
			//
			optionMenu->Show();
			optionMenu->RemoveReference();
		}
	}
	else if (mValue == "main::button::exit")
	{
		//close (i.e. unload) the main document.
		mainMenu->Close();
		//set PandaFramework exit flag
		mFramework->set_exit_flag();
	}
	else if (mValue == "options::body::load_logo")
	{

	}
	else if (mValue == "options::form::submit_options")
	{
		Rocket::Core::String paramValue;
		//check if ok or cancel
		paramValue = event.GetParameter<Rocket::Core::String>("submit",
				"cancel");
		if (paramValue == "ok")
		{
			//get controls values
			//graphics
			paramValue = event.GetParameter<Rocket::Core::String>("graphics",
					"default");
			if (paramValue == "good")
			{
				graphicsQuality = 1;
			}
			else if (paramValue == "bad")
			{
				graphicsQuality = 2;
			}
			else
			{
				//default
				graphicsQuality = 0;
			}
			//audio
			paramValue = event.GetParameter<Rocket::Core::String>("reverb",
					"NONE");
			paramValue == "true" ? audioReverb = true : audioReverb = false;
			paramValue = event.GetParameter<Rocket::Core::String>("3d", "NONE");
			paramValue == "true" ? audio3d = true : audio3d = false;
		}
		//close (i.e. unload) the options document.
		optionMenu->Close();
		//return to main document.
		mainMenu->Show();
	}
	else if (mValue == "options::input::bad_graphics")
	{
		// This event is sent from the 'onchange' of the bad graphics
		//radio button. It shows or hides the bad graphics warning message.
		Rocket::Core::ElementDocument* options_body =
				event.GetTargetElement()->GetOwnerDocument();
		if (options_body == NULL)
			return;

		Rocket::Core::Element* bad_warning = options_body->GetElementById(
				"bad_warning");
		if (bad_warning)
		{
			// The 'value' parameter of an onchange event is set to
			//the value the control would send if it was submitted;
			//so, the empty string if it is clear or to the 'value'
			//attribute of the control if it is set.
			if (event.GetParameter<Rocket::Core::String>("value", "").Empty())
			{
				bad_warning->SetProperty("display", "none");
			}
			else
			{
				bad_warning->SetProperty("display", "block");
			}
		}
	}
	else
	{
	}
}
