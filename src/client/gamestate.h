#pragma once
#include "precomp.h"
#include <utility>
#include "map.h"
#include "player.h"

class GameState {
	public:
		// Event struct, used for sending events to GameState
		struct Event {
			enum Type {
				Keydown,
				Keyup,
			};
			Type type;
			int key;
			static Event makeKeydown(int key);
			static Event makeKeyup(int key);
		};

		void handleEvent(Event e);
		void step(unsigned int delay);

		const Player& getPlayer() const { return pl; }
		const Map& getMap() const { return map; }

		GameState(const Map& map);

	private:
		enum {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_LAST};
		bool heldKeys[KEY_LAST];
		signed char xmov, ymov;

		const Map& map;
		Player pl;
};
