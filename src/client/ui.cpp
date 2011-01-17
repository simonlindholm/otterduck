#include "precomp.h"
#include "ui.h"

namespace{

float toScreenPos(Coord c) {
	return (float)(c >> 8);
}
Coord screenPosToTile(Coord c) {
	return Map::posToTile(c << 8);
}

const Coord minScrollLeft = 230;
const Coord minScrollRight = 230;
const Coord minScrollTop = 100;
const Coord minScrollBottom = 200;

}

UI::UI(CL_ResourceManager* res, CL_DisplayWindow* win)
	: resources(res), window(win), gc(window->get_gc()),
	otterLeft(gc, "game/otter-walk-left", resources),
	otterRight(gc, "game/otter-walk-right", resources),
	otterUpLeft(gc, "game/otter-walk-upleft", resources),
	otterUpRight(gc, "game/otter-walk-upright", resources),
	otterDownLeft(gc, "game/otter-walk-downleft", resources),
	otterDownRight(gc, "game/otter-walk-downright", resources),
	tileSprite(gc, "game/tiles", resources),
	scrollx(0), scrolly(0)
{
	otterLeft.set_delay(140);
	otterRight.set_delay(140);
	otterUpLeft.set_delay(140);
	otterUpRight.set_delay(140);
	otterDownLeft.set_delay(140);
	otterDownRight.set_delay(140);
}

void UI::drawMapSprite(CL_Sprite& spr, Position pos) {
	spr.draw(gc, toScreenPos(pos.x) - scrollx, toScreenPos(pos.y) - scrolly);
}

void UI::adjustScrolling(const Player& pl) {
	Position pos = pl.getPosition();
	Coord w = toScreenPos(pl.getWidth()), h = toScreenPos(pl.getHeight());
	const Coord maxx = gc.get_width() - minScrollRight - w;
	const Coord maxy = gc.get_height() - minScrollBottom - h;

	Coord oldx = toScreenPos(pos.x) - scrollx;
	Coord oldy = toScreenPos(pos.y) - scrolly;

	if (oldx < minScrollLeft)
		scrollx -= minScrollLeft - oldx;
	else if (oldx > maxx)
		scrollx -= maxx - oldx;

	if (oldy < minScrollTop)
		scrolly -= minScrollTop - oldy;
	else if (oldy > maxy)
		scrolly -= maxy - oldy;
}

void UI::paint(unsigned int delay, const GameState& gs) {
	const Map& map = gs.getMap();
	const Player& pl = gs.getPlayer();

	gc.clear(CL_Colorf::snow);

	adjustScrolling(pl);

	Coord fromx = screenPosToTile(scrollx);
	Coord tox = screenPosToTile(scrollx + gc.get_width()-1);
	Coord fromy = screenPosToTile(scrolly);
	Coord toy = screenPosToTile(scrolly + gc.get_height()-1);
	for (Coord tiley = fromy; tiley <= toy; ++tiley) {
		for (Coord tilex = fromx; tilex <= tox; ++tilex) {
			Position p(tilex, tiley);
			const Map::Tile& tile = map.getTileAt(p);
			tileSprite.set_frame(tile.getImage());
			drawMapSprite(tileSprite, Map::tileToPos(p));
		}
	}

	tileSprite.set_frame(0);

	int pose = pl.getPose();
	if (pose == 0) otterSprite = otterLeft;
	else if (pose == 1) otterSprite = otterRight;
	else if (pose == 2) otterSprite = otterUpLeft;
	else if (pose == 3) otterSprite = otterUpRight;
	else if (pose == 4) otterSprite = otterDownLeft;
	else if (pose == 5) otterSprite = otterDownRight;

	otterSprite.update(delay);
	Position plpos = pl.getPosition();
	drawMapSprite(otterSprite, plpos);
}
