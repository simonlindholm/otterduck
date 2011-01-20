#include "ui.h"
#include "map.h"

namespace{

float toScreenPos(Coord c) {
	return (float)(c >> 8);
}
Coord screenPosToTile(Coord c) {
	return Map::posToTile(c << 8);
}

const Coord minScrollLeft = 245;
const Coord minScrollRight = 245;
const Coord minScrollTop = 115;
const Coord minScrollBottom = 215;

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
	int x = toScreenPos(pos.x) - scrollx;
	int y = toScreenPos(pos.y) - scrolly;
	spr.draw(gc, x, y);
}

void UI::drawCenteredMapSprite(CL_Sprite& spr, Position pos) {
	int x = toScreenPos(pos.x) - (spr.get_width() / 2) - scrollx;
	int y = toScreenPos(pos.y) - (spr.get_height() / 2) - scrolly;
	spr.draw(gc, x, y);
}

void UI::adjustScrolling(const Player& pl) {
	Position p = pl.getPosition();
	const Coord maxx = gc.get_width() - minScrollRight;
	const Coord maxy = gc.get_height() - minScrollBottom;

	Coord oldx = toScreenPos(p.x) - scrollx;
	Coord oldy = toScreenPos(p.y) - scrolly;

	if (oldx < minScrollLeft)
		scrollx -= minScrollLeft - oldx;
	else if (oldx > maxx)
		scrollx -= maxx - oldx;

	if (oldy < minScrollTop)
		scrolly -= minScrollTop - oldy;
	else if (oldy > maxy)
		scrolly -= maxy - oldy;
}

void UI::drawPlayer(const Player& pl) {
	int pose = pl.getPose();
	if (pose == 0) otterSprite = otterLeft;
	else if (pose == 1) otterSprite = otterRight;
	else if (pose == 2) otterSprite = otterUpLeft;
	else if (pose == 3) otterSprite = otterUpRight;
	else if (pose == 4) otterSprite = otterDownLeft;
	else if (pose == 5) otterSprite = otterDownRight;

	otterSprite.update();
	drawCenteredMapSprite(otterSprite, pl.getPosition());
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

	gs.drawObjects(*this);
}
