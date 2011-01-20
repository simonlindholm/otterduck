#pragma once

typedef long Coord;

struct Position {
	Coord x, y;
	Position(Coord x, Coord y) : x(x), y(y) {}
	Position() {}
};

struct Rect {
	Position p;
	Coord w, h;
	Rect(Coord x, Coord y, Coord w, Coord h) : p(x,y), w(w), h(h) {}
	Rect() {}
	Position getCenter() const { return Position(p.x + w/2, p.y + h/2); }
};
