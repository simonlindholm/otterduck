#pragma once
#include "precomp.h"
#include <string>
#include <memory>
#include "util.h"

class Map {
	public:
		class Tile {
			friend class Map;
			private:
				enum {
					EMPTY,
					BLOCK,
					LAST
				};

				bool empty;
				unsigned int image;

				// Tiles can only be constructed from Map
				Tile(unsigned int img, bool e);
				Tile() = default;
				Tile& operator=(const Tile& other) = delete;
				Tile(const Tile& other) = delete;

			public:
				bool isEmpty() const { return empty; }
				unsigned int getImage() const { return image; }
		};

		Map(const std::string& fname);

		Map(const Map& other) = delete;
		Map& operator=(const Map& other) = delete;

		const Tile& getTileAt(Position pos) const;

		// Get the top-right map position of a tile position.
		static Coord tileToPos(Coord tile);
		static Position tileToPos(Position tile);

		// Convert a tile position to a map position.
		static Coord posToTile(Coord tile);
		static Position posToTile(Position tile);

	private:
		Coord w, h;
		std::shared_ptr<Tile> tileSorts[Tile::LAST];
		std::vector<std::vector<unsigned char>> tiles;

		static unsigned char tileFromChar(char ch);
};
