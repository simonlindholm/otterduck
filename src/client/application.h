#pragma once
#include <vector>
#include <list>
#include <queue>
#include "cl.h"
#include "ui.h"
#include "gamestate.h"

class Application {
	public:
		void onWindowClose();
		void onKeydown(const CL_InputEvent& event, const CL_InputState&);
		void onKeyup(const CL_InputEvent& event, const CL_InputState&);
		virtual int main(const std::vector<CL_String>& args);

	private:
		static const unsigned int timestep;
		std::list<UI::Event> uiEventQueue;
		std::queue<GameState::Event::Type> eventQueue;
		bool quit;
};
