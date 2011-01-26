#pragma once
#include <vector>
#include <string>
#include <memory>
#include "util.h"
#include "resources.h"

class Map {
	public:
		class Tile {
			friend class Map;
			private:
				enum {
					EMPTY,
					BLOCK,
					SPIKE,
					LAST
				};

				bool empty, deadly;
				Resources::Img image;

				// Tiles can only be constructed from Map
				Tile(Resources::Img image, bool empty, bool deadly);
				Tile() = default;
				Tile& operator=(const Tile& other) = delete;
				Tile(const Tile& other) = delete;

			public:
				bool isEmpty() const { return empty; }
				Resources::Img getImage() const { return image; }
				bool isDeadly() const { return deadly; }
		};

		Map(const std::string& fname);

		Map(const Map& other) = delete;
		Map& operator=(const Map& other) = delete;

		const Tile& getTileAt(TPosition pos) const;

		// Get the top-right map position of a tile position.
		static mcoord tileToPos(tcoord tile);
		static MPosition tileToPos(TPosition tile);

		// Convert a tile position to a map position.
		static tcoord posToTile(mcoord tile);
		static TPosition posToTile(MPosition tile);

	private:
		tcoord w, h;
		std::shared_ptr<Tile> tileSorts[Tile::LAST];
		std::vector<std::vector<unsigned char>> tiles;

		static unsigned char tileFromChar(char ch);
};
