#pragma once
#include "precomp.h"
#include <vector>
#include <list>
#include "gamestate.h"

class Application {
	private:
		static const unsigned int timestep;
		std::list<GameState::Event> eventQueue;
		bool quit;
	public:
		void onWindowClose();
		void onKeydown(const CL_InputEvent& event, const CL_InputState&);
		void onKeyup(const CL_InputEvent& event, const CL_InputState&);
		virtual int main(const std::vector<CL_String>& args);
};
