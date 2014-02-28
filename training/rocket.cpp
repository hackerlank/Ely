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
#include <throw_event.h>
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
	LoadFonts("/REPOSITORY/KProjects/libRocket/Samples/assets/");

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
			reinterpret_cast<void*>(NULL));

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
class EventListener: public Rocket::Core::EventListener
{
public:
	EventListener(const Rocket::Core::String& value);
	virtual ~EventListener();

protected:
	virtual void ProcessEvent(Rocket::Core::Event& event);

private:
	Rocket::Core::String mValue;
};

class EventListenerInstancer: public Rocket::Core::EventListenerInstancer
{
public:
	EventListenerInstancer();
	virtual ~EventListenerInstancer();

	/// Instances a new event handle for Invaders.
	virtual Rocket::Core::EventListener* InstanceEventListener(
			const Rocket::Core::String& value, Rocket::Core::Element* element);

	/// Destroys the instancer.
	virtual void Release();
};

Rocket::Core::ElementDocument *mainMenu = NULL, *optionMenu = NULL;
EventListenerInstancer* eventListenerInstancer = NULL;

void showMainMenu(const Event* e, void* data)
{
	if (mainMenu)
	{
		return;
	}
	//register EventListenerInstancer
	eventListenerInstancer = new EventListenerInstancer;
	Rocket::Core::Factory::RegisterEventListenerInstancer(
			eventListenerInstancer);
	eventListenerInstancer->RemoveReference();

	// Load and show the main document.
	mainMenu = context->LoadDocument(
			(baseDir + "data/models/rocket-main.rml").c_str());
	if (mainMenu != NULL)
	{
		mainMenu->GetElementById("title")->SetInnerRML(mainMenu->GetTitle());
		mainMenu->Show();
		mainMenu->RemoveReference();
	}
}

EventListener::EventListener(const Rocket::Core::String& value) :
		mValue(value)
{
}

EventListener::~EventListener()
{
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
	Rocket::Core::String key, strRepr;
	Rocket::Core::Variant* value;
	while (event.GetParameters()->Iterate(pos, key, value))
	{
		strRepr = value->Get<Rocket::Core::String>();
		std::cout << "Event parameter: pos = " << pos << " key = \""
				<< key.CString() << "\"" << " value = " << strRepr.CString()
				<< std::endl;
	}
	std::cout << std::endl;
#endif

	if (mValue == "main::body::load_logo")
	{

	}
	else if (mValue == "main::button::start_game")
	{
		//hide and unload the main document.
		mainMenu->Close();
		context->UnloadDocument(mainMenu);
		mainMenu = NULL;
	}
	else if (mValue == "main::button::options")
	{
		mainMenu->Hide();
		// Load and show the options document.
		optionMenu = context->LoadDocument(
				(baseDir + "data/models/rocket-options.rml").c_str());
		if (optionMenu != NULL)
		{
			//populate controls
			optionMenu->GetElementById("title")->SetInnerRML(optionMenu->GetTitle());
			optionMenu->Show();
			optionMenu->RemoveReference();
		}
	}
	else if (mValue == "main::button::exit")
	{
		throw_event("window-event");
	}
	else if (mValue == "options::body::load_logo")
	{

	}
	else
	{
	}
}

EventListenerInstancer::EventListenerInstancer()
{
}

EventListenerInstancer::~EventListenerInstancer()
{
}

// Instances a new event handle for Invaders.
Rocket::Core::EventListener* EventListenerInstancer::InstanceEventListener(
		const Rocket::Core::String& value, Rocket::Core::Element* element)
{
	return new EventListener(value);
}

// Destroys the instancer.
void EventListenerInstancer::Release()
{
	delete this;
}
