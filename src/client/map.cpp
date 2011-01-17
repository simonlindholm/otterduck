#include "precomp.h"
#include <fstream>
#include <sstream>
#include "map.h"
#include "exceptions.h"

Map::Map(const std::string& str) {
	// Read the map from file
	std::ifstream fin;
	fin.exceptions(fin.eofbit | fin.failbit | fin.badbit);
	try {
		fin.open(str);
		fin >> w >> h;
		tiles.assign(h, std::vector<unsigned char>(w, 0));
		fin.ignore(1000, '\n');
		for (Coord i = 0; i < h; ++i) {
			for (Coord j = 0; j < w; ++j) {
				char ch;
				fin.get(ch);
				tiles[i][j] = tileFromChar(ch);
			}
			fin.ignore(1000, '\n');
		}
	}
	catch (std::ifstream::failure e) {
		throw ConfigException("Failed to read map.");
	}

	// Construct the different kinds of tiles
	tileSorts[Tile::EMPTY] = std::shared_ptr<Tile>(new Tile(1, true));
	tileSorts[Tile::BLOCK] = std::shared_ptr<Tile>(new Tile(0, false));
}

Map::Tile::Tile(unsigned int img, bool e)
	: empty(e), image(img)
{}

const Map::Tile& Map::getTileAt(Position pos) const {
	if (pos.x < 0 || pos.x >= w || pos.y < 0 || pos.y >= h) return *tileSorts[Tile::EMPTY];
	return *tileSorts[tiles[pos.y][pos.x]];
}

unsigned char Map::tileFromChar(char ch) {
	switch (ch) {
		case ' ': return Tile::EMPTY;
		case 'X': return Tile::BLOCK;
	}
	std::ostringstream oss;
	oss << "Invalid map: Invalid character '" << ch << "'.";
	throw ConfigException(oss.str());
}

Coord Map::tileToPos(Coord tile) {
	return tile << 13;
}

Position Map::tileToPos(Position p) {
	return Position(tileToPos(p.x), tileToPos(p.y));
}

Coord Map::posToTile(Coord tile) {
	return tile >> 13;
}

Position Map::posToTile(Position p) {
	return Position(posToTile(p.x), posToTile(p.y));
}
