#pragma once
#include "precomp.h"

typedef long Coord;

struct Position {
	Coord x, y;
	Position(Coord x, Coord y) : x(x), y(y) {}
	Position() {}
};
