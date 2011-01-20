#pragma once
#include <utility>
#include <memory>
#include "util.h"
#include "gameobject.h"
#include "player.h"

class UI;
class Map;

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

		void drawObjects(UI& ui) const;

		GameState(const Map& map);

	private:
		enum {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_LAST};
		bool heldKeys[KEY_LAST];
		signed char xmov, ymov;

		const Map& map;
		std::vector<std::shared_ptr<GameObject>> items;
		Player pl;
};
