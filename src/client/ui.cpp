#include "ui.h"
#include "map.h"

namespace {
	scoord toScreenPos(mcoord c) {
		return (scoord)(c >> 8);
	}
	tcoord screenPosToTile(scoord c) {
		return Map::posToTile(mcoord(c) << 8);
	}

	const scoord minScrollLeft = 245;
	const scoord minScrollRight = 245;
	const scoord minScrollTop = 115;
	const scoord minScrollBottom = 215;
}

UI::Event UI::Event::makeKeydown(int key) {
	Event ret;
	ret.type = Keydown;
	ret.key = key;
	return ret;
}

UI::Event UI::Event::makeKeyup(int key) {
	Event ret;
	ret.type = Keyup;
	ret.key = key;
	return ret;
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
	soundDeath("game/snd/death", resources),
	hasPlayedDeathSound(false), scrollx(0), scrolly(0)
{
	CL_Font font = CL_Font(gc, "tahoma", 50);
	restartText.add_text("Press R to restart.", font, CL_Colorf::white);
	layoutCentered(restartText, 135);
	font = CL_Font(gc, "tahoma", 100);
	restartHeader.add_text("You died.", font, CL_Colorf::red);
	layoutCentered(restartHeader, 45);

	otterLeft.set_delay(140);
	otterRight.set_delay(140);
	otterUpLeft.set_delay(140);
	otterUpRight.set_delay(140);
	otterDownLeft.set_delay(140);
	otterDownRight.set_delay(140);
}

void UI::makeEvent(Event e, const GameState& gs,
		std::queue<GameState::Event::Type>& eventQueue) const {
	if (e.type == Event::Keyup) {
		if (e.key == CL_KEY_UP)
			eventQueue.push(GameState::Event::EndLookUp);
		else if (e.key == CL_KEY_LEFT)
			eventQueue.push(GameState::Event::EndMoveLeft);
		else if (e.key == CL_KEY_RIGHT)
			eventQueue.push(GameState::Event::EndMoveRight);
	}
	else if (e.type == Event::Keydown) {
		if (e.key == CL_KEY_UP)
			eventQueue.push(GameState::Event::BeginLookUp);
		else if (e.key == CL_KEY_LEFT)
			eventQueue.push(GameState::Event::BeginMoveLeft);
		else if (e.key == CL_KEY_RIGHT)
			eventQueue.push(GameState::Event::BeginMoveRight);
		else if (e.key == CL_KEY_Z)
			eventQueue.push(GameState::Event::Jump);
		else if (e.key == CL_KEY_R)
			eventQueue.push(GameState::Event::Restart);
	}
}

void UI::layoutCentered(CL_SpanLayout& layout, int y) {
	layout.layout(gc, gc.get_width());
	int centerX = (gc.get_width() - layout.get_size().width) / 2;
	layout.set_position(CL_Point(centerX, y));
}

void UI::drawMapSprite(CL_Sprite& spr, MPosition pos) {
	scoord x = toScreenPos(pos.x) - scrollx;
	scoord y = toScreenPos(pos.y) - scrolly;
	spr.draw(gc, (int)x, (int)y);
}

void UI::drawCenteredMapSprite(CL_Sprite& spr, MPosition pos) {
	scoord x = toScreenPos(pos.x) - (spr.get_width() / 2) - scrollx;
	scoord y = toScreenPos(pos.y) - (spr.get_height() / 2) - scrolly;
	spr.draw(gc, (int)x, (int)y);
}

void UI::adjustScrolling(const Player& pl) {
	MPosition p = pl.getPosition();
	const scoord maxx = gc.get_width() - minScrollRight;
	const scoord maxy = gc.get_height() - minScrollBottom;

	scoord oldx = toScreenPos(p.x) - scrollx;
	scoord oldy = toScreenPos(p.y) - scrolly;

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
	if (pose >= 6) pose -= 6; // temp hack
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

	gc.clear(CL_Colorf::red);

	adjustScrolling(pl);

	tcoord fromx = screenPosToTile(scrollx);
	tcoord tox = screenPosToTile(scrollx + gc.get_width()-1);
	tcoord fromy = screenPosToTile(scrolly);
	tcoord toy = screenPosToTile(scrolly + gc.get_height()-1);
	for (tcoord tiley = fromy; tiley <= toy; ++tiley) {
		for (tcoord tilex = fromx; tilex <= tox; ++tilex) {
			TPosition p(tilex, tiley);
			const Map::Tile& tile = map.getTileAt(p);
			tileSprite.set_frame(tile.getImage());
			drawMapSprite(tileSprite, Map::tileToPos(p));
		}
	}

	gs.drawObjects(*this);

	if (gs.deadPlayer()) {
		if (!hasPlayedDeathSound) {
			soundDeath.play();
			hasPlayedDeathSound = true;
		}
		restartHeader.draw_layout(gc);
		restartText.draw_layout(gc);
	}
	else {
		// Reset the death state
		hasPlayedDeathSound = false;
	}
}
