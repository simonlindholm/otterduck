#pragma once
#include <utility>
#include <vector>
#include "util.h"
#include "gameobject.h"
#include "player.h"

class UI;
class Map;

class GameState {
	public:
		struct Event {
			enum Type {
				BeginMoveLeft,
				EndMoveLeft,
				BeginMoveRight,
				EndMoveRight,
				BeginLookUp,
				EndLookUp,
				Jump,
				Restart,
			};
		};

		void handleEvent(Event::Type e);
		void step(unsigned int delay);

		void setSavedState(const GameState* state);

		void playerDie();
		bool deadPlayer() const { return dead; }

		const Player& getPlayer() const { return pl.getAs<Player>(); }
		Player& getPlayer() { return pl.getAs<Player>(); }

		const Map& getMap() const { return map; }

		void drawObjects(UI& ui) const;

		GameState(const Map& map);
		void swap(GameState& other);
		GameState& operator=(const GameState& other);

	private:
		bool dead;

		const Map& map;
		const GameState* savedState;
		std::vector<GameObject> items;
		GameObject pl;
};
