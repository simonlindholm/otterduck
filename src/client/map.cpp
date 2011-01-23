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
		for (tcoord i = 0; i < h; ++i) {
			for (tcoord j = 0; j < w; ++j) {
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
	tileSorts[Tile::EMPTY] = std::shared_ptr<Tile>(new Tile(1, true, false));
	tileSorts[Tile::BLOCK] = std::shared_ptr<Tile>(new Tile(0, false, false));
	tileSorts[Tile::SPIKE] = std::shared_ptr<Tile>(new Tile(2, true, true));
}

Map::Tile::Tile(unsigned int image, bool empty, bool deadly)
	: empty(empty), deadly(deadly), image(image)
{}

const Map::Tile& Map::getTileAt(TPosition pos) const {
	if (pos.x < 0 || pos.x >= w || pos.y < 0 || pos.y >= h) return *tileSorts[Tile::EMPTY];
	return *tileSorts[tiles[pos.y][pos.x]];
}

unsigned char Map::tileFromChar(char ch) {
	switch (ch) {
		case ' ': return Tile::EMPTY;
		case 'X': return Tile::BLOCK;
		case '^': return Tile::SPIKE;
	}
	std::ostringstream oss;
	oss << "Invalid map: Invalid character '" << ch << "'.";
	throw ConfigException(oss.str());
}

mcoord Map::tileToPos(tcoord tile) {
	return tile << 13;
}
MPosition Map::tileToPos(TPosition pos) {
	return MPosition(tileToPos(pos.x), tileToPos(pos.y));
}

tcoord Map::posToTile(mcoord tile) {
	return tile >> 13;
}
TPosition Map::posToTile(MPosition pos) {
	return TPosition(posToTile(pos.x), posToTile(pos.y));
}
