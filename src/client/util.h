#pragma once

typedef long mcoord;
struct MPosition {
	mcoord x, y;
	MPosition(mcoord x, mcoord y) : x(x), y(y) {}
	MPosition() {}
};

typedef long tcoord;
struct TPosition {
	tcoord x, y;
	TPosition(tcoord x, tcoord y) : x(x), y(y) {}
	TPosition() {}
};

struct Rect {
	MPosition p;
	mcoord w, h;
	Rect(mcoord x, mcoord y, mcoord w, mcoord h) : p(x,y), w(w), h(h) {}
	Rect() {}
	MPosition getCenter() const { return MPosition(p.x + w/2, p.y + h/2); }
};
