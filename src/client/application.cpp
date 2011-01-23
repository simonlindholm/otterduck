#include <iostream>
#include "application.h"
#include "framerate_counter.h"
#include "gamestate.h"
#include "map.h"
#include "ui.h"

const unsigned int Application::timestep = 10;

void Application::onWindowClose() {
	quit = true;
}

void Application::onKeydown(const CL_InputEvent& event, const CL_InputState&) {
	if (event.id == CL_KEY_ESCAPE) {
		// For now, handle quitting locally in Application, because it's
		// easier that way.
		quit = true;
	}
	else {
		if (!uiEventQueue.empty()) {
			// Work around the Clanlib bug where key events are sent like
			// DOWN .. UP DOWN .. UP DOWN .. UP
			// on Linux, instead of
			// DOWN .. DOWN .. DOWN .. UP.
			const UI::Event& e = uiEventQueue.back();
			if (e.type == UI::Event::Keyup && e.key == event.id) {
				uiEventQueue.pop_back();
				return;
			}
		}

		// Don't include repeat events
		if (event.repeat_count == 0)
			uiEventQueue.push_back(UI::Event::makeKeydown(event.id));
	}
}

void Application::onKeyup(const CL_InputEvent& event, const CL_InputState&) {
	uiEventQueue.push_back(UI::Event::makeKeyup(event.id));
}

int Application::main(const std::vector<CL_String>& args) {
	// Set up display, resources, etc.
	CL_SoundOutput soundOutput(44100);
	CL_DisplayWindow window("The Cracking Good Adventures of Otter and Duck", 640, 480);

	CL_ResourceManager resources;
	resources.load("resources.xml");

	// Create a saved and a current game object, with a test map.
	// (In the future, there should be a menu here first, and the
	// creation of the game object should be handled somewhere in the
	// state machine.)
	Map map("maps/test.map");
	GameState savedGs(map);
	savedGs.setSavedState(&savedGs);
	GameState gs = savedGs;

	// Create the UI
	UI ui(&resources, &window);

	// Run the game loop and relevant logic
	// (In the future, Application should probably only contain a state
	// machine, delegating the game loop logic to somewhere else.)

	// Set up event listeners, because this is the only event polling system
	// ClanLib supports. -.-
	quit = false;
	CL_Slot slot_window_close = 
		window.sig_window_close().connect(this, &Application::onWindowClose);

	CL_InputContext ic = window.get_ic();
	CL_InputDevice keyboard = ic.get_keyboard();
	CL_Slot slot_keydown = 
		keyboard.sig_key_down().connect(this, &Application::onKeydown);
	CL_Slot slow_keyup =
		keyboard.sig_key_up().connect(this, &Application::onKeyup);

	unsigned int ptime = CL_System::get_time();
	FramerateCounter frcounter;

	while (!quit) {
		unsigned int ntime = CL_System::get_time();
		int dif = ntime - ptime, odif = dif;

		// Transform all the UI events into game events.
		while (!uiEventQueue.empty()) {
			UI::Event e = uiEventQueue.front();
			ui.makeEvent(e, gs, eventQueue);
			uiEventQueue.pop_front();
		}

		// Send information about all events that have happened.
		while (!eventQueue.empty()) {
			GameState::Event::Type e = eventQueue.front();
			gs.handleEvent(e);
			eventQueue.pop();
		}

		// Step the gamestate in 'timestep' large steps.
		while (dif > (int)timestep) {
			dif -= timestep;
			gs.step(timestep);
		}
		if (dif > 0) {
			gs.step((unsigned int)dif);
			ptime = ntime;
		}

		// Calculate the actual time stepped.
		if (odif < 0) odif = 0;

		// Draw the interface to screen.
		ui.paint((unsigned int)odif, gs);

		// Perform some deep ClanLib magic.
		window.flip();
		frcounter.frame_shown();
		CL_KeepAlive::process();
	}

	return 0;
}
