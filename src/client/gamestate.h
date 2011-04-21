#pragma once
#include <utility>
#include <unordered_map>
#include <vector>
#include "util.h"
#include "golist.h"
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
		bool deadPlayer() const { return data.dead; }

		const Map* getMap() const { return map; }
		const Player& getPlayer() const { return *pl; }

		void drawObjects(UI& ui) const;

		GameState(const Map* map);

		void markState();
		void restoreState();
		void markDirty(RawGameObject* item);

	private:
		const Map* map;

		GameState& operator=(const GameState& other) = delete;
		GameState(const GameState& other) = delete;

		GOList items;
		Player* pl;

		struct Data {
			bool dead;
		} data, savedData;
};
